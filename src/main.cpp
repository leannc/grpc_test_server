//
// Created by wangkang on 2023/12/3.
//
#include "TestService.h"
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

int main() {
  std::string server_address("0.0.0.0:50051");
  TestService server;
  // ServerBuilder builder;

  std::cout << "Hello, World!" << std::endl;
  return 0;
}
