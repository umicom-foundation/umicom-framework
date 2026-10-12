# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# CTest builds this isolated SDK consumer beneath its own build root.
foreach(_required FRAMEWORK_ROOT TEST_ROOT TEST_GENERATOR TEST_COMPILER)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing package-check input: ${_required}")
    endif()
endforeach()
get_filename_component(_root "${TEST_ROOT}" ABSOLUTE)
file(MAKE_DIRECTORY "${_root}")
set(_prefix "${_root}/installed SDK")
set(_relocated "${_root}/copied SDK")
set(_producer "${_root}/producer")
set(_consumer "${_root}/consumer")

# Each step must succeed before the next begins. Keep warnings from successful
# commands in a build-tree log too; CTest normally hides successful output.
# These commands run only when CTest selects this check.
set(_diagnostic_log "${_root}/inspection-sdk.log")
file(WRITE "${_diagnostic_log}" "Education inspection SDK qualification\n")
function(inspection_step description)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result
        OUTPUT_VARIABLE _out ERROR_VARIABLE _error TIMEOUT 90)
    file(APPEND "${_diagnostic_log}"
        "\n${description}\nExit: ${_result}\n${_out}\n${_error}\n")
    if(NOT _result STREQUAL "0")
        message(FATAL_ERROR "${description} failed (${_result})\n${_out}\n${_error}")
    endif()
endfunction()
set(_generator_args -G "${TEST_GENERATOR}")
if(TEST_PLATFORM)
    list(APPEND _generator_args -A "${TEST_PLATFORM}")
endif()
if(TEST_TOOLSET)
    list(APPEND _generator_args -T "${TEST_TOOLSET}")
endif()
set(_compiler_args "-DCMAKE_C_COMPILER=${TEST_COMPILER}")
if(TEST_MAKE_PROGRAM)
    list(APPEND _compiler_args "-DCMAKE_MAKE_PROGRAM=${TEST_MAKE_PROGRAM}")
endif()
if(TEST_TOOLCHAIN)
    list(APPEND _compiler_args "-DCMAKE_TOOLCHAIN_FILE=${TEST_TOOLCHAIN}")
endif()
set(_config_args "")
if(TEST_CONFIG)
    list(APPEND _config_args --config "${TEST_CONFIG}")
    list(APPEND _compiler_args "-DCMAKE_BUILD_TYPE=${TEST_CONFIG}")
endif()

inspection_step("Configure inspection package" "${CMAKE_COMMAND}"
    -S "${FRAMEWORK_ROOT}/tests/education/inspection_package"
    -B "${_producer}" ${_generator_args} ${_compiler_args}
    "-DFRAMEWORK_ROOT=${FRAMEWORK_ROOT}" "-DCMAKE_INSTALL_PREFIX=${_prefix}"
    -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_INCLUDEDIR=include -DCMAKE_INSTALL_DATADIR=share)
inspection_step("Build inspection package" "${CMAKE_COMMAND}"
    --build "${_producer}" ${_config_args} --parallel 2)
inspection_step("Install inspection package" "${CMAKE_COMMAND}"
    --install "${_producer}" ${_config_args})
# Copy rather than delete or move an existing SDK. The consumer checks that
# every imported include/library path uses this copy, not the producer tree.
file(COPY "${_prefix}/" DESTINATION "${_relocated}")
inspection_step("Configure installed consumer" "${CMAKE_COMMAND}"
    -S "${FRAMEWORK_ROOT}/tests/education/inspection_consumer"
    -B "${_consumer}" ${_generator_args} ${_compiler_args}
    "-DINSPECTION_PREFIX=${_relocated}" "-DCMAKE_PREFIX_PATH=${_relocated}"
    "-DUmicomFramework_DIR=${_relocated}/lib/cmake/UmicomFramework"
    -DBUILD_TESTING=ON -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
    -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)
inspection_step("Build installed consumer" "${CMAKE_COMMAND}"
    --build "${_consumer}" ${_config_args} --parallel 2)
get_filename_component(_cmake_bin "${CMAKE_COMMAND}" DIRECTORY)
find_program(_ctest NAMES ctest ctest.exe PATHS "${_cmake_bin}" NO_DEFAULT_PATH REQUIRED)
set(_ctest_config "")
if(TEST_CONFIG)
    list(APPEND _ctest_config -C "${TEST_CONFIG}")
endif()
inspection_step("Run installed consumer" "${_ctest}"
    --test-dir "${_consumer}" ${_ctest_config} --no-tests=error --output-on-failure)
message(STATUS "Installed inspection consumer and example succeeded from the copied prefix")
message(STATUS "Qualification diagnostics: ${_diagnostic_log}")
