#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCreativeEditedPngCases.cmake
#
# PURPOSE:
#   Run the same edited PNG export fixtures in focused and full builds.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_register_creative_edited_png_cases target)
    foreach(case IN ITEMS branded plain too-small existing cancel-before-export invalid-crop)
        set(test "framework.creative_assets.edited_png.${case}")
        add_test(NAME "${test}" COMMAND "${target}" "${case}")
        set_tests_properties("${test}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;creative-assets;image-edit;branding;native;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${test}" TARGETS "${target}")
        endif()
    endforeach()
endfunction()
