//
// Created by wangkang on 2023/12/3.
//
#include "TestService.h"
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

int main() {
  std::string server_address("0.0.0.0:2105");
  TestService service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  auto server = std::move(builder.BuildAndStart());
  server->Wait();

  std::cout << "Hello, World!" << std::endl;
  return 0;
}
