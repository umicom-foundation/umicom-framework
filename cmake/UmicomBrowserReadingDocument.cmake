#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrowserReadingDocument.cmake
#
# PURPOSE:
#   Compose the reusable bounded reading projection and its regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_reading_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_browser PRIVATE
    "${_reading_root}/src/browser/reading_document.c"
    "${_reading_root}/src/browser/reading_document_tokens.c"
    "${_reading_root}/src/browser/reading_document_builder.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_browser FILES
        "${_reading_root}/src/browser/reading_document_internal.h")
endif()
install(FILES "${_reading_root}/include/umicom/browser/reading_document.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/browser" COMPONENT FrameworkSDK)
if(BUILD_TESTING)
    add_executable(umicom-browser-reading-document-test
        "${_reading_root}/tests/browser/test_reading_document.c")
    set_target_properties(umicom-browser-reading-document-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-browser-reading-document-test PRIVATE Umicom::browser)
    umicom_apply_warnings(umicom-browser-reading-document-test)
    umicom_apply_sanitizers(umicom-browser-reading-document-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-browser-reading-document-test)
    endif()
    foreach(_case empty paragraphs headings code inline nested entities unknown-entity comments hidden script head template svg external table list image image-missing image-decorative links quoted-angle unquoted anchor-entity unsupported mismatch unclosed-style unclosed-script unclosed-comment unclosed-quote duplicate-anchor duplicate-attribute anchor-space numeric-nul numeric-surrogate numeric-overflow numeric-control external-doctype limits arguments span-capacity link-capacity depth anchor-capacity)
        add_test(NAME "framework.browser.reading_document.${_case}"
            COMMAND umicom-browser-reading-document-test "${_case}")
        set_tests_properties("framework.browser.reading_document.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.browser.reading_document.${_case}"
                TARGETS umicom-browser-reading-document-test)
        endif()
    endforeach()
endif()

install(FILES "${_reading_root}/examples/reading_document/main.c"
    "${_reading_root}/examples/reading_document/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/reading_document" COMPONENT FrameworkSDK)
