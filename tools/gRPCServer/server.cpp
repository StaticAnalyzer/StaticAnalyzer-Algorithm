#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <random>
#include "json.hpp"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <google/protobuf/util/json_util.h>

#include "algservice.grpc.pb.h"

#include "myanalysis/AnalysisFactory.h"
#include "World.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using algservice::AnalyseRequest;
using algservice::AnalyseResponse;
using algservice::AlgAnalyseResult;
using algservice::FileAnalyseResults;
using algservice::AnalyseResultEntry;
using algservice::AlgService;

#if __has_include(<filesystem>)

#include <filesystem>

namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
error "Missing the <filesystem> header."
#endif

class AlgServiceUtils {
public:
    static std::string RandomStr(int len) {
        std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
        std::random_device rd;
        std::mt19937 generator(rd());
        std::string result;
        result.resize(len);
        for (int i = 0; i < len; i++)
            result[i] = str[generator() % (str.size() - 1)];
        return result;
    }
};

class AlgServiceImpl final : public AlgService::Service {
    Status Analyse(ServerContext *context, const AnalyseRequest *request,
                   AnalyseResponse *reply) override {
        const std::string& file = request->file();

        // 保存文件到本地
        std::string dir = AlgServiceUtils::RandomStr(20);
        fs::path project_root(dir);
        fs::create_directories(project_root);  // 创建目录

        std::string code_filename = dir + ".tar.gz";
        std::ofstream code_out(project_root / code_filename, std::ios::binary);
        code_out.write(file.c_str(), file.length());
        code_out.close();

        // 解压缩文件
        std::string cmd = "tar -zxvf " + (project_root / code_filename).string() + " -C " + project_root.string();
        if (!execCommand(cmd)) {
            clearTempFile(project_root.string());
            reply->set_code(-1);
            reply->set_msg("解压缩文件失败");
            return Status::OK;
        }

        // 执行算法
        try {
            auto configStr = request->config().empty() ? "{}" : request->config();
            auto config = nlohmann::json::parse(configStr);
            auto defMacros = config.value("defMacros", std::vector<std::string>());
            auto includes = config.value("includeDirs", std::vector<std::string>());
            auto std = config.value("std", std::string("c++11"));

            std::vector<std::string> args;
            args.emplace_back("-I/usr/lib/llvm-17/lib/clang/17/include");
            for (const auto &include : includes) {
                fs::path includePath(include);
                if (includePath.is_relative())
                    args.emplace_back("-I" + (project_root / include).string());
                else
                    args.emplace_back("-I" + include);
            }
            for (const auto &defMacro : defMacros) {
                args.emplace_back("-D" + defMacro);
            }

            std::vector<std::unique_ptr<my_analysis::Analysis>> analysisList;
            my_analysis::AnalysisFactory analysisFactory(project_root.string(),
                                                         "",
                                                         std, args);

            analysisList.push_back(analysisFactory.createUseBeforeDefAnalysis());
            analysisList.push_back(analysisFactory.createArithmeticIntensityAnalysis());
            analysisList.push_back(analysisFactory.createDeadCodeEliminationAnalysis());
            analysisList.push_back(analysisFactory.createMemoryLeakAnalysis());
            analysisList.push_back(analysisFactory.createNullPointerDereferenceAnalysis());
            analysisList.push_back(analysisFactory.createDivisionByZeroAnalysis());
            analysisList.push_back(analysisFactory.createArrayIndexOutOfBoundsAnalysis());

            for (const auto& analysis : analysisList) {
                analysis->analyze();
                const auto &uni_result = analysis->getResult();

                AlgAnalyseResult &&alg_result = convertToAlgAnalyseResult(uni_result, project_root.string());
                reply->add_alganalyseresults()->CopyFrom(alg_result);
            }
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            clearTempFile(project_root.string());
            reply->set_code(-1);
            reply->set_msg(std::string("执行异常：") + e.what());
            return Status::OK;
        }

        // 清除临时文件
        clearTempFile(project_root.string());

        reply->set_code(0);
        return Status::OK;
    }

    static bool execCommand(std::string cmd) {
        std::cout << "execCommand: " << cmd << std::endl;
        int rt = system(cmd.c_str());
        return rt == 0;
    }

    static void clearTempFile(std::string dir) {
        std::string cmd = "rm -rf " + dir;
        execCommand(cmd);
    }

    static AlgAnalyseResult
    convertToAlgAnalyseResult(const my_analysis::AnalysisResult &result, const std::string &root) {
        AlgAnalyseResult alg_result;

        alg_result.set_analysetype(result.getAnalysisType());
        alg_result.set_code(result.getCode());
        alg_result.set_msg(result.getMsg());

        for (const auto &[filename, file_result_raw]: result.getFileAnalyseResults()) {
            if (filename.empty()) {
                std::cout << "Warning: filename is empty" << std::endl;
                continue;
            }
            FileAnalyseResults file_result;
            for (const auto &result_entry_raw: file_result_raw) {
                AnalyseResultEntry &&result_entry = convertToAnalyseResultEntry(result_entry_raw);
                file_result.add_analyseresults()->CopyFrom(result_entry);
            }
            std::string real_filename(filename);
            removePrefix(real_filename, root);
            removePrefix(real_filename, "/");

            alg_result.mutable_fileanalyseresults()->emplace(real_filename, file_result);
        }

        return alg_result;
    }

    static AnalyseResultEntry convertToAnalyseResultEntry(const my_analysis::AnalysisResult::ResultUnit &entry) {
        AnalyseResultEntry result_entry;

        result_entry.set_startline(entry.getStartLine());
        result_entry.set_endline(entry.getEndLine());
        result_entry.set_startcolumn(entry.getStartColumn());
        result_entry.set_endcolumn(entry.getEndColumn());
        result_entry.set_message(entry.getMessage());
        result_entry.set_severity(toString(entry.getSeverity()));

        return result_entry;
    }

    static std::string toString(const my_analysis::AnalysisResult::Severity servity) {
        switch (servity) {
            case my_analysis::AnalysisResult::Severity::Error:
                return "Error";
            case my_analysis::AnalysisResult::Severity::Warning:
                return "Warning";
            case my_analysis::AnalysisResult::Severity::Info:
                return "Info";
            case my_analysis::AnalysisResult::Severity::Hint:
                return "Hint";
            default:
                return "UNKNOWN";
        }
    }

    static void removePrefix(std::string &str, const std::string &prefix) {
        if (str.rfind(prefix, 0) == 0)
            str.erase(0, prefix.length());
    }
};

int main() {
    analyzer::World::getLogger().disable();

    std::string server_address("0.0.0.0:8081");
    AlgServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}