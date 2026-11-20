#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomReactiveUiStatePlatform.cmake
#
# PURPOSE:
#   Extend canonical Umicom::ui with declarative bindings, validation, computed
#   state, dependency propagation, commands and transactional reactive updates.
#
# ARCHITECTURE:
#   Existing UmiUiValue/ViewModel/Binding contracts remain authoritative. This
#   platform layers richer reactive semantics before workstation renderers.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_UI_REACTIVE_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_ui)
    message(FATAL_ERROR "UmicomReactiveUiStatePlatform.cmake requires canonical umicom_ui")
endif()

# Reactive state capability extends the existing canonical UI target.
target_sources(umicom_ui PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_direction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_endpoint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_descriptor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/converter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/converter_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/built_in_converters.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validator_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validation_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validation_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validation_group.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/validation_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/property_path.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/observable_value.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/observable_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/change_event.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/change_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/change_transaction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/change_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/dependency_node.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/dependency_edge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/dependency_graph.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/cycle_detection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/topological_order.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/computed_value.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/computed_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_token.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_lexer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_operand.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_operator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_program.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/expression_evaluator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/boolean_expression.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/comparison_expression.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/logical_expression.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/visibility_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/enablement_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/readonly_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/required_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/command_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/command_parameter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/command_enablement.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/event_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/selection_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/context_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/form_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/list_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/grid_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/tree_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/inspector_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/binding_scheduler.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/propagation_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/propagation_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/propagation_guard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/loop_prevention.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/debounce_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/throttle_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/batch_update.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/state_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/state_restore.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/state_graph.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/ui_state_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/reactive/conformance_profile.c"
)

