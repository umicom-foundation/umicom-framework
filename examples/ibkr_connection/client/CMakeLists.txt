# Umicom Foundation | Sammy Hegab | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomBrokerSdkClient LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ibkr_connection)
add_executable(umicom-broker-sdk-client main.c)
set_target_properties(umicom-broker-sdk-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-broker-sdk-client PRIVATE Umicom::ibkr_connection)
enable_testing()
add_test(NAME broker.sdk.read_only_profiles COMMAND umicom-broker-sdk-client)
