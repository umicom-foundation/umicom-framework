#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLibraryLinkChecks.cmake
# PURPOSE: Make editor-library symbol checks reusable build prerequisites.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Keep the source owner visible at configuration time. This catches an omitted
# source-list merge before a large build, rather than quietly adding a duplicate
# implementation to whichever executable happened to fail.
function(umicom_require_library_source target relativePath)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Required Framework library is missing: ${target}")
    endif()
    get_target_property(ownerDirectory "${target}" SOURCE_DIR)
    get_target_property(sources "${target}" SOURCES)
    get_filename_component(requiredPath "${relativePath}" ABSOLUTE BASE_DIR "${ownerDirectory}")
    if(NOT EXISTS "${requiredPath}")
        message(FATAL_ERROR "Required source is missing: ${requiredPath}. Restore the complete Framework file.")
    endif()
    set(found FALSE)
    foreach(source IN LISTS sources)
        if(source MATCHES "^\\$<")
            continue()
        endif()
        get_filename_component(candidate "${source}" ABSOLUTE BASE_DIR "${ownerDirectory}")
        if(candidate STREQUAL requiredPath)
            set(found TRUE)
        endif()
    endforeach()
    if(NOT found)
        message(FATAL_ERROR "${target} does not compile ${relativePath}. Merge the matching Framework CMakeLists.txt.")
    endif()
endfunction()

# A custom target runs whenever requested. An unchanged archive is inspected
# again; a stale report is never used as the reason to skip the check.
function(umicom_add_library_symbol_check target checkTarget symbolFile)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR MSVC)
        add_custom_target("${checkTarget}"
            COMMAND "${CMAKE_COMMAND}" -E echo "Archive symbol inspection requires GNU/LLVM nm. Use the real executable link checks for this toolchain."
            COMMAND "${CMAKE_COMMAND}" -E false VERBATIM)
        return()
    endif()
    set(underscore OFF)
    if(APPLE OR (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4))
        set(underscore ON)
    endif()
    add_custom_target("${checkTarget}"
        COMMAND "${CMAKE_COMMAND}"
            "-DARCHIVE=$<TARGET_FILE:${target}>"
            "-DSYMBOL_FILE=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/link_contracts/${symbolFile}"
            "-DNM_TOOL=${CMAKE_NM}"
            "-DALLOW_LEADING_UNDERSCORE=${underscore}"
            "-DREPORT=${CMAKE_BINARY_DIR}/link-reports/$<CONFIG>/${target}.json"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/VerifyLibrarySymbols.cmake"
        COMMENT "Verify ${target} editor function definitions" VERBATIM)
    add_dependencies("${checkTarget}" "${target}")
endfunction()

umicom_require_library_source(umicom_document "src/document/close_session.c")
umicom_require_library_source(umicom_document "src/document/save_session.c")
umicom_require_library_source(umicom_ui "src/ui/workbench.c")
umicom_add_library_symbol_check(umicom_document umicom-document-symbol-check document.symbols)
umicom_add_library_symbol_check(umicom_ui umicom-workbench-symbol-check workbench.symbols)
add_custom_target(umicom-editor-core-link-check)
add_dependencies(umicom-editor-core-link-check umicom-document-symbol-check umicom-workbench-symbol-check)
if(TARGET umicom_ui_gtk4)
    umicom_require_library_source(umicom_ui_gtk4 "adapters/gtk4/document_commands_gtk4.c")
    umicom_require_library_source(umicom_ui_gtk4 "adapters/gtk4/document_saving_gtk4.c")
    umicom_add_library_symbol_check(umicom_ui_gtk4 umicom-gtk4-document-symbol-check gtk4_document.symbols)
    add_custom_target(umicom-editor-native-link-check)
    add_dependencies(umicom-editor-native-link-check umicom-editor-core-link-check umicom-gtk4-document-symbol-check)
else()
    add_custom_target(umicom-editor-native-link-check
        COMMAND "${CMAKE_COMMAND}" -E echo "Native editor linkage requires the real umicom_ui_gtk4 library. Enable the GTK configuration."
        COMMAND "${CMAKE_COMMAND}" -E false VERBATIM)
endif()
