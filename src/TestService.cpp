#include "TestService.h"
static int i = 0;
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

::grpc::ServerWriteReactor<::Shape>* TestService::GetShapes(::grpc::CallbackServerContext* context,
                                                            const ::SceneID* request) {
  class ShapeWriter : public ::grpc::ServerWriteReactor<::Shape> {
   public:
    explicit ShapeWriter() { NextWrite(); }

    void OnDone() override { delete this; }

    void OnCancel() override {
      std::cout << "【" << std::this_thread::get_id() << "】" << "shape writer has been cancelled" << std::endl;
    }

    void OnWriteDone(bool ok) override {
      if (ok) {
        std::cout << "【" << std::this_thread::get_id() << "】" << "shape has been written" << std::endl;
        NextWrite();
      } else {
        std::cout << "【" << std::this_thread::get_id() << "】" << "shape write failed" << std::endl;
        Finish(::grpc::Status::CANCELLED);
      }
    }

   private:
    void NextWrite() {
      while (i++ < 5) {
        ::Shape* shape = new ::Shape();
        shape->set_type("circle");
        StartWrite(shape);
        return;
      }
      Finish(::grpc::Status::OK);
    }
  };
  return new ShapeWriter();
}
