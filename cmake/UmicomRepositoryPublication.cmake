#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryPublication.cmake
#
# PURPOSE:
#   Register native workspace publication and focused regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Extend the established repository library so every product shares one policy.
target_sources(umicom_repository PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_git.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_lock.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_discovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_inspection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_recovery.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_primary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_scope.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_execution.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_progress.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_report.c")

# Use the established JSON escaping service for publication evidence.
target_link_libraries(umicom_repository PRIVATE Umicom::protocol)

if(BUILD_TESTING)
    # Existing cases remain registered; evidence and graph refresh add regressions.
    foreach(_case policy plan execution progress report report_file)
        add_executable("umicom-repo-publication-${_case}-test"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_${_case}.c")
        target_link_libraries("umicom-repo-publication-${_case}-test" PRIVATE Umicom::repository)
        target_include_directories("umicom-repo-publication-${_case}-test" PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations")
        umicom_apply_warnings("umicom-repo-publication-${_case}-test")
        umicom_apply_sanitizers("umicom-repo-publication-${_case}-test")
        add_test(NAME "framework.repository.publication.${_case}"
            COMMAND "umicom-repo-publication-${_case}-test")
        set_tests_properties("framework.repository.publication.${_case}" PROPERTIES
            LABELS "framework;repository;publication;regression" TIMEOUT 60)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-repo-publication-${_case}-test")
        endif()
    endforeach()
endif()

# Ship the public command guide with the Framework documentation.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/repository-publication.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides"
    COMPONENT Framework)

# The allowed report destination must be outside all source checkouts.
if(BUILD_TESTING)
    set_tests_properties(framework.repository.publication.report_file PROPERTIES
        UMICOM_TEST_OUTSIDE_REPOSITORY TRUE)
endif()

# Primary ownership is policy, not a Git side effect. These fixtures use a
# transport that refuses Git execution and inspect the shared planner directly.
if(BUILD_TESTING)
    add_executable(umicom-repo-publication-primary-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_primary.c")
    target_link_libraries(umicom-repo-publication-primary-test PRIVATE Umicom::repository)
    target_include_directories(umicom-repo-publication-primary-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations")
    umicom_apply_warnings(umicom-repo-publication-primary-test)
    umicom_apply_sanitizers(umicom-repo-publication-primary-test)
    foreach(_case default-options size-contract outside-workspace empty-directory
            clean-primary edited-primary edited-copy unpublished-copy identical-unpublished
            ambiguous-primary missing-directory boundary-directory other-repository
            remote-mismatch compatibility vendor-only)
        add_test(NAME "framework.repository.publication.primary.${_case}"
            COMMAND umicom-repo-publication-primary-test "${_case}")
        set_tests_properties("framework.repository.publication.primary.${_case}" PROPERTIES
            LABELS "framework;repository;publication;regression" TIMEOUT 30)
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-repo-publication-primary-test)
    endif()
endif()

if(BUILD_TESTING)
    add_executable(umicom-repo-publication-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_review.c")
    target_link_libraries(umicom-repo-publication-review-test PRIVATE Umicom::repository)
    target_include_directories(umicom-repo-publication-review-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations")
    umicom_apply_warnings(umicom-repo-publication-review-test)
    umicom_apply_sanitizers(umicom-repo-publication-review-test)
    foreach(_case approved sha256 refused display-failure changed-index tree-probe-failure
            summary-failure patch-failure summary-nul patch-nul summary-empty patch-empty
            invalid-object cancel-before-review cancel-during-review compatibility)
        add_test(NAME "framework.repository.publication.review.${_case}"
            COMMAND umicom-repo-publication-review-test "${_case}")
        set_tests_properties("framework.repository.publication.review.${_case}" PROPERTIES
            LABELS "framework;repository;publication;regression" TIMEOUT 30)
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-repo-publication-review-test)
    endif()
endif()

if(BUILD_TESTING)
    add_executable(umicom-repo-publication-cli-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_cli.c")
    target_link_libraries(umicom-repo-publication-cli-test PRIVATE Umicom::repository)
    umicom_apply_warnings(umicom-repo-publication-cli-test)
    umicom_apply_sanitizers(umicom-repo-publication-cli-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-repo-publication-cli-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_repository_publication.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-repo-publication-cli-test)
    endif()
    foreach(_case preview watch apply blocked-preview prepare-failure null-vector null-value
            empty-value negative-count null-context unknown-command empty-workspace missing-primary
            duplicate-primary primary-followed-by-option missing-message missing-report
            duplicate-review watch-apply watch-review duplicate-interval)
        add_test(NAME "framework.repository.publication.cli.${_case}"
            COMMAND umicom-repo-publication-cli-test "${_case}")
        set_tests_properties("framework.repository.publication.cli.${_case}" PROPERTIES
            LABELS "framework;repository;publication;cli;regression" TIMEOUT 30)
    endforeach()
endif()

# Explicit scope never suppresses registered child traversal. Controlled
# discovery and CLI seams exercise policy without a real Git process.
if(BUILD_TESTING)
    foreach(suite scope scope_cli)
        add_executable(umicom-repo-publication-${suite}-test "${CMAKE_CURRENT_LIST_DIR}/../tests/repository_operations/test_publication_${suite}.c")
        target_include_directories(umicom-repo-publication-${suite}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations")
        target_link_libraries(umicom-repo-publication-${suite}-test PRIVATE Umicom::repository)
        umicom_apply_warnings(umicom-repo-publication-${suite}-test)
        umicom_apply_sanitizers(umicom-repo-publication-${suite}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-repo-publication-${suite}-test)
        endif()
    endforeach()
    set(scope_cases empty valid duplicate null-vector null-name empty-name absolute nested parent option-name trailing-dot long-name excess-count total-limit size-contract skip-unrelated registered-child unmatched name-prefix all-excluded root-repository repeat-discovery)
    set(scope_cli_cases preview apply watch missing-name duplicate nested option-name too-many unknown unmatched-preview)
    foreach(suite scope scope_cli)
        foreach(case IN LISTS ${suite}_cases)
            add_test(NAME framework.repository.publication.${suite}.${case} COMMAND umicom-repo-publication-${suite}-test "${case}")
            set_tests_properties(framework.repository.publication.${suite}.${case} PROPERTIES TIMEOUT 30 LABELS "framework;repository;publication;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.repository.publication.${suite}.${case} TARGETS umicom-repo-publication-${suite}-test)
            endif()
        endforeach()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-repo-publication-scope-test FILES "${CMAKE_CURRENT_LIST_DIR}/../src/repository/operations/publication_discovery.c")
        umicom_target_qualification_inputs(umicom-repo-publication-scope_cli-test FILES "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_repository_publication.c" "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/cli.h")
    endif()
endif()
