# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledComponents LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS market_tape research_replay ibkr_connection)
add_executable(umicom-components-client ../main.c)
set_target_properties(umicom-components-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-components-client PRIVATE
    Umicom::market_tape Umicom::research_replay Umicom::ibkr_connection)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME installed.component_integration.lesson COMMAND umicom-components-client)
endif()
