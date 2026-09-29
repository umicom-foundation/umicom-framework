# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomReleaseEvidenceClient LANGUAGES C)
enable_testing()
find_package(UmicomReleaseBaseline CONFIG REQUIRED)
add_executable(umicom-release-client main.c)
target_link_libraries(umicom-release-client PRIVATE Umicom::distribution)
set_target_properties(umicom-release-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_test(NAME client.public_contract COMMAND umicom-release-client)
