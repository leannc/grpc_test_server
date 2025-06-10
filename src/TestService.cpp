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

::grpc::ServerWriteReactor<::Shape>* TestService::GetShapes(::grpc::CallbackServerContext* context,
                                                            const ::SceneID* request) {
  class ShapeWriter : public ::grpc::ServerWriteReactor<::Shape> {
   public:
    explicit ShapeWriter() {
      for (int i = 0; i < 10; i++) {
        ::Shape shape;
        shape.set_type("circle");
        shape.add_dimensions(10);
        shape.set_color("red");
        shapes_.push_back(shape);
      }
      it_ = shapes_.begin();
      NextWrite();
    }

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
      while (it_ != shapes_.end()) {
        ::Shape& shape = *it_++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        StartWrite(&shape);
        return;
      }
      Finish(::grpc::Status::OK);
    }

   private:
    std::vector<::Shape> shapes_;
    std::vector<::Shape>::iterator it_;
  };
  return new ShapeWriter();
}
