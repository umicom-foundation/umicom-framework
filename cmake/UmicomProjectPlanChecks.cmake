#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProjectPlanChecks.cmake
#
# PURPOSE:
#   Register pure project-plan argument, publication and allocation-refusal coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND TARGET Umicom::developer)
    add_executable(umicom-project-plan-publication-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_plan_publication.c")
    target_link_libraries(umicom-project-plan-publication-test PRIVATE Umicom::developer)
    add_executable(umicom-project-plan-allocation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_project/test_plan_allocation.c")
    # The allocation fixture includes production planner source with one allocator
    # substitution. Its remaining operations are the actual shared build helpers.
    target_link_libraries(umicom-project-plan-allocation-test PRIVATE Umicom::build)
    foreach(target umicom-project-plan-publication-test umicom-project-plan-allocation-test)
        set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-project-plan-allocation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/build_plan.c")
    endif()
    foreach(case IN ITEMS directory-default-build preset-default-build directory-debug-build preset-debug-build directory-default-build-test preset-default-build-test directory-debug-build-test preset-debug-build-test directory-default-build-package preset-default-build-package directory-debug-build-package preset-debug-build-package directory-default-build-test-package preset-default-build-test-package directory-debug-build-test-package preset-debug-build-test-package cmake-alias zig-alias cargo-alias cmake-long-root zig-long-root cargo-long-root long-directory long-preset long-configuration null-output null-root missing-binary truthy-flags metadata-root metadata-directory metadata-preset metadata-configuration command-program argument-count invalid-enum excess-commands empty-initialized unused-command stale-cache copy-self copy-independent copy-invalid copy-null-source copy-null-destination provider-default provider-preset provider-null provider-malformed provider-null-output)
        add_test(NAME "framework.project.plan_publication.${case}"
            COMMAND umicom-project-plan-publication-test "${case}")
        set_tests_properties("framework.project.plan_publication.${case}" PROPERTIES
            TIMEOUT 20 LABELS "framework;build;planning;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.project.plan_publication.${case}"
                TARGETS umicom-project-plan-publication-test)
        endif()
    endforeach()
    foreach(kind cmake zig cargo)
        add_test(NAME "framework.project.plan_allocation.${kind}"
            COMMAND umicom-project-plan-allocation-test "${kind}")
        set_tests_properties("framework.project.plan_allocation.${kind}" PROPERTIES
            TIMEOUT 20 LABELS "framework;build;planning;allocation")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.project.plan_allocation.${kind}"
                TARGETS umicom-project-plan-allocation-test)
        endif()
    endforeach()
endif()
