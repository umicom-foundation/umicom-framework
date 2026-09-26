#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeskFederationCore.cmake
# PURPOSE: Build saved-workspace and reviewed-dispatch services without a GUI.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(TARGET Umicom::desk_federation)
    return()
endif()
include(GNUInstallDirs)
set(_umi_federation_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET Umicom::data)
    message(FATAL_ERROR "Desk federation requires the canonical Umicom::data target")
endif()
add_library(umicom_desk_federation STATIC
    "${_umi_federation_root}/src/desktop/federation/validation.c"
    "${_umi_federation_root}/src/desktop/federation/codec.c"
    "${_umi_federation_root}/src/desktop/federation/links.c"
    "${_umi_federation_root}/src/desktop/federation/repository.c"
    "${_umi_federation_root}/src/desktop/federation/service.c"
    "${_umi_federation_root}/src/desktop/federation/workspaces.c"
    "${_umi_federation_root}/src/desktop/federation/planning.c"
    "${_umi_federation_root}/src/desktop/federation/activity.c"
    "${_umi_federation_root}/src/desktop/federation/text.c")
add_library(Umicom::desk_federation ALIAS umicom_desk_federation)
set_target_properties(umicom_desk_federation PROPERTIES EXPORT_NAME desk_federation
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_desk_federation PUBLIC
    $<BUILD_INTERFACE:${_umi_federation_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_desk_federation PUBLIC Umicom::data)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_desk_federation)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_desk_federation)
endif()
install(TARGETS umicom_desk_federation EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umi_federation_root}/include/umicom/desktop/federation"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/desktop" COMPONENT Framework)
install(FILES "${_umi_federation_root}/docs/DESK_WORKSPACES.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-desk-federation-test
        "${_umi_federation_root}/tests/desk_federation/test_main.c"
        "${_umi_federation_root}/tests/desk_federation/test_support.c"
        "${_umi_federation_root}/tests/desk_federation/test_groups.c"
        "${_umi_federation_root}/tests/desk_federation/test_review.c"
        "${_umi_federation_root}/tests/desk_federation/test_links.c"
        "${_umi_federation_root}/tests/desk_federation/test_storage.c"
        "${_umi_federation_root}/tests/desk_federation/test_activity.c")
    target_link_libraries(umicom-desk-federation-test PRIVATE Umicom::desk_federation)
    set_target_properties(umicom-desk-federation-test PROPERTIES C_STANDARD 23
        C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-desk-federation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-desk-federation-test)
    endif()
    include("${_umi_federation_root}/tests/desk_federation/cases.cmake")
    foreach(case IN LISTS UMICOM_DESK_FEDERATION_TEST_CASES)
        add_test(NAME framework.desk_federation.${case}
            COMMAND umicom-desk-federation-test "${case}"
                "${CMAKE_CURRENT_BINARY_DIR}/desk-federation-${case}.sqlite")
        set_tests_properties(framework.desk_federation.${case} PROPERTIES TIMEOUT 60
            LABELS "framework;desktop;desk-federation;regression")
        # Only tests needing SQLite may report it unavailable; ordinary test
        # failures and a missing display are never reclassified as core passes.
        if(case MATCHES "^storage[.]sqlite_")
            set_tests_properties(framework.desk_federation.${case} PROPERTIES SKIP_RETURN_CODE 77)
        endif()
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-desk-federation-test)
    endif()
endif()
unset(_umi_federation_root)
