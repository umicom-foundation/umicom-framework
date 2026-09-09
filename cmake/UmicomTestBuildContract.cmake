#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestBuildContract.cmake
#
# PURPOSE:
#   Fail configuration early when an opted-in diagnostic test has lost its
#   explicit public Framework dependency. Never repair the graph silently.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# This build-tree contract is for small tests using test_runtime/check.h.
# The caller explicitly links Umicom::base before invoking this assertion.
# It deliberately checks a direct dependency: a global include path, or an
# unrelated library that happens to expose the SDK, is not this contract.
function(umicom_assert_test_build_contract target)
    if(NOT ARGC EQUAL 1)
        message(FATAL_ERROR
            "Umicom test build contract: supply exactly one executable target.")
    endif()
    if(NOT TARGET "${target}")
        message(FATAL_ERROR
            "Umicom test build contract: target '${target}' does not exist.")
    endif()
    get_target_property(_umicom_test_target "${target}" ALIASED_TARGET)
    if(NOT _umicom_test_target)
        set(_umicom_test_target "${target}")
    endif()
    get_target_property(_umicom_test_imported "${_umicom_test_target}" IMPORTED)
    get_target_property(_umicom_test_type "${_umicom_test_target}" TYPE)
    if(_umicom_test_imported OR NOT _umicom_test_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR
            "Umicom test build contract: '${target}' must be a local executable.")
    endif()
    if(NOT TARGET Umicom::base)
        message(FATAL_ERROR
            "Umicom test build contract: Umicom::base is unavailable. "
            "Configure Framework before its test consumers.")
    endif()
    get_target_property(_umicom_base_target Umicom::base ALIASED_TARGET)
    if(NOT _umicom_base_target)
        set(_umicom_base_target Umicom::base)
    endif()

    # Resolve relative to this reusable module, never the calling application
    # or a machine-specific path. Installed-SDK consumers have a separate role.
    get_filename_component(_umicom_framework_root
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    if(NOT EXISTS
       "${_umicom_framework_root}/include/umicom/test_runtime/check.h")
        message(FATAL_ERROR
            "Umicom test build contract: missing Framework header "
            "'${_umicom_framework_root}/include/umicom/test_runtime/check.h'. "
            "Restore the parent-pinned Framework checkout; do not copy headers.")
    endif()

    get_target_property(_umicom_test_links "${_umicom_test_target}" LINK_LIBRARIES)
    set(_umicom_has_base FALSE)
    foreach(_umicom_link IN LISTS _umicom_test_links)
        if(NOT TARGET "${_umicom_link}")
            continue()
        endif()
        get_target_property(_umicom_link_target "${_umicom_link}" ALIASED_TARGET)
        if(NOT _umicom_link_target)
            set(_umicom_link_target "${_umicom_link}")
        endif()
        if(_umicom_link_target STREQUAL _umicom_base_target)
            set(_umicom_has_base TRUE)
            break()
        endif()
    endforeach()
    if(NOT _umicom_has_base)
        message(FATAL_ERROR
            "Umicom test build contract: '${target}' must link Umicom::base "
            "directly. Add target_link_libraries(${_umicom_test_target} "
            "PRIVATE Umicom::base). Global include paths and add_dependencies "
            "do not declare this SDK usage requirement.")
    endif()
endfunction()
