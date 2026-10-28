#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAutomaticTests.cmake
#
# PURPOSE:
#   Connect native per-test receipts to configured self-contained test contracts.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET Umicom::testing)
    return()
endif()
target_sources(umicom_testing PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/testing/reuse.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/reuse.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/testing" COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/automatic-builds.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides" COMPONENT Framework)
# Cross builds still expose the portable receipt service. Host test execution
# and its driver are only needed in native configurations with tests enabled.
if(CMAKE_CROSSCOMPILING OR NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-test-receipts "${CMAKE_CURRENT_LIST_DIR}/../tools/qualification/reuse_main.c")
target_link_libraries(umicom-test-receipts PRIVATE Umicom::testing)
set_target_properties(umicom-test-receipts PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-test-receipts)
umicom_apply_sanitizers(umicom-test-receipts)

# A contract is a developer assertion about the test, never a user-maintained
# list of changes. Do not register network, clock, hardware, mutable fixture or
# environment-observing tests. Unknown tests continue to execute on every run.
function(umicom_test_closed_inputs test target)
    if(NOT TEST "${test}" OR NOT TARGET "${target}")
        message(FATAL_ERROR "Automatic test contract requires an existing test and executable")
    endif()
    if(NOT "${test}" MATCHES "^[A-Za-z0-9_.:+/-]+$")
        message(FATAL_ERROR "Automatic test names must use portable printable identifiers")
    endif()
    get_target_property(_kind "${target}" TYPE)
    if(NOT _kind STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "Automatic test contract must identify its executable")
    endif()
    set_property(GLOBAL APPEND PROPERTY UMICOM_AUTOMATIC_TEST_CONTRACTS
        "${CMAKE_CURRENT_SOURCE_DIR}|${test}|${target}")
endfunction()

