# Umicom Framework — installed SDK financial inbox example
# File: examples/financial_messaging/CMakeLists.txt
# PURPOSE: Show a complete native consumer of the financial inbox SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomFinancialInboxExample LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(financial-inbox-example inbox.c)
set_target_properties(financial-inbox-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(financial-inbox-example PRIVATE Umicom::financial_inbox)
