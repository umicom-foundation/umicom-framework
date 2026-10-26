# Umicom Framework
# File: cmake/UmicomSearchPages.cmake
# PURPOSE: Register search-page SDK ownership and isolated regression cases.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_platform)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_search_page_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_platform include/umicom/platform/search_page.h)
endif()
install(FILES "${_search_page_root}/include/umicom/platform/search_page.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform" COMPONENT Framework)
install(FILES "${_search_page_root}/docs/guides/paged-file-search.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umi-search-page-test "${_search_page_root}/tests/search_page/test_page.c")
    target_link_libraries(umi-search-page-test PRIVATE Umicom::platform)
    set_target_properties(umi-search-page-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-search-page-test)
    umicom_apply_sanitizers(umi-search-page-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-search-page-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-search-page-test FILES
            "${_search_page_root}/tests/build_log/fixture.h"
            "${_search_page_root}/tests/support/symbolic_link.h")
    endif()
    foreach(case IN ITEMS normal default align last overflow empty single maximum limit null bounds immutable new-request invalid-request invalidate index-change index-unchanged other-session after-owner filter cancel-completed)
        set(test "framework.search.page.${case}")
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/search-page/${case}")
        file(MAKE_DIRECTORY "${directory}")
        add_test(NAME ${test} COMMAND umi-search-page-test ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 WORKING_DIRECTORY "${directory}"
            LABELS "framework;search;page;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-search-page-test)
        endif()
    endforeach()
endif()
unset(_search_page_root)
