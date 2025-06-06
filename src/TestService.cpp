#include "TestService.h"

::grpc::ServerUnaryReactor* TestService::TimeConsumingMethod(::grpc::CallbackServerContext* context,
                                                             const ::TimeConsumingRequest* request,
                                                             ::TimeConsumingResponse* response) {
  response->set_status(4);
  auto* reactor = context->DefaultReactor();
  reactor->Finish(grpc::Status::OK);
  return reactor;
}
