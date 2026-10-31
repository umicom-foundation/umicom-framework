#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomBrandResourceTests.cmake
#
# PURPOSE:
#   Qualify shared resource dependency identity and automatic re-embedding.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    foreach(_brand_order IN ITEMS gtk-first media-first)
        add_test(NAME framework.build_configuration.brand-resources.${_brand_order}
            COMMAND "${CMAKE_COMMAND}"
                "-DROOT=${CMAKE_CURRENT_SOURCE_DIR}"
                "-DDEST=${CMAKE_CURRENT_BINARY_DIR}/brand-resources-${_brand_order}"
                "-DCOMPILER=${CMAKE_C_COMPILER}"
                "-DORDER=${_brand_order}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/build_configuration/check_brand_resources.cmake")
        set_tests_properties(framework.build_configuration.brand-resources.${_brand_order}
            PROPERTIES TIMEOUT 180
            LABELS "framework;branding;configuration;regression")
    endforeach()
endif()
