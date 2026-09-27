# Build-system regression harness, not an application implementation.
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE}" -B "${OUTPUT}" -G "${GENERATOR}"
    "-DCASE=${CASE}" "-DRESOLVER=${RESOLVER}" "-DCMAKE_C_COMPILER=${COMPILER}" -Werror=dev
    RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 40)
file(WRITE "${OUTPUT}-configure.log" "${stdout}\n${stderr}")
if(CASE STREQUAL "missing")
    if(result EQUAL 0 OR NOT stderr MATCHES "neither supported imported target")
        message(FATAL_ERROR "Missing dependency did not fail with its expected diagnostic: ${stdout} ${stderr}")
    endif()
    return()
endif()
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Target-selection configuration failed: ${stdout} ${stderr}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${OUTPUT}" --config Debug
    RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 40)
file(WRITE "${OUTPUT}-build.log" "${stdout}\n${stderr}")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Target-selection consumer did not link: ${stdout} ${stderr}")
endif()
