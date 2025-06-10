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

    void OnDone() override {
      std::cout << "【" << std::this_thread::get_id() << "】" << "====== done" << std::endl;
      delete this;
    }

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

static int count = 0;
::grpc::ServerBidiReactor<::MachineStatusRequest, ::MachineStatusResponse>*
TestService::MachineStatus(::grpc::CallbackServerContext* context) {
  class MachineStatusReactor : public ::grpc::ServerBidiReactor<::MachineStatusRequest, ::MachineStatusResponse> {
   public:
    MachineStatusReactor() {
      count = 0;
      std::cout << "【" << std::this_thread::get_id() << "】" << "machine status reactor has been created" << std::endl;
      StartRead(&request_);
    }
    void OnDone() override {
      std::cout << "【" << std::this_thread::get_id() << "】" << "machine status reactor has been done" << std::endl;
      delete this;
    }

    void OnCancel() override {
      std::cout << "【" << std::this_thread::get_id() << "】" << "machine status reactor has been cancelled"
                << std::endl;
    }

    void OnWriteDone(bool ok) override {
      if (ok) {
        std::cout << "【" << std::this_thread::get_id() << "】" << "machine status response has been written"
                  << std::endl;
        NextWrite();
      } else {
        std::cout << "【" << std::this_thread::get_id() << "】" << "machine status write failed" << std::endl;
        Finish(::grpc::Status::CANCELLED);
      }
    }

    void OnReadDone(bool ok) override {
      if (ok) {
        std::cout << "【" << std::this_thread::get_id() << "】" << "machine status request has been read" << std::endl;
        NextWrite();
      } else {
        std::cout << "【" << std::this_thread::get_id() << "】" << "machine status read failed" << std::endl;
        Finish(::grpc::Status::CANCELLED);
      }
    }

   private:
    void NextWrite() {
      response_.set_machine_id(request_.machine_id());
      response_.set_status("OK1");
      response_.add_logs("Log entry 1");
      response_.add_logs("Log entry 2");
      response_.add_logs("Log entry 3");

      if (count++ < 40) {
        if (count % 3 == 0) {  // 实际场景中，就是看是否有未处理的数据，如果有，则处理数据并写入返回。
          StartWrite(&response_);
        } else {
          StartRead(&request_);  // 如果数据都处理完了，就继续接收新的数据
        }
        return;
      }
      Finish(::grpc::Status::OK);
      return;
    }

   private:
    MachineStatusRequest request_;
    MachineStatusResponse response_;
  };
  return new MachineStatusReactor();
}
