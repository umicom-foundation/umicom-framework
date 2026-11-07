#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomStageLearningModule.cmake
#
# PURPOSE:
#   Validate declared learning files and stage complete content without executing it.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

if(NOT IS_ABSOLUTE "${UMI_MODULE_SOURCE}")
    message(FATAL_ERROR "An absolute learning module source is required")
endif()
function(_umicom_module_path relative)
    if(relative STREQUAL "" OR IS_ABSOLUTE "${relative}" OR
       relative MATCHES "(^|/)\\.\\.?(/|$)|//|[\\\\:;]|/$")
        message(FATAL_ERROR "Invalid module-relative file: ${relative}")
    endif()
    string(REPLACE "/" ";" _parts "${relative}")
    set(_path "${UMI_MODULE_SOURCE}")
    foreach(_part IN LISTS _parts)
        string(APPEND _path "/${_part}")
        if(IS_SYMLINK "${_path}")
            message(FATAL_ERROR "Linked learning resource is not accepted: ${relative}")
        endif()
    endforeach()
    if(NOT EXISTS "${_path}" OR IS_DIRECTORY "${_path}")
        message(FATAL_ERROR "Missing learning resource: ${relative}")
    endif()
endfunction()
_umicom_module_path("learning-pack.json")
file(SIZE "${UMI_MODULE_SOURCE}/learning-pack.json" _size)
if(_size GREATER 262144)
    message(FATAL_ERROR "Learning manifest exceeds 256 KiB")
endif()
file(READ "${UMI_MODULE_SOURCE}/learning-pack.json" _manifest)
string(JSON _schema GET "${_manifest}" schema)
string(JSON _version GET "${_manifest}" version)
string(JSON _id GET "${_manifest}" id)
string(JSON _product GET "${_manifest}" product)
if(NOT _schema STREQUAL "umicom.learning.pack" OR NOT _version EQUAL 1 OR
   NOT _id STREQUAL UMI_MODULE_ID OR NOT _product STREQUAL UMI_MODULE_PRODUCT)
    message(FATAL_ERROR "Learning module identity does not match the configured offer")
endif()
string(JSON _count LENGTH "${_manifest}" resources)
if(_count LESS 1 OR _count GREATER 256)
    message(FATAL_ERROR "A bundled module must declare 1 to 256 resources")
endif()
set(UMI_MODULE_FILES "learning-pack.json")
set(_checksum_mismatches "")
math(EXPR _last "${_count} - 1")
foreach(_index RANGE 0 ${_last})
    string(JSON _file GET "${_manifest}" resources ${_index} path)
    string(JSON _expected GET "${_manifest}" resources ${_index} sha256)
    _umicom_module_path("${_file}")
    file(SHA256 "${UMI_MODULE_SOURCE}/${_file}" _actual)
    if(NOT _actual STREQUAL _expected)
        # The former first-mismatch error is retained for engineering review.
        # Collect every changed resource so one content review can repair the
        # complete module. Staging still begins only after all hashes match.
        if(FALSE)
            message(FATAL_ERROR "Learning resource checksum mismatch: ${_file}; review and regenerate its manifest")
        endif()
        list(APPEND _checksum_mismatches "${_file}")
    endif()
    list(APPEND UMI_MODULE_FILES "${_file}")
endforeach()
list(LENGTH _checksum_mismatches _mismatch_count)
if(_mismatch_count GREATER 0)
    list(JOIN _checksum_mismatches "\n  " _mismatch_paths)
    message(FATAL_ERROR
        "Learning resource checksum mismatch: ${_mismatch_count} resource(s)\n"
        "Manifest: ${UMI_MODULE_SOURCE}/learning-pack.json\n"
        "Changed files:\n  ${_mismatch_paths}\n"
        "Review these files and prepare a matching manifest before configuring again. "
        "No learning files were staged by this invocation.")
endif()
foreach(_file IN LISTS UMI_MODULE_EXTRA_FILES)
    _umicom_module_path("${_file}")
    list(APPEND UMI_MODULE_FILES "${_file}")
endforeach()
list(REMOVE_DUPLICATES UMI_MODULE_FILES)
if(NOT UMI_MODULE_VALIDATE_ONLY)
    if(NOT IS_ABSOLUTE "${UMI_MODULE_DESTINATION}")
        message(FATAL_ERROR "An absolute staging destination is required")
    endif()
    file(REAL_PATH "${UMI_MODULE_SOURCE}" _source)
    # REAL_PATH requires an existing path. Resolve only the existing ancestor,
    # then append missing directories without creating anything during preflight.
    # The earlier unconditional resolution is retained for review.
    if(FALSE)
        file(REAL_PATH "${UMI_MODULE_DESTINATION}" _destination)
    endif()
    set(_ancestor "${UMI_MODULE_DESTINATION}")
    set(_suffix "")
    while(NOT EXISTS "${_ancestor}")
        if(IS_SYMLINK "${_ancestor}")
            message(FATAL_ERROR "Linked learning destination is not accepted")
        endif()
        get_filename_component(_name "${_ancestor}" NAME)
        get_filename_component(_parent "${_ancestor}" DIRECTORY)
        if(_parent STREQUAL _ancestor OR _parent STREQUAL "")
            message(FATAL_ERROR "Learning destination has no existing ancestor")
        endif()
        set(_suffix "/${_name}${_suffix}")
        set(_ancestor "${_parent}")
    endwhile()
    if(NOT IS_DIRECTORY "${_ancestor}")
        message(FATAL_ERROR "Learning destination ancestor is not a directory")
    endif()
    file(REAL_PATH "${_ancestor}" _ancestor_real)
    cmake_path(SET _destination NORMALIZE "${_ancestor_real}${_suffix}")
    # Windows file identity is normally case insensitive; differently cased
    # spellings must not bypass the source/destination overlap check.
    if(WIN32)
        string(TOLOWER "${_source}" _source)
        string(TOLOWER "${_destination}" _destination)
    endif()
    cmake_path(IS_PREFIX _source "${_destination}" NORMALIZE _inside_source)
    cmake_path(IS_PREFIX _destination "${_source}" NORMALIZE _contains_source)
    if(_inside_source OR _contains_source)
        message(FATAL_ERROR "Learning source and destination must not overlap")
    endif()
    # Check every destination before writing any resource. A stale linked
    # directory or file must not redirect a content-only build into other data.
    foreach(_file IN LISTS UMI_MODULE_FILES)
        set(_probe "${UMI_MODULE_DESTINATION}/${_file}")
        if(IS_DIRECTORY "${_probe}")
            message(FATAL_ERROR "Learning output names a directory: ${_file}")
        endif()
        while(NOT _probe STREQUAL "")
            if(IS_SYMLINK "${_probe}")
                message(FATAL_ERROR "Linked learning output is not accepted: ${_file}")
            endif()
            get_filename_component(_parent "${_probe}" DIRECTORY)
            if(_parent STREQUAL _probe)
                break()
            endif()
            set(_probe "${_parent}")
        endwhile()
    endforeach()
    foreach(_file IN LISTS UMI_MODULE_FILES)
        get_filename_component(_parent "${UMI_MODULE_DESTINATION}/${_file}" DIRECTORY)
        file(MAKE_DIRECTORY "${_parent}")
        file(COPY_FILE "${UMI_MODULE_SOURCE}/${_file}" "${UMI_MODULE_DESTINATION}/${_file}" ONLY_IF_DIFFERENT)
    endforeach()
endif()
