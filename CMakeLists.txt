cmake_minimum_required(VERSION 3.20.0)

project(vlogger VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(VLOGGER_ENABLE_TESTS "Enable testing for vlogger" OFF)

# 自动查找并收集源文件列表 注意：在添加或删除文件后请重新运行 CMake 配置
file(GLOB VLOGGER_CODE_SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${VLOGGER_CODE_SOURCES})

# 查找线程库
find_package(Threads REQUIRED)

# 链接线程库
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# 指定头文件的位置
# $<BUILD_INTERFACE:...> 指定构建后的包含路径
# $<INSTALL_INTERFACE:...> 指定安装后的包含路径
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 指定库文件（动态库、静态库、或可执行文件）的安装位置
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# 指定要安装的头文件及其目标位置
install(DIRECTORY include/ DESTINATION include)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "V")
include(CPack)

# 基于 VLOGGER_ENABLE_TESTS 来启用测试
if(VLOGGER_ENABLE_TESTS)
    include(CTest)
    enable_testing()
    add_executable(test_vlogger test/test_vlogger.cpp)
    target_link_libraries(test_vlogger PRIVATE ${PROJECT_NAME})
    add_test(NAME VLoggerTest COMMAND test_vlogger)
endif()