#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/StageRuntime.cmake
#
# PURPOSE:
#   Inspect real PE imports, deploy a coherent runtime and record only the files
#   owned by that runtime. Unresolved imports are build failures, not warnings.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

# Runtime staging runs in its own cmake -P process, so policies selected by the
# application configure step do not carry into this script. CMake 4.3 added
# normalised dependency paths; opt in here rather than hiding policy warnings.
# The guard preserves the supported CMake 3.24 baseline and older build hosts.
if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")
if(NOT UMI_DEPLOY_CONFIG OR NOT EXISTS "${UMI_DEPLOY_CONFIG}")
    message(FATAL_ERROR "A generated UMI_DEPLOY_CONFIG is required")
endif()
include("${UMI_DEPLOY_CONFIG}")
if(NOT CMAKE_HOST_WIN32)
    message(FATAL_ERROR "PE runtime staging must run on the Windows build machine")
endif()
umicom_windows_require_pe("${UMI_DEPLOY_EXECUTABLE}")
foreach(_required UMI_DEPLOY_TARGET UMI_DEPLOY_PREFIX UMI_DEPLOY_WORK UMI_DEPLOY_OBJDUMP)
    if(NOT ${_required})
        message(FATAL_ERROR "Incomplete deployment configuration: ${_required}")
    endif()
endforeach()
get_filename_component(_binary_dir "${UMI_DEPLOY_EXECUTABLE}" DIRECTORY)
get_filename_component(_binary_name "${_binary_dir}" NAME)
string(TOLOWER "${_binary_name}" _binary_name)
if(_binary_name STREQUAL "bin")
    get_filename_component(_runtime_root "${_binary_dir}" DIRECTORY)
else()
    set(_runtime_root "${_binary_dir}")
endif()
file(MAKE_DIRECTORY "${_runtime_root}" "${UMI_DEPLOY_WORK}")
# Products usually share build/bin. Serialise writes without introducing false
# build dependencies between unrelated application executables.
file(LOCK "${_runtime_root}/.umicom-runtime.lock" GUARD PROCESS TIMEOUT 300)
set(_records "")

macro(umicom_stage_owned_file source deployed relative)
    umicom_windows_require_relative("${relative}")
    umicom_windows_copy_file("${source}" "${deployed}")
    list(APPEND _records "${relative}|${deployed}")
endmacro()

# Source resources are the only trees enumerated. We never package build/lib,
# which contains SDK archives and tests unrelated to the application runtime.
macro(umicom_stage_owned_tree source relative)
    if(EXISTS "${source}")
        if(IS_SYMLINK "${source}")
            message(FATAL_ERROR "Runtime resource root must not be a symlink: ${source}")
        endif()
        file(GLOB_RECURSE _tree_files LIST_DIRECTORIES FALSE RELATIVE "${source}" "${source}/*")
        list(SORT _tree_files)
        foreach(_tree_file IN LISTS _tree_files)
            umicom_stage_owned_file("${source}/${_tree_file}"
                "${_runtime_root}/${relative}/${_tree_file}" "${relative}/${_tree_file}")
        endforeach()
    endif()
endmacro()

# Hide inherited toolchain prefixes from helper processes. GTK utilities can
# still resolve their own runtime and Windows system DLLs, without mingw64 and
# ucrt64 taking turns according to the developer's shell PATH ordering.
file(TO_CMAKE_PATH "$ENV{SystemRoot}" _system_root)
if(NOT _system_root)
    message(FATAL_ERROR "The Windows SystemRoot environment variable is missing")
endif()
set(ENV{PATH} "${UMI_DEPLOY_PREFIX}/bin;${_system_root}/System32;${_system_root}")
set(_modules "")
file(GLOB _pixbuf_modules
    "${UMI_DEPLOY_PREFIX}/lib/gdk-pixbuf-2.0/*/loaders/*.dll")
file(GLOB _gio_modules "${UMI_DEPLOY_PREFIX}/lib/gio/modules/*.dll")
file(GLOB _gtk_modules
    "${UMI_DEPLOY_PREFIX}/lib/gtk-4.0/*/media/*.dll"
    "${UMI_DEPLOY_PREFIX}/lib/gtk-4.0/*/printbackends/*.dll")
