cmake_minimum_required(VERSION 3.10)

project(phone_book LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS contact.proto)

add_executable(${PROJECT_NAME}
	${PROTO_SRCS}
	${PROTO_HDRS}
	main.cpp
	phone_book.cpp
	phone_book.h
	test_runner.h
	iterator_range.h
)
target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${Protobuf_INCLUDE_DIRS}	# protobuf include files
	${CMAKE_CURRENT_BINARY_DIR}	# protobuf generated files
)
target_link_libraries(
	${PROJECT_NAME}
	${Protobuf_LIBRARIES}
)

# cmake -S . -B build
# cmake --build build
