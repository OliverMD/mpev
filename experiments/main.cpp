#include <experimental/filesystem>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/GlobalExecutor.h>
#include <glog/logging.h>
#include <iostream>

#include "ExperimentRunner.h"

int main(int argc, char *argv[]) {

  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;
  if (argc != 2) {
    std::cout << "Invalid arguments" << std::endl;
    std::cout << "Example: ./experiments path/to/config.toml" << std::endl;
    return 1;
  }

  std::cout << "Waiting for input..." << std::endl;
  std::string in;
  std::cin >> in;
  std::cout << "Input received! Starting..." << std::endl;

  RunConfig runConfig;
  try {
    runConfig = parseTomlConfig(argv[1]);
  } catch (std::runtime_error& e) {
    std::cout << "Error parsing config: " << e.what() << std::endl;
    return 1;
  }
  LOG(INFO) << "Running from config";
  auto exec = std::make_shared<folly::CPUThreadPoolExecutor>(8);
  folly::setCPUExecutor(exec);
  runFromConfig(runConfig, argv[1], 8);

  return 0;
}