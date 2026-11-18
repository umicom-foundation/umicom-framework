#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomImageBrandCases.cmake
#
# PURPOSE:
#   Keep full and focused watermark qualification on the same behavioral case inventory.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_register_image_brand_cases brand_target failure_target)
    if(TARGET ${brand_target})
        foreach(_case apply transparent light portrait minimum png existing cancel small
                      null-source null-output occupied-output relative-output null-receipt)
            add_test(NAME framework.media.image_brand.${_case} COMMAND ${brand_target} "${_case}")
            set_tests_properties(framework.media.image_brand.${_case} PROPERTIES TIMEOUT 30
                SKIP_RETURN_CODE 77 LABELS "framework;media;svg;png;branding;regression")
        endforeach()
    endif()
    if(TARGET ${failure_target})
        foreach(_case decode save-decode width height pixels cancel-after-decode)
            add_test(NAME framework.media.image_brand-failure.${_case}
                COMMAND ${failure_target} "${_case}")
            set_tests_properties(framework.media.image_brand-failure.${_case}
                PROPERTIES TIMEOUT 30 LABELS "framework;media;branding;fault-injection;regression")
        endforeach()
    endif()
endfunction()
