# Umicom Framework
# File: cmake/UmicomCompilePolicy.cmake
#
# PURPOSE:
#   Share consumer warning and sanitizer helpers across source and installed Framework builds.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
option(UMICOM_ENABLE_STRICT_WARNINGS "Enable strict warnings for Umicom consumer targets" ON)
if(NOT COMMAND umicom_apply_warnings)
    # Define the apply warnings build helper so parent and application projects apply one
    # consistent rule.
    function(umicom_apply_warnings target)
        # Apply this branch only when its contract condition is satisfied.
        if(NOT UMICOM_ENABLE_STRICT_WARNINGS)
            return()
        endif()

        # Select the warning options understood by the active compiler.
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4)
        # Use this fallback path when the earlier condition does not apply.
        else()
            target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wshadow
            )
        endif()
    endfunction()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomSanitizers.cmake")
