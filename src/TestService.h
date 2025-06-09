#pragma once
#include "proto/gen/test.grpc.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/alarm.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

class TestService : public Test::CallbackService {
  ::grpc::ServerUnaryReactor* TimeConsumingMethod(::grpc::CallbackServerContext* context,
                                                  const ::TimeConsumingRequest* request,
                                                  ::TimeConsumingResponse* response) override;
  ::grpc::ServerWriteReactor<::Shape>* GetShapes(::grpc::CallbackServerContext* context,
                                                 const ::SceneID* request) override;
};