# Compose HTTP and the existing state owner in a separate SDK component. UI
# clients without a web view keep the same dependencies and storage contract.
if(TARGET Umicom::web AND TARGET Umicom::protocol)
    add_library(umicom_reactive_web STATIC
        "${UMICOM_UI_REACTIVE_ROOT}/src/web/reactive_view.c"
        "${UMICOM_UI_REACTIVE_ROOT}/src/web/reactive_edit.c")
    add_library(Umicom::reactive_web ALIAS umicom_reactive_web)
    set_target_properties(umicom_reactive_web PROPERTIES EXPORT_NAME reactive_web
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom_reactive_web PUBLIC Umicom::ui Umicom::web PRIVATE Umicom::protocol)
    umicom_apply_warnings(umicom_reactive_web)
    umicom_apply_sanitizers(umicom_reactive_web)
    install(TARGETS umicom_reactive_web EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
    if(COMMAND umicom_register_sdk_libraries)
        umicom_register_sdk_libraries(umicom_reactive_web)
    endif()
    install(FILES "${UMICOM_UI_REACTIVE_ROOT}/resources/web/reactive/view.mjs"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/resources/web/reactive"
        COMPONENT Framework)
    install(FILES "${UMICOM_UI_REACTIVE_ROOT}/resources/web/reactive/view.mjs"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/resources/web/reactive"
        COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
    # The offline guide uses the same existing reactive asset and typed route.
    install(FILES "${UMICOM_UI_REACTIVE_ROOT}/docs/guides/native-reactive-boolean-edits.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
    install(FILES "${UMICOM_UI_REACTIVE_ROOT}/docs/guides/native-reactive-boolean-edits.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom_reactive_web FILES
            "${UMICOM_UI_REACTIVE_ROOT}/resources/web/reactive/view.mjs")
    endif()
    if(BUILD_TESTING)
        add_executable(umicom-reactive-web-view-test
            "${UMICOM_UI_REACTIVE_ROOT}/tests/ui_reactive/test_web_reactive_view.c")
        target_link_libraries(umicom-reactive-web-view-test PRIVATE Umicom::reactive_web)
        umicom_apply_warnings(umicom-reactive-web-view-test)
        umicom_apply_sanitizers(umicom-reactive-web-view-test)
        add_test(NAME framework.ui_reactive.web_view.native COMMAND umicom-reactive-web-view-test)
        set_tests_properties(framework.ui_reactive.web_view.native PROPERTIES TIMEOUT 30
            LABELS "framework;ui;reactive;web")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-reactive-web-view-test)
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ui_reactive.web_view.native
                TARGETS umicom-reactive-web-view-test)
        endif()
        add_executable(umicom-reactive-web-edit-test
            "${UMICOM_UI_REACTIVE_ROOT}/tests/ui_reactive/test_web_reactive_edit.c")
        target_link_libraries(umicom-reactive-web-edit-test PRIVATE Umicom::reactive_web)
        umicom_apply_warnings(umicom-reactive-web-edit-test)
        umicom_apply_sanitizers(umicom-reactive-web-edit-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-reactive-web-edit-test)
        endif()
        foreach(_case IN ITEMS success minimum maximum last-revision stale future exhausted range
                origin duplicate-origin required-origin method path permission reauthenticate gate-failure
                content-type duplicate-type query large-body embedded-zero unconfigured excess-header-count
                unknown-key wrong-kind malformed)
            set(_test "framework.ui_reactive.web_edit.${_case}")
            add_test(NAME "${_test}" COMMAND umicom-reactive-web-edit-test "${_case}")
            set_tests_properties("${_test}" PROPERTIES TIMEOUT 30 LABELS "framework;ui;reactive;web")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${_test}" TARGETS umicom-reactive-web-edit-test)
            endif()
        endforeach()
        # Cover the second value type through the same native transaction owner.
        add_executable(umicom-reactive-web-boolean-edit-test
            "${UMICOM_UI_REACTIVE_ROOT}/tests/ui_reactive/test_web_reactive_boolean_edit.c")
        target_link_libraries(umicom-reactive-web-boolean-edit-test PRIVATE Umicom::reactive_web)
        umicom_apply_warnings(umicom-reactive-web-boolean-edit-test)
        umicom_apply_sanitizers(umicom-reactive-web-boolean-edit-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-reactive-web-boolean-edit-test)
        endif()
        foreach(_case IN ITEMS set clear numeric quoted case extra-value leading-zero stale origin permission wrong-kind missing-key)
            set(_test "framework.ui_reactive.web_boolean_edit.${_case}")
            add_test(NAME "${_test}" COMMAND umicom-reactive-web-boolean-edit-test "${_case}")
            set_tests_properties("${_test}" PROPERTIES TIMEOUT 30 LABELS "framework;ui;reactive;web")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("${_test}" TARGETS umicom-reactive-web-boolean-edit-test)
            endif()
        endforeach()
        # A Windows executable found through WSL interop cannot read this
        # POSIX source path. Keep the former unrestricted discovery for review.
        if(FALSE)
            find_program(UMICOM_REACTIVE_NODE_EXECUTABLE NAMES node node.exe)
        endif()
        if(WIN32)
            find_program(UMICOM_REACTIVE_NODE_EXECUTABLE NAMES node node.exe)
        else()
            find_program(UMICOM_REACTIVE_NODE_EXECUTABLE NAMES node)
        endif()
        # Apply the same host check to a value cached by a previous configure.
        if(UMICOM_REACTIVE_NODE_EXECUTABLE AND
           (WIN32 OR NOT UMICOM_REACTIVE_NODE_EXECUTABLE MATCHES "\\.[Ee][Xx][Ee]$"))
            add_test(NAME framework.ui_reactive.web_view.browser_contract
                COMMAND "${UMICOM_REACTIVE_NODE_EXECUTABLE}" --test
                    "${UMICOM_UI_REACTIVE_ROOT}/tests/ui_reactive/test_browser_reactive_view.mjs")
            set_tests_properties(framework.ui_reactive.web_view.browser_contract PROPERTIES TIMEOUT 30
                LABELS "framework;ui;reactive;web")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(framework.ui_reactive.web_view.browser_contract
                    TARGETS umicom_reactive_web)
            endif()
        endif()
    endif()
endif()

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add ui reactive test build helper so parent and application projects apply
    # one consistent rule.
    function(umicom_add_ui_reactive_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_UI_REACTIVE_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::ui)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;ui-reactive;bindings;state-graph;validation")
    endfunction()

    umicom_add_ui_reactive_test(umicom-ui-reactive-types-test framework.ui_reactive.types tests/ui_reactive/test_types.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-direction-test framework.ui_reactive.binding.direction tests/ui_reactive/test_binding_direction.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-endpoint-test framework.ui_reactive.binding.endpoint tests/ui_reactive/test_binding_endpoint.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-descriptor-test framework.ui_reactive.binding.descriptor tests/ui_reactive/test_binding_descriptor.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-catalogue-test framework.ui_reactive.binding.catalogue tests/ui_reactive/test_binding_catalogue.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-converter-test framework.ui_reactive.converter tests/ui_reactive/test_converter.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-converter-registry-test framework.ui_reactive.converter.registry tests/ui_reactive/test_converter_registry.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-built-in-converters-test framework.ui_reactive.built.in.converters tests/ui_reactive/test_built_in_converters.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validator-test framework.ui_reactive.validator tests/ui_reactive/test_validator.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validator-registry-test framework.ui_reactive.validator.registry tests/ui_reactive/test_validator_registry.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validation-rule-test framework.ui_reactive.validation.rule tests/ui_reactive/test_validation_rule.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validation-result-test framework.ui_reactive.validation.result tests/ui_reactive/test_validation_result.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validation-group-test framework.ui_reactive.validation.group tests/ui_reactive/test_validation_group.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-validation-summary-test framework.ui_reactive.validation.summary tests/ui_reactive/test_validation_summary.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-property-path-test framework.ui_reactive.property.path tests/ui_reactive/test_property_path.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-observable-value-test framework.ui_reactive.observable.value tests/ui_reactive/test_observable_value.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-observable-store-test framework.ui_reactive.observable.store tests/ui_reactive/test_observable_store.c)
    add_executable(umicom-ui-reactive-event-commit-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_reactive/test_observable_event_commit.c")
    target_link_libraries(umicom-ui-reactive-event-commit-test PRIVATE Umicom::ui)
    umicom_apply_warnings(umicom-ui-reactive-event-commit-test)
    umicom_apply_sanitizers(umicom-ui-reactive-event-commit-test)
    foreach(case commit alias invalid-id stale previous-value unknown-path duplicate-path invalid-value empty last exhausted)
        add_test(NAME "framework.ui_reactive.event_commit.${case}"
            COMMAND umicom-ui-reactive-event-commit-test "${case}")
        set_tests_properties("framework.ui_reactive.event_commit.${case}" PROPERTIES TIMEOUT 20 LABELS "framework;ui;reactive")
    endforeach()
    add_executable(umicom-ui-reactive-view-delta-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui_reactive/test_observable_view_delta.c")
    target_link_libraries(umicom-ui-reactive-view-delta-test PRIVATE Umicom::ui)
    umicom_apply_warnings(umicom-ui-reactive-view-delta-test)
    umicom_apply_sanitizers(umicom-ui-reactive-view-delta-test)
    foreach(case initial committed repeated unchanged unknown duplicate future small corrupt invalid-key ownership empty exhausted)
        set(_test "framework.ui_reactive.view_delta.${case}")
        add_test(NAME "${_test}" COMMAND umicom-ui-reactive-view-delta-test "${case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 20 LABELS "framework;ui;reactive")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-ui-reactive-view-delta-test)
        endif()
    endforeach()
    umicom_add_ui_reactive_test(umicom-ui-reactive-change-event-test framework.ui_reactive.change.event tests/ui_reactive/test_change_event.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-change-queue-test framework.ui_reactive.change.queue tests/ui_reactive/test_change_queue.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-change-transaction-test framework.ui_reactive.change.transaction tests/ui_reactive/test_change_transaction.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-change-set-test framework.ui_reactive.change.set tests/ui_reactive/test_change_set.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-dependency-node-test framework.ui_reactive.dependency.node tests/ui_reactive/test_dependency_node.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-dependency-edge-test framework.ui_reactive.dependency.edge tests/ui_reactive/test_dependency_edge.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-dependency-graph-test framework.ui_reactive.dependency.graph tests/ui_reactive/test_dependency_graph.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-cycle-detection-test framework.ui_reactive.cycle.detection tests/ui_reactive/test_cycle_detection.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-topological-order-test framework.ui_reactive.topological.order tests/ui_reactive/test_topological_order.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-computed-value-test framework.ui_reactive.computed.value tests/ui_reactive/test_computed_value.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-computed-registry-test framework.ui_reactive.computed.registry tests/ui_reactive/test_computed_registry.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-token-test framework.ui_reactive.expression.token tests/ui_reactive/test_expression_token.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-lexer-test framework.ui_reactive.expression.lexer tests/ui_reactive/test_expression_lexer.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-operand-test framework.ui_reactive.expression.operand tests/ui_reactive/test_expression_operand.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-operator-test framework.ui_reactive.expression.operator tests/ui_reactive/test_expression_operator.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-program-test framework.ui_reactive.expression.program tests/ui_reactive/test_expression_program.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-expression-evaluator-test framework.ui_reactive.expression.evaluator tests/ui_reactive/test_expression_evaluator.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-boolean-expression-test framework.ui_reactive.boolean.expression tests/ui_reactive/test_boolean_expression.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-comparison-expression-test framework.ui_reactive.comparison.expression tests/ui_reactive/test_comparison_expression.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-logical-expression-test framework.ui_reactive.logical.expression tests/ui_reactive/test_logical_expression.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-visibility-rule-test framework.ui_reactive.visibility.rule tests/ui_reactive/test_visibility_rule.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-enablement-rule-test framework.ui_reactive.enablement.rule tests/ui_reactive/test_enablement_rule.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-readonly-rule-test framework.ui_reactive.readonly.rule tests/ui_reactive/test_readonly_rule.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-required-rule-test framework.ui_reactive.required.rule tests/ui_reactive/test_required_rule.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-command-binding-test framework.ui_reactive.command.binding tests/ui_reactive/test_command_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-command-parameter-test framework.ui_reactive.command.parameter tests/ui_reactive/test_command_parameter.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-command-enablement-test framework.ui_reactive.command.enablement tests/ui_reactive/test_command_enablement.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-event-binding-test framework.ui_reactive.event.binding tests/ui_reactive/test_event_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-selection-binding-test framework.ui_reactive.selection.binding tests/ui_reactive/test_selection_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-context-binding-test framework.ui_reactive.context.binding tests/ui_reactive/test_context_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-form-binding-test framework.ui_reactive.form.binding tests/ui_reactive/test_form_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-list-binding-test framework.ui_reactive.list.binding tests/ui_reactive/test_list_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-grid-binding-test framework.ui_reactive.grid.binding tests/ui_reactive/test_grid_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-tree-binding-test framework.ui_reactive.tree.binding tests/ui_reactive/test_tree_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-inspector-binding-test framework.ui_reactive.inspector.binding tests/ui_reactive/test_inspector_binding.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-session-test framework.ui_reactive.binding.session tests/ui_reactive/test_binding_session.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-binding-scheduler-test framework.ui_reactive.binding.scheduler tests/ui_reactive/test_binding_scheduler.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-propagation-plan-test framework.ui_reactive.propagation.plan tests/ui_reactive/test_propagation_plan.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-propagation-result-test framework.ui_reactive.propagation.result tests/ui_reactive/test_propagation_result.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-propagation-guard-test framework.ui_reactive.propagation.guard tests/ui_reactive/test_propagation_guard.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-loop-prevention-test framework.ui_reactive.loop.prevention tests/ui_reactive/test_loop_prevention.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-debounce-policy-test framework.ui_reactive.debounce.policy tests/ui_reactive/test_debounce_policy.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-throttle-policy-test framework.ui_reactive.throttle.policy tests/ui_reactive/test_throttle_policy.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-batch-update-test framework.ui_reactive.batch.update tests/ui_reactive/test_batch_update.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-state-snapshot-test framework.ui_reactive.state.snapshot tests/ui_reactive/test_state_snapshot.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-state-restore-test framework.ui_reactive.state.restore tests/ui_reactive/test_state_restore.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-state-graph-test framework.ui_reactive.state.graph tests/ui_reactive/test_state_graph.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-ui-state-service-test framework.ui_reactive.ui.state.service tests/ui_reactive/test_ui_state_service.c)
    umicom_add_ui_reactive_test(umicom-ui-reactive-conformance-profile-test framework.ui_reactive.conformance.profile tests/ui_reactive/test_conformance_profile.c)
endif()

message(STATUS "Umicom declarative bindings, validation and reactive UI state graph enabled")
