# Umicom Foundation | MIT | Independent public-header consumer.
cmake_minimum_required(VERSION 3.25)
project(StudyClient LANGUAGES C)
include(CTest)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(study-client main.c)
set_target_properties(study-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(study-client PRIVATE Umicom::education_workspace)
add_test(NAME installed.study.client COMMAND study-client)
