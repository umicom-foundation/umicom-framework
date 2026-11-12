#-----------------------------------------------------------------------------
# Umicom Framework
# File: docs/site/check_guide_branding.cmake
#
# PURPOSE:
#   Check standalone guide admission and publication without rendering or deployment.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
include("${FRAMEWORK}/docs/site/GuideBranding.cmake")
file(READ "${FRAMEWORK}/docs/GUI_RECORDING.html" _input)
file(SHA256 "${FRAMEWORK}/resources/brand/umicom.ico" _ico_sha)
file(SHA256 "${FRAMEWORK}/resources/brand/umicom-logo-on-dark.svg" _logo_sha)
set(_prefix "../assets/brand/")
set(_expected "ok")
set(_logo_file "umicom-logo-on-dark.svg")
if(CASE STREQUAL "light")
    file(READ "${FRAMEWORK}/docs/learning/index.html" _input)
    set(_logo_file "umicom-logo.svg")
    file(SHA256 "${FRAMEWORK}/resources/brand/${_logo_file}" _logo_sha)
elseif(CASE STREQUAL "invalid-variant")
    set(_logo_file "../unapproved.svg")
    set(_expected "invalid-logo-variant")
elseif(CASE STREQUAL "valid")
    # Use the real standalone guide and the current source-asset identities.
elseif(CASE STREQUAL "nested")
    set(_prefix "../../assets/brand/")
elseif(CASE STREQUAL "missing-logo")
    string(REPLACE "data-umicom-guide-logo=" "data-unrelated-logo=" _input "${_input}")
    set(_expected "missing-or-duplicate-marker")
elseif(CASE STREQUAL "missing-icon")
    string(REPLACE "data-umicom-guide-ico=" "data-unrelated-icon=" _input "${_input}")
    set(_expected "missing-or-duplicate-marker")
elseif(CASE STREQUAL "missing-fonts")
    string(REPLACE "Cascadia Code" "Unrelated monospace" _input "${_input}")
    set(_expected "missing-font-role")
elseif(CASE STREQUAL "duplicate-logo")
    string(APPEND _input "<div data-umicom-guide-logo=\"duplicate\"></div>")
    set(_expected "missing-or-duplicate-marker")
elseif(CASE STREQUAL "wrong-identity")
    set(_logo_sha "unrelated")
    set(_expected "asset-identity-or-format")
elseif(CASE STREQUAL "external-logo")
    string(REPLACE "src=\"data:image/svg+xml;base64," "src=\"https://example.invalid/" _input "${_input}")
    set(_expected "asset-identity-or-format")
elseif(CASE STREQUAL "invalid-prefix")
    set(_prefix "https://example.invalid/")
    set(_expected "invalid-asset-prefix")
elseif(CASE STREQUAL "retain-content")
    set(_example [=[<pre><code>${do_not_expand}; "literal" &amp; &lt;source&gt;</code></pre>]=])
    string(APPEND _input "${_example}")
else()
    message(FATAL_ERROR "Unknown guide-branding case: ${CASE}")
endif()
string(SHA256 _before "${_input}")
set(_output "previous")
if(CASE STREQUAL "light" OR CASE STREQUAL "invalid-variant")
    UmiSiteGuideBranding("${_input}" "${_prefix}" "${_ico_sha}" "${_logo_sha}" _output _status "${_logo_file}")
else()
    UmiSiteGuideBranding("${_input}" "${_prefix}" "${_ico_sha}" "${_logo_sha}" _output _status)
endif()
string(SHA256 _after "${_input}")
if(NOT _before STREQUAL _after OR NOT _status STREQUAL _expected)
    message(FATAL_ERROR "Guide input changed or status was ${_status}, expected ${_expected}")
endif()
if(_status STREQUAL "ok")
    foreach(_pair IN ITEMS "data-umicom-guide-ico=\"${_ico_sha}\" href=\"${_prefix}umicom.ico\""
            "data-umicom-guide-logo=\"${_logo_sha}\" src=\"${_prefix}${_logo_file}\"")
        string(FIND "${_output}" "${_pair}" _found)
        if(_found LESS 0)
            message(FATAL_ERROR "Published guide asset route is missing.")
        endif()
    endforeach()
    if(CASE STREQUAL "retain-content")
        string(FIND "${_output}" "${_example}" _found)
        if(_found LESS 0)
            message(FATAL_ERROR "Unrelated lesson content changed.")
        endif()
    endif()
elseif(NOT _output STREQUAL "previous")
    message(FATAL_ERROR "A refused guide replaced caller output.")
endif()
