#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFinanceUiPlatform.cmake
#
# PURPOSE:
#   Build one toolkit-neutral finance UI projection over the existing finance
#   and UI targets for Bank, TMS, Accountant and other thin applications.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET Umicom::finance OR NOT TARGET Umicom::ui)
    message(FATAL_ERROR
        "UmicomFinanceUiPlatform requires existing Umicom::finance and Umicom::ui targets")
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_finance_ui)
    add_library(umicom_finance_ui STATIC
        "${CMAKE_CURRENT_LIST_DIR}/../src/finance_ui/finance_views.c")
    target_sources(umicom_finance_ui PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/finance_ui/funds_review.c")
    add_library(Umicom::finance_ui ALIAS umicom_finance_ui)
    set_target_properties(umicom_finance_ui PROPERTIES EXPORT_NAME finance_ui)
    target_link_libraries(umicom_finance_ui PUBLIC Umicom::finance Umicom::ui)
    target_include_directories(umicom_finance_ui PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(umicom_finance_ui)
    umicom_apply_sanitizers(umicom_finance_ui)
endif()

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING AND NOT TARGET umicom-finance-ui-test)
    add_executable(umicom-finance-ui-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/finance_ui/test_finance_views.c")
    target_link_libraries(umicom-finance-ui-test PRIVATE Umicom::finance_ui)
    umicom_apply_warnings(umicom-finance-ui-test)
    umicom_apply_sanitizers(umicom-finance-ui-test)
    add_test(NAME framework.finance_ui.views COMMAND umicom-finance-ui-test)
endif()

# Attach to the existing native adapter after both targets exist. No extra
# financial GTK library or app-local calculation implementation is introduced.
function(umicom_finance_attach_review_gtk4)
    if(TARGET umicom_ui_gtk4)
        get_target_property(_attached umicom_ui_gtk4 UMICOM_FINANCE_REVIEW_ATTACHED)
        if(NOT _attached)
            target_sources(umicom_ui_gtk4 PRIVATE
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/finance_review_gtk4.c")
            target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::finance_ui)
            set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_FINANCE_REVIEW_ATTACHED TRUE)
        endif()
    endif()
endfunction()
umicom_finance_attach_review_gtk4()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_finance_attach_review_gtk4)
endif()