list(APPEND _modules ${_pixbuf_modules} ${_gio_modules} ${_gtk_modules})
foreach(_module IN LISTS _modules)
    umicom_windows_require_pe("${_module}")
    file(RELATIVE_PATH _relative "${UMI_DEPLOY_PREFIX}" "${_module}")
    umicom_stage_owned_file("${_module}" "${_runtime_root}/${_relative}" "${_relative}")
endforeach()
set(_extra_dirs "")
foreach(_module IN LISTS UMI_DEPLOY_EXTRA_MODULES)
    umicom_windows_require_pe("${_module}")
    get_filename_component(_name "${_module}" NAME)
    get_filename_component(_directory "${_module}" DIRECTORY)
    umicom_stage_owned_file("${_module}" "${_binary_dir}/${_name}" "bin/${_name}")
    list(APPEND _modules "${_module}")
    list(APPEND _extra_dirs "${_directory}")
endforeach()
list(REMOVE_DUPLICATES _modules)

# Inspect a private copy of the executable. A stale GLib DLL in build/bin must
# not outrank the configured runtime prefix just because it sits beside the
# original executable. Project-owned DLL directories remain explicit fallbacks.
set(_scan "${UMI_DEPLOY_WORK}/owned-scan-${UMI_DEPLOY_TARGET}")
umicom_windows_reset_owned_directory("${_scan}")
get_filename_component(_executable_name "${UMI_DEPLOY_EXECUTABLE}" NAME)
umicom_windows_copy_file("${UMI_DEPLOY_EXECUTABLE}" "${_scan}/${_executable_name}")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${UMI_DEPLOY_OBJDUMP}")
set(_module_arguments "")
if(_modules)
    list(APPEND _module_arguments LIBRARIES ${_modules})
endif()
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${_scan}/${_executable_name}"
    ${_module_arguments}
    DIRECTORIES "${UMI_DEPLOY_PREFIX}/bin" "${_binary_dir}"
        "${UMI_DEPLOY_BUILD_ROOT}/lib" ${_extra_dirs}
    PRE_EXCLUDE_REGEXES "^[Aa][Pp][Ii]-[Mm][Ss]-.*" "^[Ee][Xx][Tt]-[Mm][Ss]-.*"
    RESOLVED_DEPENDENCIES_VAR _dependencies
    UNRESOLVED_DEPENDENCIES_VAR _unresolved
    CONFLICTING_DEPENDENCIES_PREFIX _conflicts)
if(_unresolved)
    message(FATAL_ERROR
        "${UMI_DEPLOY_PRODUCT}: unresolved DLL imports: ${_unresolved}. "
        "Repair the selected runtime prefix; this package has not passed deployment.")
endif()
if(_conflicts_FILENAMES)
    message(FATAL_ERROR "Conflicting DLLs in the runtime dependency graph: ${_conflicts_FILENAMES}")
endif()
string(TOLOWER "${_system_root}/" _system_lower)
set(_allowed_roots "${UMI_DEPLOY_PREFIX}" "${_binary_dir}"
    "${UMI_DEPLOY_BUILD_ROOT}/lib" ${_extra_dirs})
foreach(_dependency IN LISTS _dependencies)
    file(TO_CMAKE_PATH "${_dependency}" _dependency)
    string(TOLOWER "${_dependency}" _dependency_lower)
    string(FIND "${_dependency_lower}" "${_system_lower}" _system_position)
    if(_system_position EQUAL 0)
        continue()
    endif()
    set(_allowed FALSE)
    foreach(_allowed_root IN LISTS _allowed_roots)
        file(TO_CMAKE_PATH "${_allowed_root}/" _allowed_root)
        string(TOLOWER "${_allowed_root}" _allowed_lower)
        string(FIND "${_dependency_lower}" "${_allowed_lower}" _position)
        if(_position EQUAL 0)
            set(_allowed TRUE)
            break()
        endif()
    endforeach()
    if(NOT _allowed)
        message(FATAL_ERROR "A DLL resolved outside the selected runtime/build roots: ${_dependency}")
    endif()
    umicom_windows_require_pe("${_dependency}")
    get_filename_component(_name "${_dependency}" NAME)
    # Refuse a same-named build-tree copy from a different runtime. It must not
    # silently shadow the coherent prefix selected for this entire package.
    if(EXISTS "${UMI_DEPLOY_PREFIX}/bin/${_name}")
        file(SHA256 "${UMI_DEPLOY_PREFIX}/bin/${_name}" _prefix_hash)
        file(SHA256 "${_dependency}" _dependency_hash)
        if(NOT _prefix_hash STREQUAL _dependency_hash)
            message(FATAL_ERROR "Mixed runtime detected for ${_name}; remove the stale build-tree copy")
        endif()
    endif()
    umicom_stage_owned_file("${_dependency}" "${_binary_dir}/${_name}" "bin/${_name}")
