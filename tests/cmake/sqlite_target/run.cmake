# Build-system regression harness, not an application implementation.
# Use the spelling supported by the running CMake while keeping developer
# diagnostics fatal. The older spelling is deprecated by newer toolchains;
# inspecting --help also keeps this fixture compatible with older releases.
execute_process(COMMAND "${CMAKE_COMMAND}" --help
    RESULT_VARIABLE help_result OUTPUT_VARIABLE cmake_help ERROR_VARIABLE help_error TIMEOUT 10)
if(NOT "${help_result}" STREQUAL "0")
    message(FATAL_ERROR "Cannot inspect CMake warning options: ${help_error}")
endif()
set(author_warning_option -Werror=dev)
if(cmake_help MATCHES "-Werror=author")
    set(author_warning_option -Werror=author)
endif()
# Preserve the fixed-spelling invocation for review. The replacement uses the
# same source, compiler, configuration checks and saved output streams.
if(FALSE)
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE}" -B "${OUTPUT}" -G "${GENERATOR}"
    "-DCASE=${CASE}" "-DRESOLVER=${RESOLVER}" "-DCMAKE_C_COMPILER=${COMPILER}" -Werror=dev
    RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr TIMEOUT 40)
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE}" -B "${OUTPUT}" -G "${GENERATOR}"
    "-DCASE=${CASE}" "-DRESOLVER=${RESOLVER}" "-DCMAKE_C_COMPILER=${COMPILER}" ${author_warning_option}
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
