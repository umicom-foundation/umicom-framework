#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDoxygenBranding.cmake
#
# PURPOSE:
#   Give both generated code-reference targets the approved wordmark and favicon.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)

function(umicom_doxygen_branding brand_root generator_version)
    # The reference title area uses the shared dark surface.
    set(_wordmark "${brand_root}/umicom-logo-on-dark.svg")
    set(_favicon "${brand_root}/umicom.ico")
    foreach(_asset IN ITEMS "${_wordmark}" "${_favicon}")
        if(NOT EXISTS "${_asset}" OR IS_DIRECTORY "${_asset}")
            message(FATAL_ERROR "Required Umicom documentation artwork is missing: ${_asset}")
        endif()
    endforeach()

    # Doxygen 1.14 introduced PROJECT_ICON. Earlier versions can still
    # generate the wordmark and continue using their original HTML layout.
    set(_icon_line "")
    if(generator_version AND NOT generator_version VERSION_LESS "1.14")
        set(_icon_line "PROJECT_ICON           = \"${_favicon}\"")
    endif()
    set(UMICOM_DOCUMENTATION_PROJECT_LOGO "${_wordmark}" PARENT_SCOPE)
    set(UMICOM_DOCUMENTATION_PROJECT_ICON "${_favicon}" PARENT_SCOPE)
    set(UMICOM_DOCUMENTATION_PROJECT_ICON_LINE "${_icon_line}" PARENT_SCOPE)
endfunction()