endforeach()

# GtkSourceView embeds many language files but loads their RelaxNG schema
# from disk. A DLL-complete Studio package is not therefore data-complete.
include("${CMAKE_CURRENT_LIST_DIR}/SourceViewResources.cmake")
umicom_windows_sourceview_resources("${UMI_DEPLOY_PREFIX}" "${_dependencies}"
    _sourceview_resources)
foreach(_resource IN LISTS _sourceview_resources)
    umicom_stage_owned_tree("${UMI_DEPLOY_PREFIX}/${_resource}" "${_resource}")
endforeach()

# GTK has runtime data and dynamically loaded modules in addition to PE import
# dependencies. Keep optional resource directories when the prefix supplies
# them, and compile schema caches from the copied XML on the build machine.
foreach(_resource IN ITEMS "share/glib-2.0/schemas" "share/icons" "share/themes"
        "share/fontconfig" "etc/fonts" "share/licenses")
    umicom_stage_owned_tree("${UMI_DEPLOY_PREFIX}/${_resource}" "${_resource}")
endforeach()
if(EXISTS "${UMI_DEPLOY_PREFIX}/share/glib-2.0/schemas")
    if(NOT EXISTS "${UMI_DEPLOY_PREFIX}/bin/glib-compile-schemas.exe")
        message(FATAL_ERROR "glib-compile-schemas.exe is required to prepare the schema cache")
    endif()
    file(MAKE_DIRECTORY "${_scan}/schemas")
    file(COPY "${UMI_DEPLOY_PREFIX}/share/glib-2.0/schemas/" DESTINATION "${_scan}/schemas")
    execute_process(COMMAND "${UMI_DEPLOY_PREFIX}/bin/glib-compile-schemas.exe"
        "${_scan}/schemas"
        RESULT_VARIABLE _result ERROR_VARIABLE _error)
    if(NOT _result STREQUAL "0")
        message(FATAL_ERROR "Schema cache generation failed: ${_error}")
    endif()
    umicom_windows_copy_file("${_scan}/schemas/gschemas.compiled"
        "${_runtime_root}/share/glib-2.0/schemas/gschemas.compiled")
    list(APPEND _records
        "share/glib-2.0/schemas/gschemas.compiled|${_runtime_root}/share/glib-2.0/schemas/gschemas.compiled")
endif()
if(_gio_modules)
    if(NOT EXISTS "${UMI_DEPLOY_PREFIX}/bin/gio-querymodules.exe")
        message(FATAL_ERROR "gio-querymodules.exe is required for the supplied GIO modules")
    endif()
    file(MAKE_DIRECTORY "${_scan}/gio")
    foreach(_module IN LISTS _gio_modules)
        file(COPY "${_module}" DESTINATION "${_scan}/gio")
    endforeach()
    execute_process(COMMAND "${UMI_DEPLOY_PREFIX}/bin/gio-querymodules.exe"
        "${_scan}/gio" RESULT_VARIABLE _result ERROR_VARIABLE _error)
    if(NOT _result STREQUAL "0" OR NOT EXISTS "${_scan}/gio/giomodule.cache")
        message(FATAL_ERROR "GIO module-cache generation failed: ${_error}")
    endif()
    umicom_windows_copy_file("${_scan}/gio/giomodule.cache"
        "${_runtime_root}/lib/gio/modules/giomodule.cache")
    list(APPEND _records "lib/gio/modules/giomodule.cache|${_runtime_root}/lib/gio/modules/giomodule.cache")
