#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomVisualApplicationDesignerPlatform.cmake
#
# PURPOSE:
#   Configure the umicom visual application designer platform build rules
#   without duplicating product logic.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomVisualApplicationDesignerPlatform.cmake
#
# PURPOSE:
#   Extend canonical Umicom::designer with production visual application,
#   form and page authoring services that consume canonical Umicom::ui state.
#
# ARCHITECTURE:
#   Existing designer documents/history/palette/preview/source generation remain
#   authoritative. This platform composes Design System, reactive UI and data
#   view semantics without creating a second visual designer target.
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(UMICOM_VISUAL_DESIGNER_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_designer OR NOT TARGET umicom_ui)
    message(FATAL_ERROR "UmicomVisualApplicationDesignerPlatform.cmake requires canonical umicom_designer and umicom_ui")
endif()

# Production visual-authoring capability extends the established designer target.
target_sources(umicom_designer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/coordinate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/size.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/rect.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/device_profile.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/canvas.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/canvas_viewport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/zoom.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/pan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/grid.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/ruler.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/guide.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/snap_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/snap_result.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/alignment_guide.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/distribution_guide.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/selection_bounds.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/resize_handle.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/drag_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/drop_target.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/insertion_marker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/component_instance.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/component_catalogue_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/palette_filter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/hierarchy_node.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/hierarchy_tree.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/hierarchy_move.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/property_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/property_editor_descriptor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/property_commit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/event_descriptor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/event_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/action_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/binding_endpoint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/binding_wire.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/binding_graph.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/validation_marker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/validation_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/undo_command.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/undo_stack.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/transaction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/clipboard_payload.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/duplicate_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/delete_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/z_order.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/parent_constraint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/layout_constraint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/anchor_constraint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/flex_constraint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/grid_constraint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/responsive_variant.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/breakpoint_preview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/page_descriptor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/form_descriptor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/page_template.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/form_template.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/template_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/preview_target.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/preview_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/preview_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/document_manifest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/document_fingerprint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/declarative_emitter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/generation_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/designer_workspace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/designer/visual_designer/designer_service.c"
)
# Designer orchestration consumes the canonical semantic UI/design/reactive contracts.
target_link_libraries(umicom_designer PUBLIC Umicom::ui)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add visual designer test build helper so parent and application projects apply
    # one consistent rule.
    function(umicom_add_visual_designer_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_VISUAL_DESIGNER_ROOT}/${source}")
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
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;designer;visual-designer;application-designer")
    endfunction()
