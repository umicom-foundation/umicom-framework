#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/boot_media/check_command_routes.cmake
#
# PURPOSE:
#   Verify native umbrella routes retain standalone exit codes and diagnostics.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT EXISTS "${UMICOM}" OR NOT EXISTS "${STANDALONE}")
    message(FATAL_ERROR "Both native commands must be built before this check.")
endif()
if(NOT KIND STREQUAL "boot-media" AND NOT KIND STREQUAL "setup")
    message(FATAL_ERROR "Unsupported equivalence route")
endif()
foreach(_case IN ITEMS help self-test missing-value unknown)
    if(_case STREQUAL "help")
        set(_arguments --help)
        set(_expected 0)
    elseif(_case STREQUAL "self-test")
        set(_arguments --self-test)
        set(_expected 0)
    elseif(_case STREQUAL "missing-value")
        if(KIND STREQUAL "boot-media")
            set(_arguments inspect --image)
        else()
            set(_arguments media-stage --kernel)
        endif()
        set(_expected 2)
    else()
        set(_arguments unknown-command)
        if(KIND STREQUAL "boot-media")
            set(_expected 2)
        else()
            set(_expected 1)
        endif()
    endif()
    execute_process(COMMAND "${STANDALONE}" ${_arguments}
        RESULT_VARIABLE _standalone_code OUTPUT_VARIABLE _standalone_out
        ERROR_VARIABLE _standalone_err TIMEOUT 10 ENCODING UTF-8)
    execute_process(COMMAND "${UMICOM}" "${KIND}" ${_arguments}
        RESULT_VARIABLE _umbrella_code OUTPUT_VARIABLE _umbrella_out
        ERROR_VARIABLE _umbrella_err TIMEOUT 10 ENCODING UTF-8)
    if(NOT "${_standalone_code}" STREQUAL "${_expected}" OR
       NOT "${_umbrella_code}" STREQUAL "${_expected}")
        message(FATAL_ERROR "${KIND}/${_case}: expected ${_expected}; standalone=${_standalone_code}, umbrella=${_umbrella_code}")
    endif()
    if(NOT "${_standalone_out}" STREQUAL "${_umbrella_out}" OR
       NOT "${_standalone_err}" STREQUAL "${_umbrella_err}")
        message(FATAL_ERROR "${KIND}/${_case}: the umbrella route changed service output")
    endif()
endforeach()
message(STATUS "Native ${KIND} route matches standalone admission and help behavior")
