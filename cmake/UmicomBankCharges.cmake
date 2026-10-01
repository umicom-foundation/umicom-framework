# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Charges use the same candidate, ledger, review, repository and GTK owner.
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(kind IN ITEMS workflow storage contract)
        add_executable(umicom-bank-charge-${kind}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/bank_charges/test_${kind}.c")
        target_link_libraries(umicom-bank-charge-${kind}-test PRIVATE Umicom::bank_operations)
        umicom_bank_review_target(umicom-bank-charge-${kind}-test)
    endforeach()
    foreach(case IN ITEMS lifecycle idempotency authority fields money duplicate-reference cancel-reject holds
            blocked-account blocked-customer reverse-overflow close-pending close-approved capacity
            review review-post stale-review review-ownership)
        add_test(NAME framework.bank_charges.workflow.${case} COMMAND umicom-bank-charge-workflow-test ${case})
        set_tests_properties(framework.bank_charges.workflow.${case} PROPERTIES TIMEOUT 45 LABELS "bank;charges;regression")
    endforeach()
    foreach(case IN ITEMS restart reverse-restart write-abort write-rollback stale-writer impossible-replay)
        add_test(NAME framework.bank_charges.storage.${case} COMMAND umicom-bank-charge-storage-test ${case}
            "${CMAKE_CURRENT_BINARY_DIR}/bank-charge-${case}.sqlite")
        set_tests_properties(framework.bank_charges.storage.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "bank;charges;sqlite;regression")
    endforeach()
    foreach(case IN ITEMS codec query statement)
        add_test(NAME framework.bank_charges.contract.${case} COMMAND umicom-bank-charge-contract-test ${case})
        set_tests_properties(framework.bank_charges.contract.${case} PROPERTIES TIMEOUT 30 LABELS "bank;charges;regression")
    endforeach()
    if(TARGET umicom-bank-interest-native-test)
        foreach(case IN ITEMS form edit invalid close post reverse statement)
            add_test(NAME framework.bank_charges.native.${case} COMMAND umicom-bank-interest-native-test charge-${case})
            set_tests_properties(framework.bank_charges.native.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "bank;charges;gtk4;regression")
        endforeach()
    endif()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/practice-charges.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
