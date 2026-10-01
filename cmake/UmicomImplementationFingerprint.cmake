#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomImplementationFingerprint.cmake
# PURPOSE: Rebuild an owning library when copied implementation contents change.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Use relative names and file contents, not modification times or checkout
# locations. A restored source can be older than its object while containing a
# new function. Reconfiguring must still change that owner's compile command.
function(umicom_compute_implementation_fingerprint out_value source_root)
    file(REAL_PATH "${source_root}" root)
    set(inputs "")
    foreach(input IN LISTS ARGN)
        file(REAL_PATH "${input}" absolute BASE_DIRECTORY "${root}")
        file(RELATIVE_PATH relative "${root}" "${absolute}")
        if(IS_ABSOLUTE "${relative}" OR relative MATCHES "^\\.\\.(/|$)")
            message(FATAL_ERROR "Implementation input is outside its source root: ${input}")
        endif()
        if(NOT EXISTS "${absolute}" OR IS_DIRECTORY "${absolute}")
            message(FATAL_ERROR "Implementation input is missing: ${absolute}")
        endif()
        list(APPEND inputs "${relative}")
    endforeach()
    if(NOT inputs)
        message(FATAL_ERROR "An implementation fingerprint needs at least one source input.")
    endif()
    list(REMOVE_DUPLICATES inputs)
    list(SORT inputs)
    set(manifest "")
    foreach(relative IN LISTS inputs)
        file(SHA256 "${root}/${relative}" digest)
        string(APPEND manifest "${relative}:${digest}\n")
        # Ordinary edits trigger regeneration. After a timestamp-preserving
        # copy, run configure explicitly: timestamp-based build tools cannot
        # discover an older replacement until its contents are inspected.
        if(NOT DEFINED CMAKE_SCRIPT_MODE_FILE)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${root}/${relative}")
        endif()
    endforeach()
    string(SHA256 fingerprint "${manifest}")
    set(${out_value} "${fingerprint}" PARENT_SCOPE)
endfunction()

# The former target-wide definition rebuilt every object when one source
# changed. Source-level definitions below preserve content-based invalidation
# while limiting it to the translation units that use the changed input.
# Keep the previous implementation disabled here for architectural review.
if(FALSE)
# Call after every module has added its sources to the owner. Include private
# shared headers explicitly: they can add function bodies without changing a
# public declaration. Keep this definition PRIVATE so installed clients do not
# inherit implementation details or unnecessary rebuilds.
function(umicom_attach_implementation_fingerprint target source_root)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Implementation owner is missing: ${target}")
    endif()
    get_target_property(owner_directory "${target}" SOURCE_DIR)
    get_target_property(sources "${target}" SOURCES)
    set(inputs ${ARGN})
    foreach(source IN LISTS sources)
        if(source MATCHES "\\$<")
            message(FATAL_ERROR "${target}: register a concrete fingerprint input for generated source ${source}")
        endif()
        get_filename_component(absolute "${source}" ABSOLUTE BASE_DIR "${owner_directory}")
        list(APPEND inputs "${absolute}")
    endforeach()
    umicom_compute_implementation_fingerprint(fingerprint "${source_root}" ${inputs})
    target_compile_definitions("${target}" PRIVATE "UMI_IMPLEMENTATION_FINGERPRINT=\"${fingerprint}\"")
    set_property(TARGET "${target}" PROPERTY UMICOM_IMPLEMENTATION_FINGERPRINT "${fingerprint}")
endfunction()
endif()

