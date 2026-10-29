#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomGtkRuntimeDiagnostics.cmake
#
# PURPOSE:
#   Report known toolkit runtime risks before native application qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Package metadata cannot prove which shared library a future process loads,
# nor whether a distributor backported a fix without changing its version.
# These identifiers are advisories, never a certificate of runtime safety.
function(umicom_gtk_runtime_advisories version target_system out_advisories)
    set(_advisories)
    if(NOT version MATCHES "^[0-9]+[.][0-9]+[.][0-9]+([.-][A-Za-z0-9.-]+)?$")
        list(APPEND _advisories gtk.runtime.version-unavailable)
    elseif(target_system STREQUAL "Windows" AND version MATCHES "^4[.]24[.](0|1)($|[.-])")
        # The 4.24.0 and 4.24.1 release sources lack disposal-time removal of
        # the Win32 IME callback. Vendor suffixes alone cannot establish a fix.
        list(APPEND _advisories gtk.win32.ime-disposal)
    endif()
    set(${out_advisories} "${_advisories}" PARENT_SCOPE)
endfunction()

function(umicom_report_gtk_runtime_advisories version target_system)
    umicom_gtk_runtime_advisories("${version}" "${target_system}" _advisories)
    if(NOT _advisories)
        return()
    endif()
    string(SHA256 _identity "${version};${target_system}")
    get_property(_reported GLOBAL PROPERTY "UMICOM_GTK_RUNTIME_ADVISORY_${_identity}")
    if(_reported)
        return()
    endif()
    set_property(GLOBAL PROPERTY "UMICOM_GTK_RUNTIME_ADVISORY_${_identity}" TRUE)
    foreach(_advisory IN LISTS _advisories)
        if(_advisory STREQUAL "gtk.win32.ime-disposal")
            message(WARNING
                "GTK ${version} on Windows needs Win32 input-method disposal review. "
                "The 4.24.0 and 4.24.1 release sources can leave an IME callback "
                "registered after its input context is destroyed. Verify that the "
                "vendor package contains GTK stable-branch fix "
                "30727f8a524dbaa69118a60d6bbef4b3447245b1, then rerun native window "
                "lifetime and TMS independent-host tests. A version-only upgrade "
                "to 4.24.1 does not establish this fix. Tests remain enabled. "
                "See https://github.com/GNOME/gtk/commit/30727f8a524dbaa69118a60d6bbef4b3447245b1")
        else()
            message(WARNING
                "GTK version metadata is unavailable or malformed. Record the "
                "loaded runtime version when qualifying native applications; "
                "configure metadata alone does not identify the loaded library.")
        endif()
    endforeach()
endfunction()
