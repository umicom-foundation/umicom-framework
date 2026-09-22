#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/SourceViewResources.cmake
#
# PURPOSE:
#   Select GtkSourceView 5 data from the same prefix as its resolved DLLs.
#   A linked editor must not ship without its external RelaxNG language schema.
#
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Only data-tree selection lives here. StageRuntime retains copy validation,
# ownership records, hashes, locking and installer integration.
function(umicom_windows_sourceview_resources prefix dependencies out_trees)
    set(_linked FALSE)
    foreach(_dependency IN LISTS dependencies)
        get_filename_component(_name "${_dependency}" NAME)
        string(TOLOWER "${_name}" _name)
        if(_name MATCHES "^(lib)?gtksourceview-5(-[0-9]+)?\\.dll$")
            set(_linked TRUE)
        endif()
    endforeach()
    set(_relative "share/gtksourceview-5")
    set(_root "${prefix}/${_relative}")
    set(_schema "${_root}/language-specs/language2.rng")
    if(NOT _linked AND NOT EXISTS "${_root}")
        set(${out_trees} "" PARENT_SCOPE)
        return()
    endif()
    if(IS_SYMLINK "${_root}" OR IS_SYMLINK "${_root}/language-specs" OR
            IS_SYMLINK "${_schema}" OR NOT EXISTS "${_schema}" OR
            IS_DIRECTORY "${_schema}")
        message(FATAL_ERROR
            "Umicom GtkSourceView data is incomplete: a regular language-specs/language2.rng "
            "is required under ${_root}. Repair the selected runtime prefix before packaging.")
    endif()
    file(SIZE "${_schema}" _size)
    if(_size EQUAL 0)
        message(FATAL_ERROR "Umicom GtkSourceView data is incomplete: language2.rng is empty")
    endif()
    # Copy the complete versioned tree, not just the schema. Languages, styles
    # and snippets then share the existing XDG_DATA_DIRS relocation boundary.
    set(${out_trees} "${_relative}" PARENT_SCOPE)
endfunction()
