#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLanguageFramingChecks.cmake
#
# PURPOSE:
#   Register byte-boundary and coalesced-frame regression checks.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-language-framing-boundaries
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_framing_boundaries.c")
    target_link_libraries(umicom-language-framing-boundaries PRIVATE Umicom::developer)
    set_target_properties(umicom-language-framing-boundaries PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-language-framing-boundaries)
    umicom_apply_sanitizers(umicom-language-framing-boundaries)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-language-framing-boundaries)
    endif()
    foreach(case arguments huge-feed full-buffer invalid-state fragmented coalesced
        valid-before-invalid valid-before-partial output-capacity incomplete-output
        encode-capacity encode-bounded encode-in-place utf8-bytes maximum-body
        zero-body ascii-case charset-utf8 charset-alias charset-quoted raw-nul
        duplicate-length conflicting-length signed-positive signed-negative
        overflow-length oversize-length empty-length length-junk missing-length
        bad-name missing-colon non-ascii-header control-header charset-other
        charset-duplicate charset-empty duplicate-type)
        add_test(NAME "framework.language_runtime.framing.${case}"
            COMMAND umicom-language-framing-boundaries "${case}")
        set_tests_properties("framework.language_runtime.framing.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;language;framing;regression")
    endforeach()
endif()
