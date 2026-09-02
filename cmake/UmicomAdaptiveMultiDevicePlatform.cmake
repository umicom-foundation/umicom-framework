#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAdaptiveMultiDevicePlatform.cmake
#
# PURPOSE:
#   Extend canonical Umicom::ui and Umicom::designer with adaptive application
#   shell runtime semantics and multi-device responsive visual-authoring services.
#
# ARCHITECTURE:
#   Existing Design System, reactive UI, data-view, workbench and RAD designer
#   contracts remain authoritative. This platform composes them without creating
#   a competing UI, designer or renderer target.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_ADAPTIVE_DEVICE_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_ui OR NOT TARGET umicom_designer)
    message(FATAL_ERROR "UmicomAdaptiveMultiDevicePlatform.cmake requires canonical umicom_ui and umicom_designer")
endif()

# Adaptive application-shell runtime capability extends canonical Umicom::ui.
target_sources(umicom_ui PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/adaptive_manifest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/adaptive_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/breakpoint_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/chart_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/content_priority.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/density_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/device_family.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/dialog_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/display_metrics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/form_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/form_factor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/grid_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/input_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/multi_window_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/navigation_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/navigation_item.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/navigation_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/navigation_pattern.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/orientation_change.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/panel_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/responsive_region.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/safe_area.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/shell_layout.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/shell_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/shell_region.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/shell_resolver.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/sidebar_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/tab_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/text_scale_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/toolbar_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/touch_target_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/viewport_class.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/adaptive/visibility_policy.c"
)

