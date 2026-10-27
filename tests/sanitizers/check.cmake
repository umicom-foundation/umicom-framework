# Umicom Framework
# File: tests/sanitizers/check.cmake
# Purpose: Keep configure/link evidence and reject unexpected sanitizer outcomes.
# Author and organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
foreach(required SOURCE BINARY MODE COMPILER GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing sanitizer check input ${required}")
    endif()
endforeach()
file(MAKE_DIRECTORY "${BINARY}")
set(arguments -S "${SOURCE}" -B "${BINARY}" -G "${GENERATOR}"
    "-DCMAKE_C_COMPILER=${COMPILER}" "-DMODE=${MODE}" -DCMAKE_BUILD_TYPE=Debug)
if(MAKE_PROGRAM)
    list(APPEND arguments "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}")
endif()
if(GENERATOR_PLATFORM)
    list(APPEND arguments -A "${GENERATOR_PLATFORM}")
endif()
if(GENERATOR_TOOLSET)
    list(APPEND arguments -T "${GENERATOR_TOOLSET}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" ${arguments}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 90)
file(WRITE "${BINARY}/configure.log" "${output}\n${error}")
if(MODE STREQUAL "unsupported")
    if(result EQUAL 0 OR NOT "${output}\n${error}" MATCHES "unsupported by this")
        message(FATAL_ERROR "Expected explicit unsupported-compiler refusal; see ${BINARY}/configure.log")
    endif()
    return()
endif()
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Sanitizer consumer configure failed (${result}); see ${BINARY}/configure.log")
endif()
if(NOT CONFIG)
    set(CONFIG Debug)
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${BINARY}" --config "${CONFIG}" --parallel 2
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 120)
file(WRITE "${BINARY}/build.log" "${output}\n${error}")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Sanitizer consumer link failed (${result}); see ${BINARY}/build.log")
endif()
