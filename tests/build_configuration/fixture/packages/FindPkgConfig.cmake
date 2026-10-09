# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# This stand-in validates the production module's dependency request. It is
# scoped to this configure-only fixture and cannot replace real build packages.
set(PkgConfig_FOUND TRUE)
function(pkg_check_modules prefix)
    if(NOT "${prefix}" STREQUAL "GLIB" OR NOT "${ARGN}" STREQUAL "REQUIRED;IMPORTED_TARGET;glib-2.0")
        message(FATAL_ERROR "Unexpected GLib discovery contract")
    endif()
    if(TARGET PkgConfig::GLIB)
        message(FATAL_ERROR "GLib discovery was repeated")
    endif()
    add_library(PkgConfig::GLIB INTERFACE IMPORTED)
    set_property(GLOBAL PROPERTY FIXTURE_GLIB_DISCOVERIES 1)
endfunction()
