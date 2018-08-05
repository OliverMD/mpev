#include <experimental/filesystem>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/GlobalExecutor.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iostream>

#include "ExperimentRunner.h"

DEFINE_string(configFile, "./config.toml", "The config file to use");
DEFINE_bool(wait, false, "Wait for input on launch? Useful for profiling");
DEFINE_uint32(numThreads, 8, "The number of threads to use");

int main(int argc, char *argv[]) {

  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = true;
  gflags::SetUsageMessage("A tool for running coevolution experiments");
  gflags::SetVersionString("0.1.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_wait) {
    std::cout << "Waiting for input..." << std::endl;
    std::string in;
    std::cin >> in;
    std::cout << "Input received! Starting..." << std::endl;
  }

  RunConfig runConfig;
  try {
    runConfig = parseTomlConfig(FLAGS_configFile);
  } catch (std::runtime_error& e) {
    LOG(FATAL) << "Error parsing config: " << e.what();
    return 1;
  }
  LOG(INFO) << "Running from config at: " << FLAGS_configFile;
  auto exec = std::make_shared<folly::CPUThreadPoolExecutor>(FLAGS_numThreads);
  folly::setCPUExecutor(exec);
  try {
    runFromConfig(runConfig, FLAGS_configFile);
  } catch (std::exception &e) {
    LOG(FATAL) << "Error running experiments: " << e.what();
  }

  return 0;
}