function(_umicom_automatic_collect directory)
    get_property(_tests DIRECTORY "${directory}" PROPERTY TESTS)
    foreach(_test IN LISTS _tests)
        get_property(_depends TEST "${_test}" DIRECTORY "${directory}" PROPERTY DEPENDS)
        if(_depends)
            set_property(GLOBAL APPEND PROPERTY UMICOM_AUTOMATIC_RELATED_TESTS "${_test}" ${_depends})
        endif()
    endforeach()
    get_property(_children DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
    foreach(_child IN LISTS _children)
        _umicom_automatic_collect("${_child}")
    endforeach()
endfunction()

function(_umicom_automatic_finalize)
    # Allow late test registration to finish. The inventory finalizer waits for
    # this callback, so exclude that observer to avoid a mutual wait.
    cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" GET_CALL_IDS _pending)
    list(REMOVE_ITEM _pending umicom_automatic_final umicom_inventory_final)
    if(_pending)
        get_property(_attempt GLOBAL PROPERTY UMICOM_AUTOMATIC_FINAL_ATTEMPTS)
        if(NOT _attempt)
            set(_attempt 0)
        endif()
        math(EXPR _attempt "${_attempt} + 1")
        if(_attempt GREATER 64)
            message(FATAL_ERROR "Automatic test registration did not settle")
        endif()
        set_property(GLOBAL PROPERTY UMICOM_AUTOMATIC_FINAL_ATTEMPTS "${_attempt}")
        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" ID umicom_automatic_final CALL _umicom_automatic_finalize)
        return()
    endif()
    if(CMAKE_VERSION VERSION_LESS "3.29")
        message(STATUS "Automatic test reuse requires CMake/CTest 3.29; ordinary tests remain available")
        return()
    endif()
    _umicom_automatic_collect("${CMAKE_SOURCE_DIR}")
    get_property(_related GLOBAL PROPERTY UMICOM_AUTOMATIC_RELATED_TESTS)
    get_property(_contracts GLOBAL PROPERTY UMICOM_AUTOMATIC_TEST_CONTRACTS)
    set(_registry "")
    foreach(_contract IN LISTS _contracts)
        string(REPLACE "|" ";" _parts "${_contract}")
        list(GET _parts 0 _directory)
        list(GET _parts 1 _test)
        list(GET _parts 2 _target)
        set(_closed TRUE)
        if(_test IN_LIST _related)
            set(_closed FALSE)
        endif()
        foreach(_property FIXTURES_SETUP FIXTURES_REQUIRED FIXTURES_CLEANUP
                ENVIRONMENT ENVIRONMENT_MODIFICATION RESOURCE_GROUPS REQUIRED_FILES)
            get_property(_value TEST "${_test}" DIRECTORY "${_directory}" PROPERTY "${_property}")
            if(_value)
                set(_closed FALSE)
            endif()
        endforeach()
        if(_closed)
            string(APPEND _registry "${_test}\t$<TARGET_FILE:${_target}>\n")
        endif()
    endforeach()
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/umicom")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/umicom/closed-tests-$<CONFIG>.tsv" CONTENT "${_registry}")
    # The configured driver belongs to this build tree. Installed Framework
    # source paths are not guessed by Studio, and no SDK is rebuilt for another
    # project's build directory.
    set(UMICOM_AUTOMATIC_BUILD_ROOT "${CMAKE_BINARY_DIR}")
    set(UMICOM_AUTOMATIC_SOURCE_ROOT "${CMAKE_SOURCE_DIR}")
    set(UMICOM_AUTOMATIC_RECEIPTS "$<TARGET_FILE:umicom-test-receipts>")
    set(UMICOM_AUTOMATIC_SCRIPT "${_UMICOM_AUTOMATIC_MODULE_DIR}/UmicomAutomaticTestsRun.cmake")
    set(UMICOM_AUTOMATIC_COMPILER_DIR "")
    get_filename_component(UMICOM_AUTOMATIC_COMPILER_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    set(UMICOM_AUTOMATIC_OBJDUMP "${CMAKE_OBJDUMP}")
    if(WIN32)
        set(UMICOM_AUTOMATIC_RUNTIME_PLATFORM "windows+pe")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(UMICOM_AUTOMATIC_RUNTIME_PLATFORM "linux+elf")
    else()
        set(UMICOM_AUTOMATIC_RUNTIME_PLATFORM "")
    endif()
    set(_driver "set(UMICOM_AUTOMATIC_BUILD_ROOT [==[${UMICOM_AUTOMATIC_BUILD_ROOT}]==])\n")
    foreach(_key SOURCE_ROOT RECEIPTS SCRIPT COMPILER_DIR OBJDUMP RUNTIME_PLATFORM)
        string(APPEND _driver "set(UMICOM_AUTOMATIC_${_key} [==[${UMICOM_AUTOMATIC_${_key}}]==])\n")
    endforeach()
    string(APPEND _driver "set(UMICOM_AUTOMATIC_CTEST [==[${CMAKE_CTEST_COMMAND}]==])\n")
    string(APPEND _driver "include([==[${UMICOM_AUTOMATIC_SCRIPT}]==])\n")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/umicom/automatic-tests-$<CONFIG>.cmake" CONTENT "${_driver}")
endfunction()
set(_UMICOM_AUTOMATIC_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Framework automatic test driver directory")
cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" ID umicom_automatic_final CALL _umicom_automatic_finalize)

if(BUILD_TESTING)
    foreach(_case receipts session provider)
        add_executable("umicom-test-reuse-${_case}" "${CMAKE_CURRENT_LIST_DIR}/../tests/test_reuse/test_${_case}.c")
        target_link_libraries("umicom-test-reuse-${_case}" PRIVATE Umicom::testing Umicom::build)
        set_target_properties("umicom-test-reuse-${_case}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings("umicom-test-reuse-${_case}")
        umicom_apply_sanitizers("umicom-test-reuse-${_case}")
        if(_case STREQUAL "provider" OR _case STREQUAL "session")
            set(_fixture "${CMAKE_CURRENT_BINARY_DIR}/qualification/test-reuse-provider")
            file(MAKE_DIRECTORY "${_fixture}/umicom")
            file(WRITE "${_fixture}/umicom/automatic-tests-Debug.cmake" "# Inert command-construction fixture; never execute.\n")
            add_test(NAME "framework.testing.reuse.${_case}" COMMAND "umicom-test-reuse-${_case}" "${_fixture}")
        else()
            add_test(NAME "framework.testing.reuse.${_case}" COMMAND "umicom-test-reuse-${_case}")
        endif()
        set_tests_properties("framework.testing.reuse.${_case}" PROPERTIES TIMEOUT 60 LABELS "framework;build;testing;regression")
    endforeach()
endif()

# A small independent project checks the real configure/build/discovery/receipt
# sequence. It does not rebuild the Framework from inside its own test suite.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.29" AND EXISTS "${CMAKE_OBJDUMP}"
        AND (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    if(WIN32)
        set(_reuse_platform "windows+pe")
    else()
        set(_reuse_platform "linux+elf")
    endif()
    add_test(NAME framework.testing.reuse.driver
        COMMAND "${CMAKE_COMMAND}"
            "-DFIXTURE_ROOT=${CMAKE_CURRENT_LIST_DIR}/../tests/test_reuse/driver"
            "-DRUN_ROOT=${CMAKE_CURRENT_BINARY_DIR}/qualification/test-reuse-driver"
            "-DRECEIPTS=$<TARGET_FILE:umicom-test-receipts>"
            "-DRUN_SCRIPT=${CMAKE_CURRENT_LIST_DIR}/UmicomAutomaticTestsRun.cmake"
            "-DC_COMPILER=${CMAKE_C_COMPILER}"
            "-DOBJDUMP=${CMAKE_OBJDUMP}"
            "-DRUNTIME_PLATFORM=${_reuse_platform}"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/test_reuse/check_driver.cmake")
    set_tests_properties(framework.testing.reuse.driver PROPERTIES
        TIMEOUT 240 LABELS "framework;build;testing;regression;integration")
endif()
