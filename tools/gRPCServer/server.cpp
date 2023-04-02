#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <random>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "algservice.grpc.pb.h"

#include "analysis/AnalysisFactory.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using algservice::JustReturnRequest;
using algservice::JustReturnResponse;
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
  Status JustReturn(ServerContext* context, const JustReturnRequest* request,
                    JustReturnResponse* reply) override {
    std::string file = request->file();
    std::string config = request->config();

    // 保存文件到本地
    std::string dir = AlgServiceUtils::RandomStr(20);
    fs::path project_root(dir);
    fs::path source_dir = project_root / "source";
    fs::create_directories(source_dir);  // 创建目录

    std::string code_filename = dir + ".tar.gz";
    std::ofstream code_out(project_root / code_filename, std::ios::binary);
    code_out.write(file.c_str(), file.length());
    code_out.close();

    // 解压缩文件
    std::string cmd = "tar -zxvf " + (project_root / code_filename).string() + " -C " + source_dir.string();
    system(cmd.c_str());

    // 保存配置文件
    std::string config_filename = "config.txt";
    std::ofstream config_out(project_root / config_filename, std::ios::binary);
    config_out.write(config.c_str(), config.length());
    config_out.close();

    // 生成ast文件和astList.txt，这里先假定只有一个文件
    fs::directory_iterator dir_ite(source_dir);
    std::list<std::string> ast_list;
    for(const auto& ite : dir_ite)
    {
      if(ite.status().type() == fs::file_type::regular)
      {
        std::string filename = ite.path().filename().string();
        // filenmae以.cpp结尾
        if(filename.rfind(".cpp") != filename.length() - 4)
          continue;

        std::string ast_path = ite.path().parent_path().string() + "/" + filename + ".ast";
        std::string cmd = "clang++ -emit-ast -c " + ite.path().string() + " -o " + ast_path;
        system(cmd.c_str());
        ast_list.push_back(ast_path);
      }
    }
    std::ofstream astlist_out(project_root / "astlist.txt", std::ios::binary);
    for(const auto& ite : ast_list)
    {
      astlist_out << ite << std::endl;
    }
    astlist_out.close();


    // 执行算法
    analysis::AnalysisFactory analysisFactory(
        project_root / "astlist.txt",
        project_root / "config.txt");
    std::unique_ptr<analysis::Analysis> echo = analysisFactory.createEchoAnalysis();
    *reply->mutable_result() = echo->analyze();

    // 清除临时文件
    cmd = "rm -rf " + project_root.string();
    system(cmd.c_str());

    return Status::OK;
  }
};

int main()
{
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