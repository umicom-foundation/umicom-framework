#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPixbufCacheTests.cmake
#
# PURPOSE:
#   Register image-loader cache contracts in full and focused qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_register_pixbuf_cache_tests framework name_prefix)
    foreach(_case forward escaped prefix-case relative crlf metadata foreign
            prefix-sibling traversal abi duplicate empty)
        add_test(NAME "${name_prefix}.pixbuf-cache.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK=${framework}" "-DCASE=${_case}"
                -P "${framework}/tests/windows_deployment/test_pixbuf_cache.cmake")
        set_tests_properties("${name_prefix}.pixbuf-cache.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;deployment;image-loader;regression")
    endforeach()
endfunction()
