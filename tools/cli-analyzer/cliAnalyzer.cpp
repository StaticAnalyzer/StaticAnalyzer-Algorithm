#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "myanalysis/AnalysisFactory.h"
#include "CLI11.h"
#include "World.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

int main(int argc, char *argv[])
{
    CLI::App app;

    std::string sourceDir, std="c++11", outputDir;
    std::vector<std::string> includeDirs;
    bool verbose = false;

    app.add_option("-S,--source-dir,", sourceDir,
                   "root path of source code")->required();

    app.add_option("-I,--include-dir", includeDirs,
                   "include directories");

    app.add_option("--std,--standard", std,
                   "c/c++ language standard");

    app.add_option("-v,--verbose", verbose,
                   "print verbose runtime logs");

    app.add_option("-O,--output", outputDir,
                   "report output file, default is stdout");

    CLI11_PARSE(app, argc, argv);

    std::transform(includeDirs.begin(), includeDirs.end(), includeDirs.begin(), [](std::string& s) { return "-I" + s; });
    my_analysis::AnalysisFactory analysisFactory(sourceDir, "", std, includeDirs);
    if (!verbose) {
        analyzer::World::getLogger().disable();
    }


    std::ostream *outStream = &std::cout;
    std::ofstream outputFile;
    if (!outputDir.empty()) {
        outputFile.open(outputDir + "/report.txt");
        outStream = &outputFile;
    }

    std::vector<std::unique_ptr<my_analysis::Analysis>> analysisList;
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

        *outStream << MAGENTA << "======================" << uni_result.getAnalysisType() << "======================" << RESET << std::endl;
        for (const auto& [filename, results] : uni_result.getFileAnalyseResults()) {
            if (filename.empty()) continue; // TODO: find why filename is empty
            // load file
            std::vector<std::string> lines;
            std::ifstream file(filename);
            std::string line;
            while (std::getline(file, line)) {
                lines.push_back(line);
            }

            *outStream << BLUE << filename << ":" << RESET << std::endl;
            for (auto& result : results) {
                switch (result.getSeverity()) {
                    case my_analysis::AnalysisResult::Severity::Hint:
                        *outStream << GREEN << "Hint: " << RESET;
                        break;
                    case my_analysis::AnalysisResult::Severity::Warning:
                        *outStream << YELLOW << "Warning: " << RESET;
                        break;
                    case my_analysis::AnalysisResult::Severity::Error:
                        *outStream << RED << "Error: " << RESET;
                        break;
                    case my_analysis::AnalysisResult::Severity::Info:
                        *outStream << CYAN << "Info: " << RESET;
                        break;
                }
                *outStream << "[" << result.getStartLine() << ":" << result.getStartColumn() << "] "
                            << result.getMessage()
                            << " (" << filename << ")" << std::endl;
                *outStream << lines[result.getStartLine() - 1] << std::endl;
                *outStream << std::string(std::max(result.getStartColumn() - 1, 0), ' ') << "^" << std::string(std::max(result.getEndColumn() - result.getStartColumn() - 1, 0), '~') << std::endl;
            }
        }
    }
}