# Follow existing quoted includes beside each source, including nested .inc
# files and private headers. This is deliberately conservative across #if
# branches, not a replacement for the compiler's dependency scanner. Public
# headers remain covered by compiler dependencies and the public-contract
# fingerprint. For macro-generated or include-path-only private dependencies,
# callers can supply explicit additional inputs to the attachment function.
function(umicom_collect_private_implementation_inputs out_inputs source_root source)
    file(REAL_PATH "${source_root}" root)
    file(REAL_PATH "${source}" first BASE_DIRECTORY "${root}")
    set(pending "${first}")
    set(inputs "")
    while(pending)
        list(POP_FRONT pending input)
        if(input IN_LIST inputs)
            continue()
        endif()
        file(RELATIVE_PATH relative "${root}" "${input}")
        if(IS_ABSOLUTE "${relative}" OR relative MATCHES "^\\.\\.(/|$)")
            message(FATAL_ERROR "Implementation input is outside its source root: ${input}")
        endif()
        if(NOT EXISTS "${input}" OR IS_DIRECTORY "${input}")
            message(FATAL_ERROR "Implementation input is missing: ${input}")
        endif()
        list(APPEND inputs "${input}")
        get_filename_component(directory "${input}" DIRECTORY)
        file(STRINGS "${input}" includes REGEX "^[ \t]*#[ \t]*include[ \t]*\"")
        foreach(line IN LISTS includes)
            if(line MATCHES "^[ \t]*#[ \t]*include[ \t]*\"([^\"]+)\"")
                file(REAL_PATH "${CMAKE_MATCH_1}" candidate BASE_DIRECTORY "${directory}")
                file(RELATIVE_PATH relative "${root}" "${candidate}")
                if(EXISTS "${candidate}" AND NOT IS_DIRECTORY "${candidate}"
                    AND NOT IS_ABSOLUTE "${relative}" AND NOT relative MATCHES "^\\.\\.(/|$)"
                    AND NOT relative MATCHES "^include/umicom/")
                    list(APPEND pending "${candidate}")
                endif()
            endif()
        endforeach()
    endwhile()
    list(SORT inputs)
    set(${out_inputs} "${inputs}" PARENT_SCOPE)
endfunction()

# Attach after all modules have registered concrete source files. The macro is
# private build metadata: it changes the compiler command but is not consumed
# by application logic or exported to installed users. Source properties live
# in the target's CMake directory; preserve unrelated definitions and merge
# explicit extra inputs when a source is shared by targets in that directory.
function(umicom_attach_implementation_fingerprint target source_root)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Implementation owner is missing: ${target}")
    endif()
    get_target_property(owner_directory "${target}" SOURCE_DIR)
    get_target_property(sources "${target}" SOURCES)
    set(owner_inputs "")
    foreach(source IN LISTS sources)
        if(source MATCHES "\\$<")
            message(FATAL_ERROR "${target}: register a concrete fingerprint input for generated source ${source}")
        endif()
        # Headers and fragments contribute through their including source;
        # they must never become separate compilation units.
        if(NOT source MATCHES "[.](c|cc|cpp|cxx|m|mm|s|S)$")
            continue()
        endif()
        get_filename_component(absolute "${source}" ABSOLUTE BASE_DIR "${owner_directory}")
        umicom_collect_private_implementation_inputs(inputs "${source_root}" "${absolute}")
        get_property(extra_inputs SOURCE "${absolute}" TARGET_DIRECTORY "${target}"
            PROPERTY UMICOM_IMPLEMENTATION_EXTRA_INPUTS)
        foreach(extra IN LISTS ARGN)
            file(REAL_PATH "${extra}" extra_absolute BASE_DIRECTORY "${source_root}")
            list(APPEND extra_inputs "${extra_absolute}")
        endforeach()
        list(REMOVE_DUPLICATES extra_inputs)
        set_property(SOURCE "${absolute}" TARGET_DIRECTORY "${target}"
            PROPERTY UMICOM_IMPLEMENTATION_EXTRA_INPUTS "${extra_inputs}")
        list(APPEND inputs ${extra_inputs})
        umicom_compute_implementation_fingerprint(fingerprint "${source_root}" ${inputs})
        get_property(definitions SOURCE "${absolute}" TARGET_DIRECTORY "${target}"
            PROPERTY COMPILE_DEFINITIONS)
        list(FILTER definitions EXCLUDE REGEX "^UMI_IMPLEMENTATION_FINGERPRINT=")
        list(APPEND definitions "UMI_IMPLEMENTATION_FINGERPRINT=\"${fingerprint}\"")
        set_property(SOURCE "${absolute}" TARGET_DIRECTORY "${target}"
            PROPERTY COMPILE_DEFINITIONS "${definitions}")
        list(APPEND owner_inputs ${inputs})
    endforeach()
    # Retain an aggregate identity for diagnostics and regression inspection.
    # It does not participate in every object's command line.
    umicom_compute_implementation_fingerprint(identity "${source_root}" ${owner_inputs})
    set_property(TARGET "${target}" PROPERTY UMICOM_IMPLEMENTATION_FINGERPRINT "${identity}")
endfunction()
