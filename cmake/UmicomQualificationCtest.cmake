# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Saved CTest parsing is optional; the pure ledger does not depend on an executor.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImpactInventory.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::distribution OR NOT TARGET Umicom::testing)
    return()
endif()
include_guard(GLOBAL)
get_filename_component(_qualification_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_qualification_ctest STATIC
    "${_qualification_root}/src/distribution/runtime/qualification_ctest.c")
add_library(Umicom::qualification_ctest ALIAS umicom_qualification_ctest)
set_target_properties(umicom_qualification_ctest PROPERTIES
    EXPORT_NAME qualification_ctest C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_qualification_ctest PUBLIC Umicom::distribution PRIVATE Umicom::testing)
target_include_directories(umicom_qualification_ctest PUBLIC
    "$<BUILD_INTERFACE:${_qualification_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_qualification_ctest)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_qualification_ctest)
endif()
install(TARGETS umicom_qualification_ctest EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_qualification_ctest)
install(FILES "${_qualification_root}/include/umicom/distribution/runtime/qualification_ctest.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/distribution/runtime" COMPONENT Framework)
umicom_release_inventory_own_headers(umicom_qualification_ctest
    include/umicom/distribution/runtime/qualification_ctest.h)
if(BUILD_TESTING)
    add_executable(umi-qualification-ctest-test
        "${_qualification_root}/tests/release_qualification/test_ctest.c"
        "${_qualification_root}/tests/release_qualification/fixture.c"
        "${_qualification_root}/tests/release_impact/fixture.c")
    target_link_libraries(umi-qualification-ctest-test PRIVATE Umicom::qualification_ctest)
    umicom_target_qualification_inputs(umi-qualification-ctest-test FILES
        "${_qualification_root}/tests/release_qualification/fixture.h"
        "${_qualification_root}/tests/release_impact/fixture.h")
    set_target_properties(umi-qualification-ctest-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umi-qualification-ctest-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umi-qualification-ctest-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-qualification-ctest-test)
    endif()
    foreach(_case IN ITEMS passed unknown missing duplicate skipped disabled not-run failed timed-out
            process-failed existing empty malformed identity)
        set(_name "framework.qualification.ctest.${_case}")
        add_test(NAME "${_name}" COMMAND umi-qualification-ctest-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;qualification;ctest;regression")
        umicom_test_impact_owners("${_name}" TARGETS umi-qualification-ctest-test)
    endforeach()
endif()
