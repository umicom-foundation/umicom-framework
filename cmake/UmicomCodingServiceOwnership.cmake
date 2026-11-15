# Umicom Framework
# File: cmake/UmicomCodingServiceOwnership.cmake
#
# PURPOSE:
# Register service owner, revision and callback lifetime boundary checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-coding-owner-admission-test tests/test_ai_coding_service_ownership.c)
    target_link_libraries(umicom-ai-coding-owner-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-coding-owner-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-owner-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-owner-admission-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-coding-owner-admission-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/coding_assistant.c")
    endif()
    set(_owner_cases zero-revision task-count patch-count task-text task-revision task-ahead task-file-count patch-text patch-duplicate policy read-final query-bound adapter-invalid destroy-null destroy-checked)
    set(_max_cases policy prepare record approve reject apply revert)
    set(_last_cases policy prepare record approve reject apply revert)
    foreach(_group IN ITEMS owner max last)
        foreach(_case IN LISTS _${_group}_cases)
            set(_name "framework.ai.coding-owner.${_group}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-owner-admission-test "${_group}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-owner")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-owner-admission-test)
        endforeach()
    endforeach()
    foreach(_direction IN ITEMS apply revert)
        foreach(_phase IN ITEMS read write compensation)
            foreach(_hook IN ITEMS policy prepare record approve reject apply revert apply-report revert-report destroy legacy-destroy reads adapter-copy)
                set(_name "framework.ai.coding-owner.reentry.${_direction}.${_phase}.${_hook}")
                add_test(NAME "${_name}" COMMAND umicom-ai-coding-owner-admission-test reentry "${_hook}" "${_phase}" "${_direction}")
                set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-owner;callback")
                umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-owner-admission-test)
            endforeach()
        endforeach()
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom-ai-coding-owner-admission-test)
    foreach(_direction IN ITEMS apply revert)
        foreach(_case IN ITEMS success failure compensation readback conflict null-report missing adapter busy exhausted patch-exhausted)
            set(_name "framework.ai.coding-owner.report.${_direction}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-owner-admission-test report "${_case}" "${_direction}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-owner;recovery")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-owner-admission-test)
        endforeach()
    endforeach()
endif()