umicom_add_visual_designer_test(
    umicom-visual-designer-types-test
    framework.designer.visual_designer.types
    tests/visual_designer/test_types.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-coordinate-test
    framework.designer.visual_designer.coordinate
    tests/visual_designer/test_coordinate.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-size-test
    framework.designer.visual_designer.size
    tests/visual_designer/test_size.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-rect-test
    framework.designer.visual_designer.rect
    tests/visual_designer/test_rect.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-device-profile-test
    framework.designer.visual_designer.device.profile
    tests/visual_designer/test_device_profile.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-canvas-test
    framework.designer.visual_designer.canvas
    tests/visual_designer/test_canvas.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-canvas-viewport-test
    framework.designer.visual_designer.canvas.viewport
    tests/visual_designer/test_canvas_viewport.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-zoom-test
    framework.designer.visual_designer.zoom
    tests/visual_designer/test_zoom.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-pan-test
    framework.designer.visual_designer.pan
    tests/visual_designer/test_pan.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-grid-test
    framework.designer.visual_designer.grid
    tests/visual_designer/test_grid.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-ruler-test
    framework.designer.visual_designer.ruler
    tests/visual_designer/test_ruler.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-guide-test
    framework.designer.visual_designer.guide
    tests/visual_designer/test_guide.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-snap-policy-test
    framework.designer.visual_designer.snap.policy
    tests/visual_designer/test_snap_policy.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-snap-result-test
    framework.designer.visual_designer.snap.result
    tests/visual_designer/test_snap_result.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-alignment-guide-test
    framework.designer.visual_designer.alignment.guide
    tests/visual_designer/test_alignment_guide.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-distribution-guide-test
    framework.designer.visual_designer.distribution.guide
    tests/visual_designer/test_distribution_guide.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-selection-bounds-test
    framework.designer.visual_designer.selection.bounds
    tests/visual_designer/test_selection_bounds.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-resize-handle-test
    framework.designer.visual_designer.resize.handle
    tests/visual_designer/test_resize_handle.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-drag-session-test
    framework.designer.visual_designer.drag.session
    tests/visual_designer/test_drag_session.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-drop-target-test
    framework.designer.visual_designer.drop.target
    tests/visual_designer/test_drop_target.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-insertion-marker-test
    framework.designer.visual_designer.insertion.marker
    tests/visual_designer/test_insertion_marker.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-component-instance-test
    framework.designer.visual_designer.component.instance
    tests/visual_designer/test_component_instance.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-component-catalogue-bridge-test
    framework.designer.visual_designer.component.catalogue.bridge
    tests/visual_designer/test_component_catalogue_bridge.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-palette-filter-test
    framework.designer.visual_designer.palette.filter
    tests/visual_designer/test_palette_filter.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-hierarchy-node-test
    framework.designer.visual_designer.hierarchy.node
    tests/visual_designer/test_hierarchy_node.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-hierarchy-tree-test
    framework.designer.visual_designer.hierarchy.tree
    tests/visual_designer/test_hierarchy_tree.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-hierarchy-move-test
    framework.designer.visual_designer.hierarchy.move
    tests/visual_designer/test_hierarchy_move.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-property-binding-test
    framework.designer.visual_designer.property.binding
    tests/visual_designer/test_property_binding.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-property-editor-descriptor-test
    framework.designer.visual_designer.property.editor.descriptor
    tests/visual_designer/test_property_editor_descriptor.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-property-commit-test
    framework.designer.visual_designer.property.commit
    tests/visual_designer/test_property_commit.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-event-descriptor-test
    framework.designer.visual_designer.event.descriptor
    tests/visual_designer/test_event_descriptor.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-event-binding-test
    framework.designer.visual_designer.event.binding
    tests/visual_designer/test_event_binding.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-action-binding-test
    framework.designer.visual_designer.action.binding
    tests/visual_designer/test_action_binding.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-binding-endpoint-test
    framework.designer.visual_designer.binding.endpoint
    tests/visual_designer/test_binding_endpoint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-binding-wire-test
    framework.designer.visual_designer.binding.wire
    tests/visual_designer/test_binding_wire.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-binding-graph-test
    framework.designer.visual_designer.binding.graph
    tests/visual_designer/test_binding_graph.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-validation-marker-test
    framework.designer.visual_designer.validation.marker
    tests/visual_designer/test_validation_marker.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-validation-summary-test
    framework.designer.visual_designer.validation.summary
    tests/visual_designer/test_validation_summary.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-undo-command-test
    framework.designer.visual_designer.undo.command
    tests/visual_designer/test_undo_command.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-undo-stack-test
    framework.designer.visual_designer.undo.stack
    tests/visual_designer/test_undo_stack.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-transaction-test
    framework.designer.visual_designer.transaction
    tests/visual_designer/test_transaction.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-clipboard-payload-test
    framework.designer.visual_designer.clipboard.payload
    tests/visual_designer/test_clipboard_payload.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-duplicate-plan-test
    framework.designer.visual_designer.duplicate.plan
    tests/visual_designer/test_duplicate_plan.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-delete-plan-test
    framework.designer.visual_designer.delete.plan
    tests/visual_designer/test_delete_plan.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-z-order-test
    framework.designer.visual_designer.z.order
    tests/visual_designer/test_z_order.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-parent-constraint-test
    framework.designer.visual_designer.parent.constraint
    tests/visual_designer/test_parent_constraint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-layout-constraint-test
    framework.designer.visual_designer.layout.constraint
    tests/visual_designer/test_layout_constraint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-anchor-constraint-test
    framework.designer.visual_designer.anchor.constraint
    tests/visual_designer/test_anchor_constraint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-flex-constraint-test
    framework.designer.visual_designer.flex.constraint
    tests/visual_designer/test_flex_constraint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-grid-constraint-test
    framework.designer.visual_designer.grid.constraint
    tests/visual_designer/test_grid_constraint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-responsive-variant-test
    framework.designer.visual_designer.responsive.variant
    tests/visual_designer/test_responsive_variant.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-breakpoint-preview-test
    framework.designer.visual_designer.breakpoint.preview
    tests/visual_designer/test_breakpoint_preview.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-page-descriptor-test
    framework.designer.visual_designer.page.descriptor
    tests/visual_designer/test_page_descriptor.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-form-descriptor-test
    framework.designer.visual_designer.form.descriptor
    tests/visual_designer/test_form_descriptor.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-page-template-test
    framework.designer.visual_designer.page.template
    tests/visual_designer/test_page_template.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-form-template-test
    framework.designer.visual_designer.form.template
    tests/visual_designer/test_form_template.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-template-catalogue-test
    framework.designer.visual_designer.template.catalogue
    tests/visual_designer/test_template_catalogue.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-preview-target-test
    framework.designer.visual_designer.preview.target
    tests/visual_designer/test_preview_target.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-preview-session-test
    framework.designer.visual_designer.preview.session
    tests/visual_designer/test_preview_session.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-preview-state-test
    framework.designer.visual_designer.preview.state
    tests/visual_designer/test_preview_state.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-document-manifest-test
    framework.designer.visual_designer.document.manifest
    tests/visual_designer/test_document_manifest.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-document-fingerprint-test
    framework.designer.visual_designer.document.fingerprint
    tests/visual_designer/test_document_fingerprint.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-declarative-emitter-test
    framework.designer.visual_designer.declarative.emitter
    tests/visual_designer/test_declarative_emitter.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-generation-plan-test
    framework.designer.visual_designer.generation.plan
    tests/visual_designer/test_generation_plan.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-designer-workspace-test
    framework.designer.visual_designer.designer.workspace
    tests/visual_designer/test_designer_workspace.c
)
umicom_add_visual_designer_test(
    umicom-visual-designer-designer-service-test
    framework.designer.visual_designer.designer.service
    tests/visual_designer/test_designer_service.c
)
endif()

message(STATUS "Umicom visual application/form/page designer production platform enabled")