# Multi-device visual-authoring capability extends canonical Umicom::designer.
target_sources(umicom_designer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/accessibility_scale.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/adaptive_designer_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/adaptive_override.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/adaptive_property.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/adaptive_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/adaptive_variant.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/breakpoint_designer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/breakpoint_inheritance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/conformance_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/layout_resolver.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/layout_rule.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/layout_rule_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/locale_layout_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/master_detail_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/media_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/navigation_designer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/preview_case.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/preview_matrix.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/preview_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/renderer_semantics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/responsive_hierarchy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/responsive_preview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/responsive_property_editor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/safe_area_resolver.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/shell_designer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/table_adaptation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/variant_designer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/variant_resolver.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/variant_set.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/window_class.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/adaptive/workbench_adaptation.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add ui adaptive test build helper so parent and application projects apply
    # one consistent rule.
    function(umicom_add_ui_adaptive_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_ADAPTIVE_DEVICE_ROOT}/${source}")
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
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;adaptive-shell;multi-device;ui-adaptive")
    endfunction()

    # Define the add designer adaptive test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_designer_adaptive_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_ADAPTIVE_DEVICE_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::designer)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;adaptive-designer;multi-device;responsive-preview")
    endfunction()

    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-adaptive-manifest-test
        framework.ui_adaptive.adaptive.manifest
        tests/ui_adaptive/test_adaptive_manifest.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-adaptive-service-test
        framework.ui_adaptive.adaptive.service
        tests/ui_adaptive/test_adaptive_service.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-breakpoint-catalogue-test
        framework.ui_adaptive.breakpoint.catalogue
        tests/ui_adaptive/test_breakpoint_catalogue.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-chart-adaptation-test
        framework.ui_adaptive.chart.adaptation
        tests/ui_adaptive/test_chart_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-content-priority-test
        framework.ui_adaptive.content.priority
        tests/ui_adaptive/test_content_priority.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-density-policy-test
        framework.ui_adaptive.density.policy
        tests/ui_adaptive/test_density_policy.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-device-family-test
        framework.ui_adaptive.device.family
        tests/ui_adaptive/test_device_family.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-dialog-adaptation-test
        framework.ui_adaptive.dialog.adaptation
        tests/ui_adaptive/test_dialog_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-display-metrics-test
        framework.ui_adaptive.display.metrics
        tests/ui_adaptive/test_display_metrics.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-form-adaptation-test
        framework.ui_adaptive.form.adaptation
        tests/ui_adaptive/test_form_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-form-factor-test
        framework.ui_adaptive.form.factor
        tests/ui_adaptive/test_form_factor.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-grid-adaptation-test
        framework.ui_adaptive.grid.adaptation
        tests/ui_adaptive/test_grid_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-input-profile-test
        framework.ui_adaptive.input.profile
        tests/ui_adaptive/test_input_profile.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-multi-window-policy-test
        framework.ui_adaptive.multi.window.policy
        tests/ui_adaptive/test_multi_window_policy.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-navigation-adaptation-test
        framework.ui_adaptive.navigation.adaptation
        tests/ui_adaptive/test_navigation_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-navigation-item-test
        framework.ui_adaptive.navigation.item
        tests/ui_adaptive/test_navigation_item.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-navigation-model-test
        framework.ui_adaptive.navigation.model
        tests/ui_adaptive/test_navigation_model.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-navigation-pattern-test
        framework.ui_adaptive.navigation.pattern
        tests/ui_adaptive/test_navigation_pattern.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-orientation-change-test
        framework.ui_adaptive.orientation.change
        tests/ui_adaptive/test_orientation_change.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-panel-adaptation-test
        framework.ui_adaptive.panel.adaptation
        tests/ui_adaptive/test_panel_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-responsive-region-test
        framework.ui_adaptive.responsive.region
        tests/ui_adaptive/test_responsive_region.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-safe-area-test
        framework.ui_adaptive.safe.area
        tests/ui_adaptive/test_safe_area.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-shell-layout-test
        framework.ui_adaptive.shell.layout
        tests/ui_adaptive/test_shell_layout.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-shell-profile-test
        framework.ui_adaptive.shell.profile
        tests/ui_adaptive/test_shell_profile.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-shell-region-test
        framework.ui_adaptive.shell.region
        tests/ui_adaptive/test_shell_region.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-shell-resolver-test
        framework.ui_adaptive.shell.resolver
        tests/ui_adaptive/test_shell_resolver.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-sidebar-adaptation-test
        framework.ui_adaptive.sidebar.adaptation
        tests/ui_adaptive/test_sidebar_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-tab-adaptation-test
        framework.ui_adaptive.tab.adaptation
        tests/ui_adaptive/test_tab_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-text-scale-policy-test
        framework.ui_adaptive.text.scale.policy
        tests/ui_adaptive/test_text_scale_policy.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-toolbar-adaptation-test
        framework.ui_adaptive.toolbar.adaptation
        tests/ui_adaptive/test_toolbar_adaptation.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-touch-target-policy-test
        framework.ui_adaptive.touch.target.policy
        tests/ui_adaptive/test_touch_target_policy.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-types-test
        framework.ui_adaptive.types
        tests/ui_adaptive/test_types.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-viewport-class-test
        framework.ui_adaptive.viewport.class
        tests/ui_adaptive/test_viewport_class.c
    )
    umicom_add_ui_adaptive_test(
        umicom-ui-adaptive-visibility-policy-test
        framework.ui_adaptive.visibility.policy
        tests/ui_adaptive/test_visibility_policy.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-accessibility-scale-test
        framework.designer_adaptive.accessibility.scale
        tests/designer_adaptive/test_accessibility_scale.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-adaptive-designer-service-test
        framework.designer_adaptive.adaptive.designer.service
        tests/designer_adaptive/test_adaptive_designer_service.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-adaptive-override-test
        framework.designer_adaptive.adaptive.override
        tests/designer_adaptive/test_adaptive_override.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-adaptive-property-test
        framework.designer_adaptive.adaptive.property
        tests/designer_adaptive/test_adaptive_property.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-adaptive-validation-test
        framework.designer_adaptive.adaptive.validation
        tests/designer_adaptive/test_adaptive_validation.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-adaptive-variant-test
        framework.designer_adaptive.adaptive.variant
        tests/designer_adaptive/test_adaptive_variant.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-breakpoint-designer-test
        framework.designer_adaptive.breakpoint.designer
        tests/designer_adaptive/test_breakpoint_designer.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-breakpoint-inheritance-test
        framework.designer_adaptive.breakpoint.inheritance
        tests/designer_adaptive/test_breakpoint_inheritance.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-conformance-profile-test
        framework.designer_adaptive.conformance.profile
        tests/designer_adaptive/test_conformance_profile.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-layout-resolver-test
        framework.designer_adaptive.layout.resolver
        tests/designer_adaptive/test_layout_resolver.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-layout-rule-test
        framework.designer_adaptive.layout.rule
        tests/designer_adaptive/test_layout_rule.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-layout-rule-set-test
        framework.designer_adaptive.layout.rule.set
        tests/designer_adaptive/test_layout_rule_set.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-locale-layout-policy-test
        framework.designer_adaptive.locale.layout.policy
        tests/designer_adaptive/test_locale_layout_policy.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-master-detail-adaptation-test
        framework.designer_adaptive.master.detail.adaptation
        tests/designer_adaptive/test_master_detail_adaptation.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-media-adaptation-test
        framework.designer_adaptive.media.adaptation
        tests/designer_adaptive/test_media_adaptation.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-navigation-designer-test
        framework.designer_adaptive.navigation.designer
        tests/designer_adaptive/test_navigation_designer.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-preview-case-test
        framework.designer_adaptive.preview.case
        tests/designer_adaptive/test_preview_case.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-preview-matrix-test
        framework.designer_adaptive.preview.matrix
        tests/designer_adaptive/test_preview_matrix.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-preview-result-test
        framework.designer_adaptive.preview.result
        tests/designer_adaptive/test_preview_result.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-renderer-semantics-test
        framework.designer_adaptive.renderer.semantics
        tests/designer_adaptive/test_renderer_semantics.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-responsive-hierarchy-test
        framework.designer_adaptive.responsive.hierarchy
        tests/designer_adaptive/test_responsive_hierarchy.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-responsive-preview-test
        framework.designer_adaptive.responsive.preview
        tests/designer_adaptive/test_responsive_preview.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-responsive-property-editor-test
        framework.designer_adaptive.responsive.property.editor
        tests/designer_adaptive/test_responsive_property_editor.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-safe-area-resolver-test
        framework.designer_adaptive.safe.area.resolver
        tests/designer_adaptive/test_safe_area_resolver.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-shell-designer-test
        framework.designer_adaptive.shell.designer
        tests/designer_adaptive/test_shell_designer.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-table-adaptation-test
        framework.designer_adaptive.table.adaptation
        tests/designer_adaptive/test_table_adaptation.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-types-test
        framework.designer_adaptive.types
        tests/designer_adaptive/test_types.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-variant-designer-test
        framework.designer_adaptive.variant.designer
        tests/designer_adaptive/test_variant_designer.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-variant-resolver-test
        framework.designer_adaptive.variant.resolver
        tests/designer_adaptive/test_variant_resolver.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-variant-set-test
        framework.designer_adaptive.variant.set
        tests/designer_adaptive/test_variant_set.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-window-class-test
        framework.designer_adaptive.window.class
        tests/designer_adaptive/test_window_class.c
    )
    umicom_add_designer_adaptive_test(
        umicom-designer-adaptive-workbench-adaptation-test
        framework.designer_adaptive.workbench.adaptation
        tests/designer_adaptive/test_workbench_adaptation.c
    )
endif()

message(STATUS "Umicom adaptive multi-device shell and responsive designer platform enabled")
