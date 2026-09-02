#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_gtk_dropdown_ownership.cmake
#
# PURPOSE:
#   Prevent GTK dropdown models from being released twice. All model-backed
#   dropdowns must pass through the Framework helper whose name documents the
#   ownership transfer.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED UMICOM_SUITE_SOURCE_DIR)
    message(FATAL_ERROR "UMICOM_SUITE_SOURCE_DIR is required")
endif()

file(GLOB_RECURSE UMICOM_GTK_SOURCES LIST_DIRECTORIES FALSE
    "${UMICOM_SUITE_SOURCE_DIR}/framework/adapters/gtk4/*.c"
    "${UMICOM_SUITE_SOURCE_DIR}/applications/*.c")

set(UMICOM_DROPDOWN_HELPER
    "${UMICOM_SUITE_SOURCE_DIR}/framework/adapters/gtk4/drop_down_gtk4.c")

foreach(UMICOM_GTK_SOURCE IN LISTS UMICOM_GTK_SOURCES)
    # The helper is the one deliberate direct call that receives GTK's full
    # ownership transfer. Application and panel code must use that helper.
    if(UMICOM_GTK_SOURCE STREQUAL UMICOM_DROPDOWN_HELPER)
        continue()
    endif()

    file(READ "${UMICOM_GTK_SOURCE}" UMICOM_GTK_SOURCE_TEXT)
    string(REGEX MATCH
        "gtk_drop_down_new[ \t\r\n]*\\([ \t\r\n]*G_LIST_MODEL"
        UMICOM_UNSAFE_DROPDOWN_CALL
        "${UMICOM_GTK_SOURCE_TEXT}")
    if(UMICOM_UNSAFE_DROPDOWN_CALL)
        message(FATAL_ERROR
            "Direct model-backed dropdown construction bypasses the Framework ownership helper: ${UMICOM_GTK_SOURCE}")
    endif()
endforeach()
