# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
target_sources(umicom_document PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/document/recovery_schedule.c")

if(BUILD_TESTING)
    add_executable(umicom-recovery-schedule-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_recovery_schedule.c")
    target_link_libraries(umicom-recovery-schedule-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-recovery-schedule-test)
    umicom_apply_sanitizers(umicom-recovery-schedule-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-recovery-schedule-test)
    endif()
    foreach(case disabled interval due acknowledged unchanged newer-text newer-store clean fairness failed-write resume close-pending disable-pending stale-ticket forged-ticket other-owner backward-clock overflow-time invalid-interval invalid-flag duplicate zero-id zero-revision invalid-dirty capacity null-observe empty-set removed-before-begin reorder pending-busy null-output suspend suspend-busy suspend-ok)
        add_test(NAME framework.document.recovery_schedule.${case} COMMAND umicom-recovery-schedule-test ${case})
        set_tests_properties(framework.document.recovery_schedule.${case} PROPERTIES TIMEOUT 30 LABELS "framework;document;recovery;scheduling;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryBudgetChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryObservationChecks.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecoveryMonitorChecks.cmake")
