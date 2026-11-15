# Umicom Framework
# File: cmake/UmicomCodingPatchAdmission.cmake
#
# PURPOSE:
# Register patch integrity, mutation preservation and file-adapter boundary checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-coding-patch-admission-test tests/test_ai_coding_patch_admission.c)
    target_link_libraries(umicom-ai-coding-patch-admission-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-coding-patch-admission-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-patch-admission-test)
    umicom_apply_sanitizers(umicom-ai-coding-patch-admission-test)
    set(_integrity_cases valid
        empty-draft
        empty-policy
        size
        abi
        count
        count-size-max
        revision-zero
        revision-final
        state-low
        state-high
        approval-empty
        id-bound
        request-bound
        title-bound
        rationale-bound
        approver-bound
        path-bound
        unsafe-path
        operation-low
        operation-high
        before-length
        after-length
        before-bound
        after-bound
        early-terminator
        before-hash
        after-hash
        removed-lines
        added-lines
        changed-lines
        create-before
        delete-after
        duplicate
        later-path-bound
        policy-files-zero
        policy-files-excess
        policy-lines-zero
        policy-lines-exact
        policy-lines-insufficient
        policy-negative-flags
        null-inputs)
    set(_mutation_cases capacity
        max-add
        max-approve
        max-reject
        corrupt-add
        corrupt-approve
        corrupt-reject
        last-add
        last-approve
        last-reject
        approver-bound
        path-bound
        before-bound
        after-bound
        duplicate
        borrow-slot
        borrow-approver
        create-before
        delete-after)
    set(_adapter_cases apply-bound
        revert-bound
        apply-size-max
        revert-size-max
        apply-max
        revert-max
        apply-last
        revert-last
        apply-corrupt-count
        revert-corrupt-count
        apply-read-error
        revert-read-error
        apply-conflict
        revert-conflict
        apply-valid
        revert-valid
        apply-write-error
        revert-write-error
        apply-negative-approval)
    foreach(_group IN ITEMS integrity mutation adapter)
        foreach(_case IN LISTS _${_group}_cases)
            set(_name "framework.ai.coding-patch.${_group}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-patch-admission-test "${_group}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;coding-patch")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-patch-admission-test)
        endforeach()
    endforeach()
endif()
