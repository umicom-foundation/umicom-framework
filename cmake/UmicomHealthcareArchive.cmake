# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Archive serialization is optional so the portable in-memory Healthcare core
# remains independently composable. JSON belongs to the developer component.
if(NOT TARGET Umicom::healthcare_core OR NOT TARGET Umicom::developer)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
add_library(umicom_healthcare_archive STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/healthcare/access_archive.c")
add_library(Umicom::healthcare_archive ALIAS umicom_healthcare_archive)
set_target_properties(umicom_healthcare_archive PROPERTIES EXPORT_NAME healthcare_archive
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_healthcare_archive PUBLIC Umicom::healthcare_core PRIVATE Umicom::developer)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_healthcare_archive)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_healthcare_archive)
endif()
install(TARGETS umicom_healthcare_archive EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_healthcare_archive)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/healthcare/access_archive.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/healthcare" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_healthcare_archive
        include/umicom/healthcare/access_archive.h)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::healthcare_archive)
endif()
if(TARGET umicom_healthcare_access_gtk4 AND TARGET Umicom::platform)
    target_link_libraries(umicom_healthcare_access_gtk4 PRIVATE Umicom::healthcare_archive Umicom::platform)
    target_compile_definitions(umicom_healthcare_access_gtk4 PRIVATE UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE=1)
endif()

if(BUILD_TESTING)
    add_executable(umicom-healthcare-archive-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/healthcare/test_access_archive.c")
    target_link_libraries(umicom-healthcare-archive-test PRIVATE Umicom::healthcare_archive)
    set_target_properties(umicom-healthcare-archive-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-healthcare-archive-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-healthcare-archive-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-healthcare-archive-test)
    endif()
    foreach(case empty identities lifecycle partial expiry followup revision schema integers invalid late-failure capacity)
        add_test(NAME framework.healthcare.archive.${case}
            COMMAND umicom-healthcare-archive-test ${case})
        set_tests_properties(framework.healthcare.archive.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;healthcare;synthetic;archive;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.healthcare.archive.${case} TARGETS umicom-healthcare-archive-test)
        endif()
    endforeach()
    if(TARGET umicom_healthcare_access_gtk4 AND TARGET Umicom::platform)
        add_executable(umicom-healthcare-archive-gtk-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/healthcare/test_archive_gtk4.c")
        target_link_libraries(umicom-healthcare-archive-gtk-test PRIVATE
            Umicom::healthcare_access_gtk4 Umicom::healthcare_archive)
        set_target_properties(umicom-healthcare-archive-gtk-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-healthcare-archive-gtk-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-healthcare-archive-gtk-test)
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-healthcare-archive-gtk-test)
        endif()
        foreach(case roundtrip overwrite malformed approval busy acceptance-close completion-close)
            add_test(NAME framework.healthcare.archive.gtk.${case}
                COMMAND umicom-healthcare-archive-gtk-test ${case}
                    "${CMAKE_CURRENT_BINARY_DIR}/healthcare-archive-gtk/${case}")
            set_tests_properties(framework.healthcare.archive.gtk.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;healthcare;synthetic;archive;gtk4;lifetime;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.healthcare.archive.gtk.${case} TARGETS umicom-healthcare-archive-gtk-test)
            endif()
        endforeach()
    endif()
endif()
