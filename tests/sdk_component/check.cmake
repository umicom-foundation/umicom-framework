# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Each run owns a fresh directory. Keep stage logs for diagnosis and never
# remove an earlier qualification result or a caller's installation.
cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS TEST_CASE SOURCE_ROOT OUTPUT_ROOT SDK_HELPER GENERATOR CC CTEST_BIN)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "SDK component fixture requires ${_required}")
    endif()
endforeach()
if(NOT IS_ABSOLUTE "${OUTPUT_ROOT}" OR NOT IS_ABSOLUTE "${SOURCE_ROOT}")
    message(FATAL_ERROR "SDK component fixture requires absolute output and source roots")
endif()
set(_cases selective spaced-prefix relocated-prefix shared release
    duplicate-registration deferred-registration full-install missing-library missing-installed-library
    missing-target imported-target alias-target executable-target object-target empty-registration name-collision)
list(APPEND _cases development-command duplicate-tool relocated-tool missing-installed-tool tool-missing tool-library tool-imported tool-alias tool-empty tool-bundle)
if(NOT TEST_CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown SDK component fixture: ${TEST_CASE}")
endif()
if(NOT DEFINED CONFIG OR CONFIG STREQUAL "")
    set(CONFIG Debug)
endif()
if(TEST_CASE STREQUAL "release")
    set(CONFIG Release)
endif()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef _run_id)
set(_run "${OUTPUT_ROOT}/run-${_run_id}")
if(EXISTS "${_run}")
    message(FATAL_ERROR "SDK component fixture output already exists: ${_run}")
endif()
file(MAKE_DIRECTORY "${_run}")
set(_prefix "${_run}/sdk")
if(TEST_CASE STREQUAL "spaced-prefix")
    set(_prefix "${_run}/installed SDK")
endif()
set(_producer "${_run}/producer")
set(_consumer "${_run}/consumer")

# Execute one bounded stage at a time. Failure leaves its complete output in a
# known log rather than continuing to build against an incomplete installation.
function(sdk_stage name)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE _result
        OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    file(WRITE "${_run}/${name}.log" "${_out}\n${_err}")
    if(NOT "${_result}" STREQUAL "0")
        message(FATAL_ERROR "SDK component ${name} failed (${_result}); see ${_run}/${name}.log\n${_out}\n${_err}")
    endif()
endfunction()
set(_configure "${CMAKE_COMMAND}" -S "${SOURCE_ROOT}/fixture" -B "${_producer}"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${CC}" "-DCMAKE_BUILD_TYPE=${CONFIG}"
    "-DTEST_CASE=${TEST_CASE}" "-DSDK_HELPER=${SDK_HELPER}"
    "-DCMAKE_INSTALL_LIBDIR=lib")