endif()
if(_pixbuf_modules)
    if(NOT EXISTS "${UMI_DEPLOY_PREFIX}/bin/gdk-pixbuf-query-loaders.exe")
        message(FATAL_ERROR "gdk-pixbuf-query-loaders.exe is required for the supplied image loaders")
    endif()
    execute_process(COMMAND "${UMI_DEPLOY_PREFIX}/bin/gdk-pixbuf-query-loaders.exe" ${_pixbuf_modules}
        RESULT_VARIABLE _result OUTPUT_VARIABLE _cache ERROR_VARIABLE _error
        ENCODING UTF-8)
    if(NOT _result STREQUAL "0" OR NOT _cache)
        message(FATAL_ERROR "Image-loader cache generation failed: ${_error}")
    endif()
    # Cache entries use quoted paths. Replace both forms emitted by supported
    # Windows builds; only the native C bootstrap inserts the installed path.
    file(TO_CMAKE_PATH "${UMI_DEPLOY_PREFIX}" _prefix_forward)
    string(REPLACE "/" "\\\\" _prefix_escaped "${_prefix_forward}")
    string(REPLACE "${_prefix_forward}" "@UMICOM_RUNTIME_ROOT@" _cache "${_cache}")
    string(REPLACE "${_prefix_escaped}" "@UMICOM_RUNTIME_ROOT@" _cache "${_cache}")
    # A cache that still names a drive path is not a relocatable package.
    if(_cache MATCHES "[\r\n]\"[A-Za-z]:")
        message(FATAL_ERROR "The generated image-loader cache still contains an absolute build-machine path")
    endif()
    file(MAKE_DIRECTORY "${_runtime_root}/share/umicom/runtime")
    file(WRITE "${_runtime_root}/share/umicom/runtime/gdk-pixbuf-loaders.cache.in" "${_cache}")
    list(APPEND _records
        "share/umicom/runtime/gdk-pixbuf-loaders.cache.in|${_runtime_root}/share/umicom/runtime/gdk-pixbuf-loaders.cache.in")
else()
    # This is one known generated file, not a recursive resource-tree deletion.
    file(REMOVE "${_runtime_root}/share/umicom/runtime/gdk-pixbuf-loaders.cache.in")
endif()
if(UMI_DEPLOY_FRAMEWORK_RESOURCES)
    umicom_stage_owned_tree("${UMI_DEPLOY_FRAMEWORK_RESOURCES}" "share/umicom/framework/resources")
    foreach(_brand IN ITEMS umicom-icon.svg umicom-icon-on-dark.svg umicom-logo.svg umicom-logo-on-dark.svg)
        umicom_stage_owned_file("${UMI_DEPLOY_FRAMEWORK_RESOURCES}/brand/${_brand}"
            "${_binary_dir}/branding/${_brand}" "bin/branding/${_brand}")
    endforeach()
endif()
file(MAKE_DIRECTORY "${_runtime_root}/share/umicom/runtime")
file(WRITE "${_runtime_root}/share/umicom/runtime/deployment.marker" "UMICOM_WINDOWS_RUNTIME=1\n")
list(APPEND _records "share/umicom/runtime/deployment.marker|${_runtime_root}/share/umicom/runtime/deployment.marker")
list(REMOVE_DUPLICATES _records)
list(SORT _records)
# Snapshot bytes now. A later application staging into the same build/bin must
# not make an earlier report silently describe different DLL contents.
set(_verified_records "")
foreach(_record IN LISTS _records)
    string(REPLACE "|" ";" _fields "${_record}")
    list(GET _fields 1 _source)
    file(SHA256 "${_source}" _sha)
    list(APPEND _verified_records "${_record}|${_sha}")
endforeach()
file(SHA256 "${UMI_DEPLOY_EXECUTABLE}" _executable_sha)
file(WRITE "${UMI_DEPLOY_WORK}/${UMI_DEPLOY_TARGET}.runtime.cmake"
    "set(UMI_REPORT_EXECUTABLE [==[${UMI_DEPLOY_EXECUTABLE}]==])\n"
    "set(UMI_REPORT_EXECUTABLE_SHA256 [==[${_executable_sha}]==])\n"
    "set(UMI_REPORT_TARGET [==[${UMI_DEPLOY_TARGET}]==])\n"
    "set(UMI_REPORT_PRODUCT [==[${UMI_DEPLOY_PRODUCT}]==])\n"
    "set(UMI_REPORT_FILES [==[${_verified_records}]==])\n")
message(STATUS "Click-ready runtime prepared: ${UMI_DEPLOY_PRODUCT}")
