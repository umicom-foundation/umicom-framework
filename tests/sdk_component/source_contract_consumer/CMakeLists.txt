# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledSourceContractConsumer LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
enable_testing()
# The standalone producer exports the same real targets as the full Framework.
# No source fallback can conceal a missing installed archive or command.
include("${SDK_ROOT}/lib/cmake/UmicomSourceContracts/UmicomFrameworkTargets.cmake")
add_executable(source_contract_consumer main.c)
target_link_libraries(source_contract_consumer PRIVATE Umicom::source_contracts)
add_test(NAME sdk.source_contracts.library COMMAND source_contract_consumer)
add_test(NAME sdk.source_contracts.command COMMAND Umicom::umicom-source-contracts --help)
set_tests_properties(sdk.source_contracts.command PROPERTIES
    PASS_REGULAR_EXPRESSION "Umicom native source contracts")
