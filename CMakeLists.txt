cmake_minimum_required(VERSION 3.21)
project(grpc_test_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/TestService.h
  src/TestService.cpp
  
  src/proto/gen/test.grpc.pb.cc
  src/proto/gen/test.pb.cc

)

file(GLOB_RECURSE grpc_libs "${GRPC_LIBS_DIR}/*.lib")

target_link_libraries(${PROJECT_NAME} PUBLIC
   ${grpc_libs}
)

target_include_directories(${PROJECT_NAME}
   PRIVATE
  ${GRPC_INCLUDE_DIRS}
  /src
)

install(TARGETS  ${PROJECT_NAME}
   RUNTIME DESTINATION ${install_DIR}
)