# Umicom Framework
# File: cmake/UmicomCodingPatchTransactions.cmake
#
# PURPOSE:
# Register explicit patch transaction and callback ownership regression cases.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(BUILD_TESTING AND TARGET Umicom::ai)
    add_executable(umicom-ai-coding-patch-transactions-test tests/test_ai_coding_patch_transactions.c)
    target_link_libraries(umicom-ai-coding-patch-transactions-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-coding-patch-transactions-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-coding-patch-transactions-test)
    umicom_apply_sanitizers(umicom-ai-coding-patch-transactions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-coding-patch-transactions-test)
    endif()
    foreach(_direction IN ITEMS apply revert)
        foreach(_case IN ITEMS create-success create-restored create-compensation create-readback create-conflict modify-success modify-restored modify-compensation modify-readback modify-conflict delete-success delete-restored delete-compensation delete-readback delete-conflict)
            set(_name "framework.ai.coding-patch-transactions.transaction.${_direction}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-patch-transactions-test transaction "${_direction}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;patch;transactions")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-patch-transactions-test)
        endforeach()
        foreach(_case IN ITEMS init add approve reject apply revert apply-report revert-report nested adapter write-add compensation-init)
            set(_name "framework.ai.coding-patch-transactions.guard.${_direction}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-patch-transactions-test guard "${_direction}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;patch;transactions")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-patch-transactions-test)
        endforeach()
        foreach(_case IN ITEMS null-report adapter exhausted)
            set(_name "framework.ai.coding-patch-transactions.admission.${_direction}.${_case}")
            add_test(NAME "${_name}" COMMAND umicom-ai-coding-patch-transactions-test admission "${_direction}" "${_case}")
            set_tests_properties("${_name}" PROPERTIES TIMEOUT 45 LABELS "framework;ai;patch;transactions")
            umicom_test_impact_owners("${_name}" TARGETS umicom-ai-coding-patch-transactions-test)
        endforeach()
    endforeach()
endif()