set(_consumer_configure "${CMAKE_COMMAND}" -S "${SOURCE_ROOT}/consumer" -B "${_consumer}"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${CC}" "-DCMAKE_BUILD_TYPE=${CONFIG}")
# Preserve native generator details without borrowing the parent build cache.
foreach(_setting IN ITEMS MAKE_PROGRAM GENERATOR_PLATFORM GENERATOR_TOOLSET)
    if(DEFINED ${_setting} AND NOT "${${_setting}}" STREQUAL "")
        if(_setting STREQUAL "MAKE_PROGRAM")
            set(_cache_name CMAKE_MAKE_PROGRAM)
        else()
            set(_cache_name "CMAKE_${_setting}")
        endif()
        list(APPEND _configure "-D${_cache_name}=${${_setting}}")
        list(APPEND _consumer_configure "-D${_cache_name}=${${_setting}}")
    endif()
endforeach()

if(DEFINED EXPECTED_ERROR AND NOT EXPECTED_ERROR STREQUAL "")
    execute_process(COMMAND ${_configure} RESULT_VARIABLE _result
        OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    file(WRITE "${_run}/configure.log" "${_out}\n${_err}")
    if("${_result}" STREQUAL "0" OR NOT "${_out}\n${_err}" MATCHES "${EXPECTED_ERROR}")
        message(FATAL_ERROR "Expected SDK registration refusal was not observed; see ${_run}/configure.log")
    endif()
    message(STATUS "Verified expected SDK refusal: ${TEST_CASE}; logs: ${_run}")
    return()
endif()

sdk_stage(configure ${_configure})
if(TEST_CASE STREQUAL "missing-library")
    # No library has been built. The component must fail, not silently install
    # an export file referencing a missing archive via OPTIONAL artifact rules.
    execute_process(COMMAND "${CMAKE_COMMAND}" --install "${_producer}"
        --config "${CONFIG}" --prefix "${_prefix}" --component FrameworkSDK
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    file(WRITE "${_run}/install.log" "${_out}\n${_err}")
    if("${_result}" STREQUAL "0" OR NOT "${_out}\n${_err}" MATCHES "sdk_base")
        message(FATAL_ERROR "Missing SDK archive was not reported; see ${_run}/install.log")
    endif()
    return()
endif()

sdk_stage(build "${CMAKE_COMMAND}" --build "${_producer}" --config "${CONFIG}"
    --target umicom-sdk-libraries --parallel 2)
file(READ "${_producer}/tool-path-${CONFIG}.txt" _tool)
if(EXISTS "${_tool}")
    message(FATAL_ERROR "SDK library target unexpectedly built the product executable")
endif()
sdk_stage(install "${CMAKE_COMMAND}" --install "${_producer}"
    --config "${CONFIG}" --prefix "${_prefix}" --component FrameworkSDK)
# Inspect generated data as well as the imported CMake targets. A deferred
# library must appear, while an unrelated product command must stay absent.
file(READ "${_prefix}/share/umicom/framework/sdk-artifacts.txt" _inventory)
foreach(_target sdk_base sdk_value sdk_interface)
    string(FIND "${_inventory}" "${_target}|" _found)
    if(_found LESS 0)
        message(FATAL_ERROR "SDK artifact inventory omitted ${_target}")
    endif()
endforeach()
string(FIND "${_inventory}" "sdk_tool|" _product_record)
if(NOT _product_record LESS 0)
    message(FATAL_ERROR "Unregistered product command entered the SDK receipt")
endif()
if(TEST_CASE STREQUAL "deferred-registration")
    string(FIND "${_inventory}" "sdk_deferred|" _deferred_record)
    if(_deferred_record LESS 0)
        message(FATAL_ERROR "SDK receipt missed a deferred target registration")
    endif()
endif()
if(TEST_CASE MATCHES "^(development-command|duplicate-tool|relocated-tool|missing-installed-tool)$")
    string(FIND "${_inventory}" "sdk_development_command|tool|bin/" _command_record)
    if(_command_record LESS 0)
        message(FATAL_ERROR "SDK receipt missed its required exported command")
    endif()
endif()
if(EXISTS "${_prefix}/bin/sdk_tool" OR EXISTS "${_prefix}/bin/sdk_tool.exe")
    message(FATAL_ERROR "SDK component unexpectedly installed the product executable")
endif()
if(TEST_CASE STREQUAL "full-install")
    sdk_stage(full-build "${CMAKE_COMMAND}" --build "${_producer}"
        --config "${CONFIG}" --parallel 2)
    sdk_stage(full-install "${CMAKE_COMMAND}" --install "${_producer}"
        --config "${CONFIG}" --prefix "${_prefix}")
    if(NOT EXISTS "${_prefix}/bin/sdk_tool" AND NOT EXISTS "${_prefix}/bin/sdk_tool.exe")
        message(FATAL_ERROR "Full install no longer includes the product executable")
    endif()
elseif(TEST_CASE STREQUAL "relocated-prefix" OR TEST_CASE STREQUAL "relocated-tool")
    # Move only the fixture-owned install. The consumer must find libraries at
    # the new location after the original prefix has ceased to exist.
    file(RENAME "${_prefix}" "${_run}/relocated SDK")
    set(_prefix "${_run}/relocated SDK")
elseif(TEST_CASE STREQUAL "missing-installed-library")
    # Preserve the installed artifact under a different name; the package must
    # then refuse a consumer instead of quietly falling back to a source tree.
    file(GLOB _archives "${_prefix}/lib/*sdk_base*")
    list(LENGTH _archives _count)
    if(NOT _count EQUAL 1)
        message(FATAL_ERROR "Expected exactly one installed fixture base archive")
    endif()
    list(GET _archives 0 _archive)
    file(RENAME "${_archive}" "${_archive}.withheld")
    execute_process(COMMAND ${_consumer_configure} "-DSDK_ROOT=${_prefix}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    file(WRITE "${_run}/consumer-configure.log" "${_out}\n${_err}")
    if("${_result}" STREQUAL "0" OR NOT "${_out}\n${_err}" MATCHES "sdk_base")
        message(FATAL_ERROR "Consumer accepted an incomplete SDK; see ${_run}/consumer-configure.log")
    endif()
    return()
endif()

# Check the command artifact independently of the archive regression. Keep the
# withheld fixture file for diagnosis; do not alter any caller's installation.
if(TEST_CASE STREQUAL "missing-installed-tool")
    file(GLOB _commands "${_prefix}/bin/sdk_development_command*")
    list(LENGTH _commands _count)
    if(NOT _count EQUAL 1)
        message(FATAL_ERROR "Expected one installed developer command")
    endif()
    list(GET _commands 0 _command)
    file(RENAME "${_command}" "${_command}.withheld")
    execute_process(COMMAND ${_consumer_configure} "-DSDK_ROOT=${_prefix}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _out ERROR_VARIABLE _err TIMEOUT 120)
    file(WRITE "${_run}/consumer-configure.log" "${_out}\n${_err}")
    if("${_result}" STREQUAL "0" OR NOT "${_out}\n${_err}" MATCHES "sdk_development_command")
        message(FATAL_ERROR "Consumer accepted an SDK with a missing command; see ${_run}/consumer-configure.log")
    endif()
    return()
endif()
sdk_stage(consumer-configure ${_consumer_configure} "-DSDK_ROOT=${_prefix}")
sdk_stage(consumer-build "${CMAKE_COMMAND}" --build "${_consumer}"
    --config "${CONFIG}" --parallel 2)
sdk_stage(consumer-test "${CTEST_BIN}" --test-dir "${_consumer}" -C "${CONFIG}"
    --output-on-failure --no-tests=error)
message(STATUS "SDK component scenario passed: ${TEST_CASE}; logs: ${_run}")
