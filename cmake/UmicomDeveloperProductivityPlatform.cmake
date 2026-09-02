#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperProductivityPlatform.cmake
#
# PURPOSE:
#   Attach professional Source Control, diff/merge, Terminal, Problems/output
#   and navigation composition to the canonical developer library.
#
# ARCHITECTURE:
#   This integration is additive. Existing VCS/source-control, terminal,
#   diagnostics, project/workspace and developer-workbench implementations stay
#   intact. This layer composes those established services into the functional
#   IDE workflows consumed by thin products such as Umicom Studio.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "Developer Productivity Platform requires the canonical umicom_developer target")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/bookmarks.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/builtin_diagnostics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/builtin_source_control_operations.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/builtin_terminal_profiles.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parser_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/clang.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/ctest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/gcc.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/java.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/linker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/msvc.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/ninja.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/python.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/rust.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/typescript.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_parsers/zig.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_pipeline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diagnostic_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diff.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diff_hunk.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/diff_navigation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/directory_compare.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/location_link.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/merge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/navigation_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/navigation_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/outline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/output_channels.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/problem.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/problem_navigation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/problem_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/references.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/branch_checkout.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/branch_create.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/branch_delete.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/commit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/diff.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/discard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/fetch.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/pull.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/push.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/refresh.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/stage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/stage_all.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/source_control_operations/unstage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/symbols.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/cmd.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/git_bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/powershell.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/ucrt64_bash.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_profiles/wsl.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_split.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_tabs.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/terminal_workspace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/unified_diff.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer_productivity/workbench_bridge.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add developer productivity test build helper so parent and application
    # projects apply one consistent rule.
    function(umicom_add_developer_productivity_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()

        add_executable(
            "${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../${source}"
        )
        target_link_libraries("${target}" PRIVATE Umicom::Framework)

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()

        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_developer_productivity_test(
        umicom-developer-productivity-bookmarks-test
        framework.developer_productivity.bookmarks
        tests/developer_productivity/test_bookmarks.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-builtin-catalogues-test
        framework.developer_productivity.builtin-catalogues
        tests/developer_productivity/test_builtin_catalogues.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-clang-test
        framework.developer_productivity.diagnostic-parser-clang
        tests/developer_productivity/test_diagnostic_parser_clang.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-cmake-test
        framework.developer_productivity.diagnostic-parser-cmake
        tests/developer_productivity/test_diagnostic_parser_cmake.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-ctest-test
        framework.developer_productivity.diagnostic-parser-ctest
        tests/developer_productivity/test_diagnostic_parser_ctest.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-gcc-test
        framework.developer_productivity.diagnostic-parser-gcc
        tests/developer_productivity/test_diagnostic_parser_gcc.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-java-test
        framework.developer_productivity.diagnostic-parser-java
        tests/developer_productivity/test_diagnostic_parser_java.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-linker-test
        framework.developer_productivity.diagnostic-parser-linker
        tests/developer_productivity/test_diagnostic_parser_linker.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-msvc-test
        framework.developer_productivity.diagnostic-parser-msvc
        tests/developer_productivity/test_diagnostic_parser_msvc.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-ninja-test
        framework.developer_productivity.diagnostic-parser-ninja
        tests/developer_productivity/test_diagnostic_parser_ninja.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-python-test
        framework.developer_productivity.diagnostic-parser-python
        tests/developer_productivity/test_diagnostic_parser_python.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-rust-test
        framework.developer_productivity.diagnostic-parser-rust
        tests/developer_productivity/test_diagnostic_parser_rust.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-typescript-test
        framework.developer_productivity.diagnostic-parser-typescript
        tests/developer_productivity/test_diagnostic_parser_typescript.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-parser-zig-test
        framework.developer_productivity.diagnostic-parser-zig
        tests/developer_productivity/test_diagnostic_parser_zig.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diagnostic-pipeline-test
        framework.developer_productivity.diagnostic-pipeline
        tests/developer_productivity/test_diagnostic_pipeline.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diff-test
        framework.developer_productivity.diff
        tests/developer_productivity/test_diff.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-diff-hunk-test
        framework.developer_productivity.diff-hunk
        tests/developer_productivity/test_diff_hunk.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-directory-compare-test
        framework.developer_productivity.directory-compare
        tests/developer_productivity/test_directory_compare.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-location-link-test
        framework.developer_productivity.location-link
        tests/developer_productivity/test_location_link.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-merge-test
        framework.developer_productivity.merge
        tests/developer_productivity/test_merge.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-navigation-history-test
        framework.developer_productivity.navigation-history
        tests/developer_productivity/test_navigation_history.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-output-channels-test
        framework.developer_productivity.output-channels
        tests/developer_productivity/test_output_channels.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-problem-store-test
        framework.developer_productivity.problem-store
        tests/developer_productivity/test_problem_store.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-references-test
        framework.developer_productivity.references
        tests/developer_productivity/test_references.c
    )
    umicom_add_developer_productivity_test(
        umicom-devprod-branch-checkout-test
        framework.developer_productivity.source-control-operation-branch-checkout
        tests/developer_productivity/test_source_control_operation_branch_checkout.c
    )
    set_target_properties(
        umicom-devprod-branch-checkout-test
        PROPERTIES OUTPUT_NAME
                   umicom-developer-productivity-source-control-operation-branch-checkout-test
    )
    add_custom_target(
        umicom-developer-productivity-source-control-operation-branch-checkout-test
        DEPENDS umicom-devprod-branch-checkout-test
    )
    umicom_add_developer_productivity_test(
        umicom-devprod-branch-create-test
        framework.developer_productivity.source-control-operation-branch-create
        tests/developer_productivity/test_source_control_operation_branch_create.c
    )
    set_target_properties(
        umicom-devprod-branch-create-test
        PROPERTIES OUTPUT_NAME
                   umicom-developer-productivity-source-control-operation-branch-create-test
    )
    add_custom_target(
        umicom-developer-productivity-source-control-operation-branch-create-test
        DEPENDS umicom-devprod-branch-create-test
    )
    umicom_add_developer_productivity_test(
        umicom-devprod-branch-delete-test
        framework.developer_productivity.source-control-operation-branch-delete
        tests/developer_productivity/test_source_control_operation_branch_delete.c
    )
    set_target_properties(
        umicom-devprod-branch-delete-test
        PROPERTIES OUTPUT_NAME
                   umicom-developer-productivity-source-control-operation-branch-delete-test
    )
    add_custom_target(
        umicom-developer-productivity-source-control-operation-branch-delete-test
        DEPENDS umicom-devprod-branch-delete-test
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-commit-test
        framework.developer_productivity.source-control-operation-commit
        tests/developer_productivity/test_source_control_operation_commit.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-diff-test
        framework.developer_productivity.source-control-operation-diff
        tests/developer_productivity/test_source_control_operation_diff.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-discard-test
        framework.developer_productivity.source-control-operation-discard
        tests/developer_productivity/test_source_control_operation_discard.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-fetch-test
        framework.developer_productivity.source-control-operation-fetch
        tests/developer_productivity/test_source_control_operation_fetch.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-pull-test
        framework.developer_productivity.source-control-operation-pull
        tests/developer_productivity/test_source_control_operation_pull.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-push-test
        framework.developer_productivity.source-control-operation-push
        tests/developer_productivity/test_source_control_operation_push.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-refresh-test
        framework.developer_productivity.source-control-operation-refresh
        tests/developer_productivity/test_source_control_operation_refresh.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-stage-test
        framework.developer_productivity.source-control-operation-stage
        tests/developer_productivity/test_source_control_operation_stage.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-stage-all-test
        framework.developer_productivity.source-control-operation-stage-all
        tests/developer_productivity/test_source_control_operation_stage_all.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-source-control-operation-unstage-test
        framework.developer_productivity.source-control-operation-unstage
        tests/developer_productivity/test_source_control_operation_unstage.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-symbols-test
        framework.developer_productivity.symbols
        tests/developer_productivity/test_symbols.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-bash-test
        framework.developer_productivity.terminal-profile-bash
        tests/developer_productivity/test_terminal_profile_bash.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-cmd-test
        framework.developer_productivity.terminal-profile-cmd
        tests/developer_productivity/test_terminal_profile_cmd.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-git-bash-test
        framework.developer_productivity.terminal-profile-git-bash
        tests/developer_productivity/test_terminal_profile_git_bash.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-powershell-test
        framework.developer_productivity.terminal-profile-powershell
        tests/developer_productivity/test_terminal_profile_powershell.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-ucrt64-bash-test
        framework.developer_productivity.terminal-profile-ucrt64-bash
        tests/developer_productivity/test_terminal_profile_ucrt64_bash.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-profile-wsl-test
        framework.developer_productivity.terminal-profile-wsl
        tests/developer_productivity/test_terminal_profile_wsl.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-split-test
        framework.developer_productivity.terminal-split
        tests/developer_productivity/test_terminal_split.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-terminal-tabs-test
        framework.developer_productivity.terminal-tabs
        tests/developer_productivity/test_terminal_tabs.c
    )
    umicom_add_developer_productivity_test(
        umicom-developer-productivity-unified-diff-test
        framework.developer_productivity.unified-diff
        tests/developer_productivity/test_unified_diff.c
    )
endif()

message(STATUS
    "Umicom Source Control, diff/merge, Terminal, Problems and navigation platform enabled")
