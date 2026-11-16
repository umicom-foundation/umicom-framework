# Umicom Framework
# File: cmake/UmicomComfyReferenceInspection.cmake
#
# PURPOSE:
#   Register shared paged reference output and native regression cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
set(_reference_inspection_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_comfy_protocol PRIVATE "${_reference_inspection_root}/src/media_generation/comfy_reference_command.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-reference-inspection-test "${_reference_inspection_root}/tests/media_generation/test_comfy_reference_inspection.c")
    target_link_libraries(umicom-comfy-reference-inspection-test PRIVATE Umicom::comfy_protocol Umicom::protocol)
    set_target_properties(umicom-comfy-reference-inspection-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-comfy-reference-inspection-test)
    umicom_apply_sanitizers(umicom-comfy-reference-inspection-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-comfy-reference-inspection-test)
    endif()
    foreach(_case summary first-page second-page last-page end-page beyond-page zero-limit large-limit exact-capacity short-capacity zero-capacity nulls escaping max-integer unknown-slot unknown-output empty private-values digests repeat all-pages maximum-page)
        add_test(NAME framework.media.comfy.reference-json.${_case} COMMAND umicom-comfy-reference-inspection-test ${_case})
        set_tests_properties(framework.media.comfy.reference-json.${_case} PROPERTIES TIMEOUT 40 LABELS "framework;comfy;offline;json")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.reference-json.${_case} TARGETS umicom-comfy-reference-inspection-test)
        endif()
    endforeach()
endif()
