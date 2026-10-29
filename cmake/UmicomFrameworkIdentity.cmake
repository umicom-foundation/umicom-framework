#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFrameworkIdentity.cmake
#
# PURPOSE:
#   Derive package and receipt identity from the public Framework version header.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_read_framework_identity header prefix)
    if(NOT EXISTS "${header}" OR IS_DIRECTORY "${header}" OR
       NOT prefix MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
        message(FATAL_ERROR "Framework identity requires a readable header and a variable prefix")
    endif()
    foreach(_field VERSION_MAJOR VERSION_MINOR VERSION_PATCH ABI_VERSION)
        file(STRINGS "${header}" _lines REGEX "^#[ \t]*define[ \t]+UMICOM_FRAMEWORK_${_field}([ \t]|$)")
        list(LENGTH _lines _count)
        if(NOT _count EQUAL 1 OR
           NOT _lines MATCHES "^#[ \t]*define[ \t]+UMICOM_FRAMEWORK_${_field}[ \t]+(0|[1-9][0-9]*)U?[ \t]*$")
            message(FATAL_ERROR "Framework identity needs one unsigned decimal ${_field} definition")
        endif()
        set(_value "${CMAKE_MATCH_1}")
        string(LENGTH "${_value}" _digits)
        if(_field STREQUAL "ABI_VERSION")
            if(_digits GREATER 10 OR _value STREQUAL "0")
                message(FATAL_ERROR "Framework identity ABI is outside its uint32 range")
            endif()
            if(_value GREATER 4294967295)
                message(FATAL_ERROR "Framework identity ABI is outside its uint32 range")
            endif()
        else()
            if(_digits GREATER 5)
                message(FATAL_ERROR "Framework identity version component exceeds uint16")
            endif()
            if(_value GREATER 65535)
                message(FATAL_ERROR "Framework identity version component exceeds uint16")
            endif()
        endif()
        set("_identity_${_field}" "${_value}")
    endforeach()
    set(_version "${_identity_VERSION_MAJOR}.${_identity_VERSION_MINOR}.${_identity_VERSION_PATCH}")
    file(STRINGS "${header}" _strings REGEX "^#[ \t]*define[ \t]+UMICOM_FRAMEWORK_VERSION_STRING([ \t]|$)")
    list(LENGTH _strings _count)
    if(NOT _count EQUAL 1 OR
       NOT _strings MATCHES "^#[ \t]*define[ \t]+UMICOM_FRAMEWORK_VERSION_STRING[ \t]+\"([^\"]+)\"[ \t]*$")
        message(FATAL_ERROR "Framework identity needs one literal version string")
    endif()
    if(NOT CMAKE_MATCH_1 STREQUAL _version)
        message(FATAL_ERROR "Framework identity version string disagrees with its numeric components")
    endif()
    set("${prefix}_VERSION" "${_version}" PARENT_SCOPE)
    set("${prefix}_ABI" "${_identity_ABI_VERSION}" PARENT_SCOPE)
endfunction()
