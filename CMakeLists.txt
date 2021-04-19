cmake_minimum_required(VERSION 3.19)

project(geogenetic-visualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib -ltbb")
set(CMAKE_CXX_FLAGS "-std=c++17 -Ofast -Wall -Werror -Wshadow -Wpedantic")

find_package(Boost COMPONENTS program_options iostreams filesystem REQUIRED)
find_package(Protobuf REQUIRED)
find_package(TBB REQUIRED)

message(STATUS "Using Boost ${Boost_VERSION}")
message(STATUS "Using Protocol Buffers ${Protobuf_VERSION}")
message(STATUS "Using TBB ${TBB_VERSION}")

protobuf_generate_cpp(
  PROTO_SRC PROTO_HDR
  src/parsimony.proto
)

file(GLOB SRC "src/*.cpp")
file(GLOB HDR "include/*.{h,hpp}")

add_executable(geoviz
  ${SRC}
  ${PROTO_SRC}
  ${HDR}
  ${PROTO_HDR}
)

target_include_directories(geoviz
  PRIVATE
  src
  include
  ${Boost_INCLUDE_DIRS}
  ${Protobuf_INCLUDE_DIRS}
  ${TBB_INCLUDE_DIRS}
)

target_link_libraries(geoviz
  ${Boost_LIBRARIES}
  ${Protobuf_LIBRARIES}
  ${TBB_LIBRARIES}
)
