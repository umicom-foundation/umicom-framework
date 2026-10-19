# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Host execution remains injectable while evidence sequencing is shared.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomQualificationCtest.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImpactInventory.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::distribution OR NOT TARGET Umicom::qualification_ctest OR NOT TARGET Umicom::build)
    return()
endif()
include_guard(GLOBAL)
get_filename_component(_qualification_workflow_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_qualification_workflow STATIC "${_qualification_workflow_root}/src/distribution/runtime/qualification_workflow.c")
add_library(Umicom::qualification_workflow ALIAS umicom_qualification_workflow)
set_target_properties(umicom_qualification_workflow PROPERTIES
    EXPORT_NAME qualification_workflow C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_qualification_workflow PUBLIC Umicom::distribution
    PRIVATE Umicom::qualification_ctest Umicom::build)
target_include_directories(umicom_qualification_workflow PUBLIC
    "$<BUILD_INTERFACE:${_qualification_workflow_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_qualification_workflow)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_qualification_workflow)
endif()
install(TARGETS umicom_qualification_workflow EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_qualification_workflow)
install(FILES "${_qualification_workflow_root}/include/umicom/distribution/runtime/qualification_workflow.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/distribution/runtime" COMPONENT Framework)
umicom_release_inventory_own_headers(umicom_qualification_workflow include/umicom/distribution/runtime/qualification_workflow.h)
if(BUILD_TESTING)
    add_executable(umi-qualification-workflow-test
        "${_qualification_workflow_root}/tests/release_qualification/test_workflow.c"
        "${_qualification_workflow_root}/tests/release_qualification/fixture.c"
        "${_qualification_workflow_root}/tests/release_impact/fixture.c")
    target_link_libraries(umi-qualification-workflow-test PRIVATE Umicom::qualification_workflow)
    umicom_target_qualification_inputs(umi-qualification-workflow-test FILES
        "${_qualification_workflow_root}/tests/release_qualification/fixture.h"
        "${_qualification_workflow_root}/tests/release_impact/fixture.h")
    set_target_properties(umi-qualification-workflow-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umi-qualification-workflow-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umi-qualification-workflow-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-qualification-workflow-test)
    endif()
    foreach(_case IN ITEMS passed arguments cancel-before cancel-prepared cancel-after-build cancel-after-tests
            cancel-after-observation source-before-fails source-after-fails source-changed tool-before-fails
            tool-after-fails tool-before-invalid tool-after-invalid tool-changed build-failed build-invalid
            build-callback-fails tests-failed tests-callback-fails runner-failed missing-test wrong-test malformed no-tests)
        set(_name "framework.qualification.workflow.${_case}")
        add_test(NAME "${_name}" COMMAND umi-qualification-workflow-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 60 LABELS "framework;qualification;workflow;regression")
        umicom_test_impact_owners("${_name}" TARGETS umi-qualification-workflow-test)
    endforeach()
endif()
