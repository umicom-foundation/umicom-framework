#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomMarkdownReading.cmake
#
# PURPOSE:
#   Compose the bounded Markdown reading profile and installed SDK learning checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_browser PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/browser/reading_markdown.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/browser/reading_markdown_inline.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_browser FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/browser/reading_markdown_internal.h")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/browser/reading_markdown.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/browser" COMPONENT FrameworkSDK)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/markdown-reading.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/markdown_reading/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/markdown_reading/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/markdown_reading" COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-markdown-reading-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/browser/test_reading_markdown.c")
    target_link_libraries(umicom-markdown-reading-test PRIVATE Umicom::browser)
    set_target_properties(umicom-markdown-reading-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-markdown-reading-test)
    umicom_apply_sanitizers(umicom-markdown-reading-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-markdown-reading-test)
    endif()
    foreach(_case empty paragraph blank crlf heading heading-indent heading-six heading-seven heading-space heading-empty heading-four-spaces list ordered-literal strong emphasis nested triple-literal unmatched space-markers underscore-literal escape code code-entity code-island multi-code-literal fence fence-unclosed fence-crlf fence-marker fence-literal fence-close-label html-literal entities-literal link-literal image-literal table-literal unicode embedded-nul utf8 control delete over-limit span-capacity work-capacity exact-span source-limit null-source null-output)
        add_test(NAME framework.browser.markdown_reading.${_case}
            COMMAND umicom-markdown-reading-test "${_case}")
        set_tests_properties(framework.browser.markdown_reading.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;markdown;unicode;reading;regression;no-execution")
    endforeach()
    add_executable(umicom-markdown-reading-example
        "${CMAKE_CURRENT_LIST_DIR}/../examples/markdown_reading/main.c")
    target_link_libraries(umicom-markdown-reading-example PRIVATE Umicom::browser)
    set_target_properties(umicom-markdown-reading-example PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-markdown-reading-example)
    umicom_apply_sanitizers(umicom-markdown-reading-example)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-markdown-reading-example
        PRODUCT_NAME "Umicom Markdown Reading Example" INTERNAL_NAME "umicom-markdown-reading-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-markdown-reading-example)
    endif()
    foreach(_case structure literal refusal)
        add_test(NAME framework.browser.markdown_reading-example.${_case}
            COMMAND umicom-markdown-reading-example "${_case}")
        set_tests_properties(framework.browser.markdown_reading-example.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;browser;markdown;reading;learning;no-execution")
    endforeach()
endif()
