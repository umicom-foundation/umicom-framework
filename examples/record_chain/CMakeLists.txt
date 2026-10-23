# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomRecordChainConsumer LANGUAGES C)
# This consumer has no source-tree include path or fallback implementation.
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS record_chain)
add_executable(record-chain-consumer main.c)
target_link_libraries(record-chain-consumer PRIVATE Umicom::record_chain)
set_target_properties(record-chain-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
enable_testing()
add_test(NAME record-chain-consumer COMMAND record-chain-consumer)
