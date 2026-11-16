# Umicom Framework
# File: cmake/UmicomFormDraft.cmake
#
# PURPOSE:
#   Compose the native form draft owner with existing application services and regression registration.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
set(_umicom_form_draft_root "${CMAKE_CURRENT_LIST_DIR}/..")
# Form values remain in the application library; clearing private copies reuses
# the existing security primitive rather than another application implementation.
target_sources(umicom_application PRIVATE "${_umicom_form_draft_root}/src/application/experience/form_draft.c")
target_link_libraries(umicom_application PRIVATE Umicom::security)
if(BUILD_TESTING)
    add_executable(umicom-form-draft-test "${_umicom_form_draft_root}/tests/application_experience/test_form_draft.c")
    target_link_libraries(umicom-form-draft-test PRIVATE Umicom::application)
    umicom_apply_warnings(umicom-form-draft-test)
    umicom_apply_sanitizers(umicom-form-draft-test)
    foreach(_case IN ITEMS isolation atomic_revision consumed stale_revision stale_edited stale_unedited owner_identity schema_identity missing_required repair_required unset_optional unset_required unset_default unset_absent read_only unknown_field invalid_value number_range copied_snapshot snapshot_owner no_change reverted_change revision_limit saved_marker parallel_drafts separate_owners invalid_presence invalid_value_model revision_zero invalid_schema occupied nulls)
        add_test(NAME "framework.application_experience.form_draft.${_case}" COMMAND umicom-form-draft-test "${_case}")
        set_tests_properties("framework.application_experience.form_draft.${_case}" PROPERTIES TIMEOUT 20 LABELS "framework;forms;regression")
    endforeach()
    add_executable(umicom-form-draft-example "${_umicom_form_draft_root}/examples/form_draft/main.c")
    target_link_libraries(umicom-form-draft-example PRIVATE Umicom::application)
    umicom_apply_warnings(umicom-form-draft-example)
    umicom_apply_sanitizers(umicom-form-draft-example)
    add_test(NAME framework.application_experience.form_draft.example COMMAND umicom-form-draft-example)
    set_tests_properties(framework.application_experience.form_draft.example PROPERTIES TIMEOUT 20 LABELS "framework;forms;example")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-form-draft-test)
        umicom_register_validation_target(umicom-form-draft-example)
    endif()
endif()

# Reconciliation extends the same draft owner; neither UI nor SDK consumers
# compile their own merge policy. The existing commit suite remains registered.
if(BUILD_TESTING)
    add_executable(umicom-form-reconcile-test "${_umicom_form_draft_root}/tests/application_experience/test_form_reconcile.c")
    add_executable(umicom-form-reconcile-example "${_umicom_form_draft_root}/examples/form_draft/reconcile.c")
    foreach(_target umicom-form-reconcile-test umicom-form-reconcile-example)
        target_link_libraries(${_target} PRIVATE Umicom::application)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case unchanged local-only remote-only disjoint convergent conflict conflict-order conflict-preserve resolve-live commit-after unversioned-live remote-unset local-unset converged-unset presence-default required-incomplete readonly-refresh saved-marker revision-only snapshot-revision revision-limit limit-nochange consumed wrong-owner schema null-schema backwards bad-presence bad-value nulls repeat edit-after maximum-conflicts)
        add_test(NAME framework.application_experience.form_reconcile.${_case} COMMAND umicom-form-reconcile-test ${_case})
        set_tests_properties(framework.application_experience.form_reconcile.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;forms;reconciliation")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.application_experience.form_reconcile.${_case} TARGETS umicom-form-reconcile-test)
        endif()
    endforeach()
    add_test(NAME framework.application_experience.form_reconcile.example COMMAND umicom-form-reconcile-example)
    set_tests_properties(framework.application_experience.form_reconcile.example PROPERTIES TIMEOUT 20 LABELS "framework;forms;sdk")
    umicom_apply_application_branding(TARGET umicom-form-reconcile-example
        PRODUCT_NAME "Umicom Draft Refresh Example" INTERNAL_NAME "umicom-form-reconcile-example"
        APPLICATION_ID "org.umicom.form-reconcile-example"
        RESOURCE_ROOT "${_umicom_form_draft_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
# Ship both complete consumer sources with selective SDK installs. Keeping an
# example in the source tree alone does not make it available in a package.
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_form_draft_root}/examples/form_draft/main.c"
        "${_umicom_form_draft_root}/examples/form_draft/reconcile.c"
        "${_umicom_form_draft_root}/examples/form_draft/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/form_draft"
        COMPONENT "${_component}" ${_selection})
endforeach()
