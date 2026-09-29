# Umicom Foundation | MIT: independently consume only installed public contracts.
cmake_minimum_required(VERSION 3.24)
project(UmicomThreadClient LANGUAGES C)
find_package(UmicomThreadSafety CONFIG REQUIRED)
add_executable(umicom-thread-client main.c)
set_target_properties(umicom-thread-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-thread-client PRIVATE Umicom::platform)
include(CTest)
add_test(NAME thread-client COMMAND umicom-thread-client)
