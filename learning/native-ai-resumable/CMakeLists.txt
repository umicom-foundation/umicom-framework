# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Standalone, opt-in native C23 dataset and adapter-SGD checkpoint laboratory.
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeAiContinuation LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiSupervisedContinuation.cmake")
if(NOT TARGET umicom-native-continuation-demo)
    add_executable(umicom-native-continuation-demo
        "${CMAKE_CURRENT_LIST_DIR}/continuation_demo.c")
    target_link_libraries(umicom-native-continuation-demo PRIVATE
        Umicom::AiSupervisedContinuation)
    target_compile_features(umicom-native-continuation-demo PRIVATE c_std_23)
    if(BUILD_TESTING)
        add_test(NAME framework.ai.continuation.demo
            COMMAND umicom-native-continuation-demo)
        set_tests_properties(framework.ai.continuation.demo PROPERTIES
            LABELS "framework;ai;native-training;continuation" TIMEOUT 120)
    endif()
endif()
