# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# The GTK projection consumes immutable Framework evidence; it owns no executor.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomImpactInventory.cmake")
include(GNUInstallDirs)
if(NOT TARGET Umicom::ui_gtk4 OR NOT TARGET Umicom::distribution)
    return()
endif()
include_guard(GLOBAL)
get_filename_component(_qualification_ui_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_qualification_gtk4 STATIC "${_qualification_ui_root}/adapters/gtk4/qualification_review.c")
add_library(Umicom::qualification_gtk4 ALIAS umicom_qualification_gtk4)
set_target_properties(umicom_qualification_gtk4 PROPERTIES
    EXPORT_NAME qualification_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_qualification_gtk4 PUBLIC Umicom::ui_gtk4 Umicom::distribution)
target_include_directories(umicom_qualification_gtk4 PUBLIC
    "$<BUILD_INTERFACE:${_qualification_ui_root}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_qualification_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_qualification_gtk4)
endif()
install(TARGETS umicom_qualification_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_qualification_gtk4)
install(FILES "${_qualification_ui_root}/include/umicom/ui/gtk4/qualification_review.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
umicom_release_inventory_own_headers(umicom_qualification_gtk4 include/umicom/ui/gtk4/qualification_review.h)
if(BUILD_TESTING)
    add_executable(umi-qualification-review-test
        "${_qualification_ui_root}/tests/release_qualification/test_review_gtk4.c"
        "${_qualification_ui_root}/tests/release_qualification/fixture.c"
        "${_qualification_ui_root}/tests/release_impact/fixture.c")
    target_link_libraries(umi-qualification-review-test PRIVATE Umicom::qualification_gtk4)
    umicom_target_qualification_inputs(umi-qualification-review-test FILES
        "${_qualification_ui_root}/tests/release_qualification/fixture.h"
        "${_qualification_ui_root}/tests/release_impact/fixture.h")
    set_target_properties(umi-qualification-review-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umi-qualification-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umi-qualification-review-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-qualification-review-test)
    endif()
    foreach(_case IN ITEMS paging invalid lifetime archived)
        set(_name "framework.qualification.gtk4.${_case}")
        add_test(NAME "${_name}" COMMAND umi-qualification-review-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;qualification;gtk4;ownership;regression")
        umicom_test_impact_owners("${_name}" TARGETS umi-qualification-review-test)
    endforeach()
endif()
