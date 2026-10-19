# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Persistence is a separate bridge: users of pure run assessment need no database.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImpactInventory.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::distribution OR NOT TARGET Umicom::data)
    return()
endif()
include_guard(GLOBAL)
get_filename_component(_qualification_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_qualification_history STATIC
    "${_qualification_root}/src/distribution/runtime/qualification_history.c")
add_library(Umicom::qualification_history ALIAS umicom_qualification_history)
set_target_properties(umicom_qualification_history PROPERTIES
    EXPORT_NAME qualification_history C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_qualification_history PUBLIC Umicom::distribution Umicom::data)
target_include_directories(umicom_qualification_history PUBLIC
    "$<BUILD_INTERFACE:${_qualification_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_qualification_history)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_qualification_history)
endif()
install(TARGETS umicom_qualification_history EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_qualification_history)
install(FILES "${_qualification_root}/include/umicom/distribution/runtime/qualification_history.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/distribution/runtime" COMPONENT Framework)
umicom_release_inventory_own_headers(umicom_qualification_history
    include/umicom/distribution/runtime/qualification_history.h)
if(BUILD_TESTING)
    add_executable(umi-qualification-history-test
        "${_qualification_root}/tests/release_qualification/test_history.c"
        "${_qualification_root}/tests/release_qualification/fixture.c"
        "${_qualification_root}/tests/release_impact/fixture.c")
    target_link_libraries(umi-qualification-history-test PRIVATE Umicom::qualification_history)
    umicom_target_qualification_inputs(umi-qualification-history-test FILES
        "${_qualification_root}/tests/release_qualification/fixture.h"
        "${_qualification_root}/tests/release_impact/fixture.h")
    set_target_properties(umi-qualification-history-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umi-qualification-history-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umi-qualification-history-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-qualification-history-test)
    endif()
    foreach(_case IN ITEMS passed large transaction wrong-entry prepared-success stale repeat skipped abandoned late-begin interrupted-cancel interrupted-io)
        set(_name "framework.qualification.history.${_case}")
        add_test(NAME "${_name}" COMMAND umi-qualification-history-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;qualification;job-history;regression")
        umicom_test_impact_owners("${_name}" TARGETS umi-qualification-history-test)
    endforeach()
endif()

# Persisted identity and rollback behavior require the configured SQLite adapter.
# Do not register a fake passing replacement when that optional backend is absent.
if(BUILD_TESTING AND SQLite3_FOUND)
    add_executable(umi-qualification-sqlite-test
        "${_qualification_root}/tests/release_qualification/test_history_sqlite.c"
        "${_qualification_root}/tests/release_qualification/fixture.c"
        "${_qualification_root}/tests/release_impact/fixture.c")
    target_link_libraries(umi-qualification-sqlite-test PRIVATE Umicom::qualification_history)
    umicom_target_qualification_inputs(umi-qualification-sqlite-test FILES
        "${_qualification_root}/tests/release_qualification/fixture.h"
        "${_qualification_root}/tests/release_impact/fixture.h")
    umicom_target_qualification_inputs(umi-qualification-sqlite-test FILES
        "${_qualification_root}/tests/build_log/fixture.h" "${_qualification_root}/tests/support/symbolic_link.h")
    set_target_properties(umi-qualification-sqlite-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umi-qualification-sqlite-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umi-qualification-sqlite-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-qualification-sqlite-test)
    endif()
    foreach(_case IN ITEMS reopen unfinished commit-failure)
        set(_name "framework.qualification.history_sqlite.${_case}")
        set(_work "${CMAKE_CURRENT_BINARY_DIR}/qualification-history-${_case}")
        file(MAKE_DIRECTORY "${_work}")
        add_test(NAME "${_name}" COMMAND umi-qualification-sqlite-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 WORKING_DIRECTORY "${_work}"
            LABELS "framework;qualification;job-history;sqlite;regression")
        umicom_test_impact_owners("${_name}" TARGETS umi-qualification-sqlite-test)
    endforeach()
elseif(BUILD_TESTING)
    message(STATUS "Qualification SQLite coverage unavailable: configure with SQLite to qualify persistent history")
endif()
