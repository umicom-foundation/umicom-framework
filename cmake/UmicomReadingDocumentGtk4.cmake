#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomReadingDocumentGtk4.cmake
#
# PURPOSE:
#   Compose the optional reusable GTK reading presenter and owner-run regression cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# Keep the guard unconsumed until both optional dependencies are available.
if(NOT TARGET Umicom::ui_gtk4 OR NOT TARGET Umicom::browser)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
get_filename_component(_reading_gtk_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_reading_gtk4 STATIC
    "${_reading_gtk_root}/adapters/gtk4/reading_document_gtk4.c")
add_library(Umicom::reading_gtk4 ALIAS umicom_reading_gtk4)
set_target_properties(umicom_reading_gtk4 PROPERTIES EXPORT_NAME reading_gtk4
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_reading_gtk4 PUBLIC Umicom::ui_gtk4 Umicom::browser)
umicom_apply_warnings(umicom_reading_gtk4)
umicom_apply_sanitizers(umicom_reading_gtk4)
install(TARGETS umicom_reading_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_reading_gtk4)
install(FILES "${_reading_gtk_root}/include/umicom/ui/gtk4/reading_document.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-reading-document-gtk4-test
        "${_reading_gtk_root}/tests/browser/test_reading_document_gtk4.c")
    set_target_properties(umicom-reading-document-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-reading-document-gtk4-test PRIVATE Umicom::reading_gtk4)
    umicom_apply_warnings(umicom-reading-document-gtk4-test)
    umicom_apply_sanitizers(umicom-reading-document-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-reading-document-gtk4-test)
    endif()
    foreach(_case IN ITEMS structure
            fonts
            zoom-min
            zoom-max
            selection
            unrelated-tag
            restyle
            empty
            text-mismatch
            null-document
            null-profile
            null-buffer
            null-output
            zoom-low
            zoom-high
            invalid-profile
            invalid-utf8
            split-start
            split-end
            range-overflow
            span-limit
            link-limit
            invalid-kind
            heading-low
            heading-high
            nonheading-level
            cancel-before
            cancel-after
            cancel-retry
            reentrant
            model-release
            profile-capture
            text-edit
            text-edit-restore
            tag-remove
            private-owner
            create-invalid
            create-zoom
            create-profile
            text-capacity
            embedded-nul
            link-split
            tag-remove-late)
        add_test(NAME "framework.browser.reading.gtk4.${_case}"
            COMMAND umicom-reading-document-gtk4-test "${_case}")
        set_tests_properties("framework.browser.reading.gtk4.${_case}" PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;browser;reading;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.browser.reading.gtk4.${_case}"
                TARGETS umicom-reading-document-gtk4-test)
        endif()
    endforeach()
endif()
