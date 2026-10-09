# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Register focused status readers without requiring the entire application test suite.
include_guard(GLOBAL)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/repository-working-tree-review.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(TARGET umicom)
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_repository_review.c")
    target_link_libraries(umicom PRIVATE Umicom::vcs)
endif()
if(BUILD_TESTING)
    set(parser_cases empty headers unknown-divergence unborn detached ordinary child child-commit rename copy conflict
        unknown-header path-boundary record-boundary missing-nul empty-record missing-source
        duplicate-path duplicate-header empty-header unknown-record bad-mode bad-object bad-child
        bad-code bad-score bad-conflict divergence-no-upstream overflow)
    set(process_cases split-stream launch-failure not-launched exit-failure timeout cancel
        empty-output truncated-record overflow stderr-contamination)
    set(review_cases summary child-only escaped-path child-advice small-buffer legacy legacy-overflow)
    # Preserve escaped rename text, immutable observations and atomic capacity refusal.
    list(APPEND review_cases renamed-path maximum-paths exact-buffer)
    set(job_cases created cancel-created wrong-owner success late-stop cancel-running read-failure)
    foreach(kind IN ITEMS parser process review job)
        set(target "umicom-vcs-working-tree-${kind}-test")
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/vcs_working_tree/test_${kind}.c")
        target_link_libraries(${target} PRIVATE Umicom::vcs)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ${kind}_cases)
            add_test(NAME framework.vcs.working_tree.${kind}.${case} COMMAND ${target} "${case}")
            set_tests_properties(framework.vcs.working_tree.${kind}.${case} PROPERTIES
                TIMEOUT 30 LABELS "framework;vcs;working-tree;regression")
        endforeach()
    endforeach()
    find_program(UMICOM_WORKING_TREE_GIT_EXECUTABLE git)
    if(UMICOM_WORKING_TREE_GIT_EXECUTABLE)
        add_executable(umicom-vcs-working-tree-native-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/vcs_working_tree/test_native.c")
        target_link_libraries(umicom-vcs-working-tree-native-test PRIVATE Umicom::vcs)
        umicom_apply_warnings(umicom-vcs-working-tree-native-test)
        umicom_apply_sanitizers(umicom-vcs-working-tree-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-vcs-working-tree-native-test)
        endif()
        set(_umi_working_tree_cli_argument "")
        if(TARGET umicom)
            set(_umi_working_tree_cli_argument "-DCLI_BINARY=$<TARGET_FILE:umicom>")
        endif()
        add_test(NAME framework.vcs.working_tree.native.lifecycle COMMAND "${CMAKE_COMMAND}"
            "-DTEST_BINARY=$<TARGET_FILE:umicom-vcs-working-tree-native-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}"
            ${_umi_working_tree_cli_argument}
            "-DGIT_EXECUTABLE=${UMICOM_WORKING_TREE_GIT_EXECUTABLE}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/vcs_working_tree/native_fixture.cmake")
        set_tests_properties(framework.vcs.working_tree.native.lifecycle PROPERTIES
            TIMEOUT 180 LABELS "framework;vcs;working-tree;native;submodule")
    endif()
    if(TARGET umicom)
        add_test(NAME framework.vcs.working_tree.cli.help COMMAND umicom repo review --help)
        set_tests_properties(framework.vcs.working_tree.cli.help PROPERTIES
            PASS_REGULAR_EXPRESSION "No fetch, stage, commit or push" TIMEOUT 15)
        foreach(case zero-timeout invalid-timeout unknown-option)
            if(case STREQUAL "zero-timeout")
                set(_review_args --timeout-ms 0)
            elseif(case STREQUAL "invalid-timeout")
                set(_review_args --timeout-ms abc)
            else()
                set(_review_args --unknown)
            endif()
            add_test(NAME framework.vcs.working_tree.cli.${case} COMMAND umicom repo review ${_review_args})
            set_tests_properties(framework.vcs.working_tree.cli.${case} PROPERTIES
                WILL_FAIL TRUE TIMEOUT 15 LABELS "framework;vcs;cli;validation")
        endforeach()
    endif()
    if(TARGET umicom_ui_gtk4)
        add_executable(umicom-vcs-working-tree-gtk4-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/vcs_working_tree/test_gtk4.c")
        target_link_libraries(umicom-vcs-working-tree-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::vcs)
        umicom_apply_warnings(umicom-vcs-working-tree-gtk4-test)
        umicom_apply_sanitizers(umicom-vcs-working-tree-gtk4-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-vcs-working-tree-gtk4-test)
        endif()
        foreach(case IN ITEMS render paging failed-refresh stop close-retained)
            add_test(NAME framework.vcs.working_tree.gtk4.${case}
                COMMAND umicom-vcs-working-tree-gtk4-test "${case}")
            set_tests_properties(framework.vcs.working_tree.gtk4.${case} PROPERTIES TIMEOUT 30
                SKIP_RETURN_CODE 77 LABELS "framework;vcs;working-tree;gtk4;lifetime")
        endforeach()
    endif()
endif()
