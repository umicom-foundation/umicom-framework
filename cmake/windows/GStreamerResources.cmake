#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/GStreamerResources.cmake
#
# PURPOSE:
#   Select dynamically loaded media plugins and their scanner from one runtime.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")

# PE imports cannot discover elements selected later by a GStreamer pipeline.
# Keep the installed plugin set together: one plugin may select another by
# factory name without importing its DLL. StageRuntime owns PE validation,
# dependency scanning, copying and hashes; this helper only selects the inputs.
function(umicom_windows_gstreamer_resources prefix out_plugins out_scanners)
    set(${out_plugins} "" PARENT_SCOPE)
    set(${out_scanners} "" PARENT_SCOPE)
    umicom_windows_require_literal("${prefix}")
    if(NOT EXISTS "${prefix}/bin/libgstreamer-1.0-0.dll")
        return()
    endif()
    set(_plugin_root "${prefix}/lib/gstreamer-1.0")
    if(EXISTS "${_plugin_root}" AND NOT IS_DIRECTORY "${_plugin_root}")
        message(FATAL_ERROR "GStreamer plugin root must be a directory: ${_plugin_root}")
    endif()
    if(IS_SYMLINK "${prefix}/lib" OR IS_SYMLINK "${_plugin_root}")
        message(FATAL_ERROR "GStreamer plugin directories must belong to the selected runtime prefix")
    endif()
    file(GLOB _plugins LIST_DIRECTORIES FALSE "${_plugin_root}/*.dll")
    list(SORT _plugins)
    if(NOT _plugins)
        # A deliberately media-free runtime remains supported. The native
        # playback capability check reports unavailable rather than a pass.
        return()
    endif()
    set(_scanner "${prefix}/libexec/gstreamer-1.0/gst-plugin-scanner.exe")
    if(IS_SYMLINK "${prefix}/libexec" OR IS_SYMLINK "${prefix}/libexec/gstreamer-1.0")
        message(FATAL_ERROR "GStreamer scanner directories must belong to the selected runtime prefix")
    endif()
    if(NOT EXISTS "${_scanner}" OR IS_DIRECTORY "${_scanner}")
        message(FATAL_ERROR
            "GStreamer plugins require libexec/gstreamer-1.0/gst-plugin-scanner.exe "
            "from the same runtime prefix. Repair that package before deployment.")
    endif()
    # Refuse indirection before the staging graph is constructed. This also
    # catches a single plugin symlink even when its parent is an ordinary folder.
    foreach(_file IN LISTS _plugins _scanner)
        if(IS_SYMLINK "${_file}" OR IS_DIRECTORY "${_file}")
            message(FATAL_ERROR "GStreamer runtime payload must be a regular file: ${_file}")
        endif()
        file(RELATIVE_PATH _relative "${prefix}" "${_file}")
        umicom_windows_require_relative("${_relative}")
    endforeach()
    set(${out_plugins} "${_plugins}" PARENT_SCOPE)
    set(${out_scanners} "${_scanner}" PARENT_SCOPE)
endfunction()
