#-----------------------------------------------------------------------------
# Umicom Framework
# File: docs/site/GuideBranding.cmake
#
# PURPOSE:
#   Admit standalone guide branding before publishing shared canonical asset routes.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Standalone guides carry canonical artwork inline so they open independently.
# Published guides use the shared copied assets instead. Only these two owned
# attributes are rebased; lesson content and layout remain source-owned.
function(UmiSiteGuideBranding text prefix ico_sha logo_sha output result)
    # The six-argument interface retains the dark-surface default. A caller may
    # select the canonical light-surface filename as the optional seventh argument.
    set(_logo_file "umicom-logo-on-dark.svg")
    if(ARGC GREATER 6)
        set(_logo_file "${ARGV6}")
    endif()
    if(ARGC GREATER 7 OR (NOT _logo_file STREQUAL "umicom-logo-on-dark.svg" AND
            NOT _logo_file STREQUAL "umicom-logo.svg"))
        set(${result} "invalid-logo-variant" PARENT_SCOPE)
        return()
    endif()
    foreach(_marker IN ITEMS "name=\"umicom-guide-branding\"" "id=\"umicom-guide-fonts\""
            "data-umicom-guide-ico=" "data-umicom-guide-logo=")
        string(REGEX MATCHALL "${_marker}" _matches "${text}")
        list(LENGTH _matches _count)
        if(NOT _count EQUAL 1)
            set(${result} "missing-or-duplicate-marker" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    string(REGEX MATCH "<style id=\"umicom-guide-fonts\">[^<]*</style>" _fonts "${text}")
    foreach(_font IN ITEMS "Segoe UI Variable Text" "Segoe UI" "Cascadia Code" "Consolas")
        string(FIND "${_fonts}" "${_font}" _found)
        if(_found LESS 0)
            set(${result} "missing-font-role" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    string(REGEX MATCH "data-umicom-guide-ico=\"[a-f0-9]+\" href=\"data:image/x-icon;base64,[A-Za-z0-9+/=]+\"" _icon "${text}")
    string(REGEX MATCH "data-umicom-guide-logo=\"[a-f0-9]+\" src=\"data:image/svg[+]xml;base64,[A-Za-z0-9+/=]+\"" _logo "${text}")
    string(FIND "${_icon}" "data-umicom-guide-ico=\"${ico_sha}\"" _icon_identity)
    string(FIND "${_logo}" "data-umicom-guide-logo=\"${logo_sha}\"" _logo_identity)
    if(NOT _icon_identity EQUAL 0 OR NOT _logo_identity EQUAL 0)
        set(${result} "asset-identity-or-format" PARENT_SCOPE)
        return()
    endif()
    if(NOT prefix MATCHES "^[A-Za-z0-9_./-]+/$")
        set(${result} "invalid-asset-prefix" PARENT_SCOPE)
        return()
    endif()
    string(REPLACE "${_icon}"
        "data-umicom-guide-ico=\"${ico_sha}\" href=\"${prefix}umicom.ico\"" _candidate "${text}")
    string(REPLACE "${_logo}"
        "data-umicom-guide-logo=\"${logo_sha}\" src=\"${prefix}${_logo_file}\"" _candidate "${_candidate}")
    set(${output} "${_candidate}" PARENT_SCOPE)
    set(${result} "ok" PARENT_SCOPE)
endfunction()
