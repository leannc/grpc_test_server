#include "TestService.h"

::grpc::ServerUnaryReactor* TestService::TimeConsumingMethod(::grpc::CallbackServerContext* context,
                                                             const ::TimeConsumingRequest* request,
                                                             ::TimeConsumingResponse* response) {
  response->set_status(4);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto* reactor = context->DefaultReactor();
  std::cout << "【" << std::this_thread::get_id() << "】" << "time consuming method has been called" << std::endl;
  reactor->Finish(grpc::Status::OK);
  return reactor;
}
