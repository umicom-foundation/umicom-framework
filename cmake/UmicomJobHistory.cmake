# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Deterministic storage and worker fixtures never launch a real compiler.
include_guard(GLOBAL)
if(BUILD_TESTING)
    foreach(kind codec store sqlite build build_sqlite)
        set(target "umicom-job-history-${kind}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/job_history/test_${kind}.c")
        target_link_libraries(${target} PRIVATE Umicom::build)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    set(codec_cases roundtrip bounded format overflow negative truncated hex nul control identifier
        success-incomplete failure-ok cancel-status zero-identity zero-revision invalid-state trailing)
    set(store_cases invalid outer-transaction capacity prune namespace missing-metadata corrupt duplicate
        identity-overflow revision-overflow premature-success cancel failure stale backward lifecycle)
    set(sqlite_cases reopen unfinished rollback commit-failure busy connections)
    set(build_cases success failure cancel destroy full detached storage-failure)
    set(build_sqlite_cases before-start between-phases final-outcome)
    foreach(kind codec store sqlite build build_sqlite)
        foreach(case IN LISTS ${kind}_cases)
            add_test(NAME framework.job_history.${kind}.${case} COMMAND umicom-job-history-${kind}-test "${case}")
            set_tests_properties(framework.job_history.${kind}.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;job-history;persistence;regression")
        endforeach()
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/persistent-job-history.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Keep the example available to the learning preset without requiring SQLite.
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-job-history-example "${CMAKE_CURRENT_LIST_DIR}/../examples/data/job_history.c")
    target_link_libraries(umicom-job-history-example PRIVATE Umicom::data)
    umicom_apply_warnings(umicom-job-history-example)
    umicom_apply_sanitizers(umicom-job-history-example)
endif()
