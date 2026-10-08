# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The fixture never uses a shell; each invocation has an explicit argument vector.
cmake_minimum_required(VERSION 3.21)
project(UmicomLiveTestOutputFixture NONE)
enable_testing()
if(NOT EXISTS "${HELPER}")
    message(FATAL_ERROR "The compiled output fixture helper is required.")
endif()
foreach(kind pass fail skip large unicode)
    add_test(NAME "output.${kind}" COMMAND "${HELPER}" "${kind}")
endforeach()
set_tests_properties(output.skip PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME output.slow COMMAND "${HELPER}" slow "${CMAKE_CURRENT_BINARY_DIR}/release")
