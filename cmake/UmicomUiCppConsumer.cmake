#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUiCppConsumer.cmake
#
# PURPOSE:
#   Register C++ consumer qualification for reusable UI components, data views, layouts and optional GTK adapters.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------


include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomCAbiConsumerChecks.cmake")

function(umicom_add_ui_cpp_checks target_prefix include_root with_gtk)
    set(contracts
        umicom/ui/components/accessibility.h|umi_ui_component_accessibility_set|Umicom::ui_components
        umicom/ui/components/catalogue.h|umi_ui_component_catalogue_count|Umicom::ui_components
        umicom/ui/components/collection.h|umi_ui_component_collection_init|Umicom::ui_components
        umicom/ui/components/command.h|umi_ui_component_command_bind|Umicom::ui_components
        umicom/ui/components/component_set.h|umi_ui_component_set_init|Umicom::ui_components
        umicom/ui/components/factory.h|umi_ui_component_factory_is_valid|Umicom::ui_components
        umicom/ui/components/form.h|umi_ui_form_field_init|Umicom::ui_components
        umicom/ui/components/form_validation.h|umi_ui_form_validation_ok|Umicom::ui_components
        umicom/ui/components/layout.h|umi_ui_component_layout_default|Umicom::ui_components
        umicom/ui/components/model.h|umi_ui_component_model_default|Umicom::ui_components
        umicom/ui/components/property.h|umi_ui_component_property_set|Umicom::ui_components
        umicom/ui/components/registry.h|umi_ui_component_registry_init|Umicom::ui_components
        umicom/ui/components/selection.h|umi_ui_component_selection_none|Umicom::ui_components
        umicom/ui/components/state.h|umi_ui_component_state_default|Umicom::ui_components
        umicom/ui/components/style.h|umi_ui_component_style_default|Umicom::ui_components
        umicom/ui/components/surface.h|umi_ui_surface_default|Umicom::ui_components
        umicom/ui/components/tree.h|umi_ui_component_tree_init|Umicom::ui_components
        umicom/ui/components/validation.h|umi_ui_component_validate_parent_child|Umicom::ui_components
        umicom/ui/enterprise/aggregation_result.h|umi_ui_ent_aggregation_result_init|Umicom::ui
        umicom/ui/enterprise/cell_cache.h|umi_ui_ent_cell_cache_init|Umicom::ui
        umicom/ui/enterprise/cell_validation.h|umi_ui_ent_cell_validation_validate|Umicom::ui
        umicom/ui/enterprise/cell_value.h|umi_ui_ent_cell_value_clear|Umicom::ui
        umicom/ui/enterprise/change_batch.h|umi_ui_ent_change_batch_init|Umicom::ui
        umicom/ui/enterprise/clipboard_export.h|umi_ui_ent_clipboard_export_init|Umicom::ui
        umicom/ui/enterprise/column_catalogue.h|umi_ui_ent_column_catalogue_init|Umicom::ui
        umicom/ui/enterprise/column_order.h|umi_ui_ent_column_order_init|Umicom::ui
        umicom/ui/enterprise/column_resize.h|umi_ui_ent_column_resize_apply|Umicom::ui
        umicom/ui/enterprise/column_virtualizer.h|umi_ui_ent_column_virtualizer_update|Umicom::ui
        umicom/ui/enterprise/column_visibility.h|umi_ui_ent_column_visibility_init|Umicom::ui
        umicom/ui/enterprise/conditional_format.h|umi_ui_ent_conditional_format_matches|Umicom::ui
        umicom/ui/enterprise/csv_export.h|umi_ui_ent_csv_export_field|Umicom::ui
        umicom/ui/enterprise/data_provider.h|umi_ui_ent_data_provider_validate|Umicom::ui
        umicom/ui/enterprise/edit_commit.h|umi_ui_ent_edit_commit_create|Umicom::ui
        umicom/ui/enterprise/edit_history.h|umi_ui_ent_edit_history_init|Umicom::ui
        umicom/ui/enterprise/edit_session.h|umi_ui_ent_edit_session_begin|Umicom::ui
        umicom/ui/enterprise/enterprise_data_service.h|umi_ui_ent_enterprise_data_service_init|Umicom::ui
        umicom/ui/enterprise/filter_plan.h|umi_ui_ent_filter_plan_init|Umicom::ui
        umicom/ui/enterprise/filter_predicate.h|umi_ui_ent_filter_predicate_validate|Umicom::ui
        umicom/ui/enterprise/focus_model.h|umi_ui_ent_focus_model_clear|Umicom::ui
        umicom/ui/enterprise/hit_test.h|umi_ui_ent_hit_test_uniform|Umicom::ui
        umicom/ui/enterprise/inspector_commit.h|umi_ui_ent_inspector_commit_create|Umicom::ui
        umicom/ui/enterprise/inspector_validation.h|umi_ui_ent_inspector_validation_run|Umicom::ui
        umicom/ui/enterprise/keyboard_navigation.h|umi_ui_ent_keyboard_navigation_apply|Umicom::ui
        umicom/ui/enterprise/measurement_cache.h|umi_ui_ent_measurement_cache_init|Umicom::ui
        umicom/ui/enterprise/pagination.h|umi_ui_ent_pagination_page_count|Umicom::ui
        umicom/ui/enterprise/pinned_rows.h|umi_ui_ent_pinned_rows_init|Umicom::ui
        umicom/ui/enterprise/query_state.h|umi_ui_ent_query_state_init|Umicom::ui
        umicom/ui/enterprise/recycler_pool.h|umi_ui_ent_recycler_pool_init|Umicom::ui
        umicom/ui/enterprise/row_cache.h|umi_ui_ent_row_cache_init|Umicom::ui
        umicom/ui/enterprise/row_key.h|umi_ui_ent_row_key_from_text|Umicom::ui
        umicom/ui/enterprise/row_virtualizer.h|umi_ui_ent_row_virtualizer_update|Umicom::ui
        umicom/ui/enterprise/selection_model.h|umi_ui_ent_selection_model_init|Umicom::ui
        umicom/ui/enterprise/selection_range.h|umi_ui_ent_selection_range_normalize|Umicom::ui
        umicom/ui/enterprise/sort_plan.h|umi_ui_ent_sort_plan_init|Umicom::ui
        umicom/ui/enterprise/sparse_index.h|umi_ui_ent_sparse_index_init|Umicom::ui
        umicom/ui/enterprise/streaming_window.h|umi_ui_ent_streaming_window_init|Umicom::ui
        umicom/ui/enterprise/summary_footer.h|umi_ui_ent_summary_footer_init|Umicom::ui
        umicom/ui/enterprise/tree_expansion.h|umi_ui_ent_tree_expansion_init|Umicom::ui
        umicom/ui/enterprise/tree_filter.h|umi_ui_ent_tree_filter_set|Umicom::ui
        umicom/ui/enterprise/tree_lazy_loader.h|umi_ui_ent_tree_lazy_loader_request|Umicom::ui
        umicom/ui/enterprise/tree_path.h|umi_ui_ent_tree_path_init|Umicom::ui
        umicom/ui/enterprise/tree_projection.h|umi_ui_ent_tree_projection_init|Umicom::ui
        umicom/ui/enterprise/tree_selection.h|umi_ui_ent_tree_selection_init|Umicom::ui
        umicom/ui/enterprise/tree_sort.h|umi_ui_ent_tree_sort_compare|Umicom::ui
        umicom/ui/enterprise/tree_table.h|umi_ui_ent_tree_table_init|Umicom::ui
        umicom/ui/enterprise/viewport.h|umi_ui_ent_viewport_row_span|Umicom::ui
        umicom/ui/enterprise/virtual_range.h|umi_ui_ent_virtual_range_intersection|Umicom::ui
        umicom/ui/gtk4/catalogue.h|umi_gtk4_component_is_supported|Umicom::ui_gtk4
        umicom/ui/gtk4/component_factory.h|umi_gtk4_component_create|Umicom::ui_gtk4
        umicom/ui/gtk4/containers.h|umi_gtk4_box_new|Umicom::ui_gtk4
        umicom/ui/gtk4/controls.h|umi_gtk4_button_new|Umicom::ui_gtk4
        umicom/ui/gtk4/feedback.h|umi_gtk4_status_message_new|Umicom::ui_gtk4
        umicom/ui/gtk4/media.h|umi_gtk4_picture_new|Umicom::ui_gtk4
        umicom/ui/gtk4/native_factory.h|umi_gtk4_native_widget_create|Umicom::ui_gtk4
        umicom/ui/gtk4/navigation.h|umi_gtk4_tab_host_new|Umicom::ui_gtk4
        umicom/ui/gtk4/views.h|umi_gtk4_text_view_new|Umicom::ui_gtk4
        umicom/ui/gtk4/window.h|umi_gtk4_window_new|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/asset_browser.h|umi_gtk4_ws_asset_browser_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/chart_surface.h|umi_gtk4_ws_chart_surface_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/command_bar.h|umi_gtk4_ws_command_bar_config_default|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/data_grid.h|umi_gtk4_ws_data_grid_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/dock_overlay.h|umi_gtk4_ws_dock_overlay_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/inspector.h|umi_gtk4_ws_inspector_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/layout_library.h|umi_gtk4_ws_layout_library_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/library_exchange.h|umi_gtk4_ws_library_exchange_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/media_canvas.h|umi_gtk4_ws_media_viewport_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/node_graph.h|umi_gtk4_ws_node_graph_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/palette.h|umi_gtk4_ws_palette_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/panel_frame.h|umi_gtk4_ws_panel_frame_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/split_host.h|umi_gtk4_ws_split_host_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/status_strip.h|umi_gtk4_ws_status_strip_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/timeline.h|umi_gtk4_ws_timeline_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/tool_rail.h|umi_gtk4_ws_tool_rail_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/transport.h|umi_gtk4_ws_transport_create|Umicom::ui_gtk4
        umicom/ui/gtk4/workstation/workspace_strip.h|umi_gtk4_ws_workspace_strip_create|Umicom::ui_gtk4
        umicom/ui/layout_library.h|umi_ui_layout_library_create|Umicom::ui
        umicom/ui/layout_persistence.h|umi_ui_layout_persistence_encode|Umicom::ui
        umicom/ui/theme_profile.h|umi_ui_theme_profile_init|Umicom::ui
        umicom/ui/window_catalogue.h|umi_ui_window_catalogue_init|Umicom::ui
        umicom/ui/window_group.h|umi_ui_window_group_define|Umicom::ui
        umicom/ui/workspace_customisation.h|umi_ui_workspace_customisation_init|Umicom::ui
        umicom/ui/workspace_customisation_persistence.h|umi_ui_workspace_import_options_default|Umicom::ui
        umicom/ui/workspace_layout.h|umi_ui_workspace_layout_init|Umicom::ui

    )
    set(check_targets)
    foreach(contract IN LISTS contracts)
        string(REPLACE "|" ";" parts "${contract}")
        list(GET parts 0 header)
        list(GET parts 1 symbol)
        list(GET parts 2 library)
        if(library STREQUAL "Umicom::ui_gtk4" AND NOT with_gtk)
            continue()
        endif()
        string(REGEX REPLACE "^umicom/ui/|[.]h$" "" key "${header}")
        string(REPLACE "/" "." key "${key}")
        set(target "${target_prefix}-header-${key}")
        umicom_add_c_abi_header_check("${target}" "framework.sdk.cpp_header.ui.${key}"
            "${header}" "${symbol}" "${library}" "${include_root}")
        list(APPEND check_targets "${target}")
    endforeach()
    foreach(workflow IN ITEMS components enterprise workspace)
        set(target "${target_prefix}-${workflow}")
        add_executable("${target}"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/sdk_consumers/ui_${workflow}.cpp")
        if(NOT "${include_root}" STREQUAL "")
            target_include_directories("${target}" PRIVATE "${include_root}")
        endif()
        target_link_libraries("${target}" PRIVATE Umicom::ui Umicom::ui_components)
        set_target_properties("${target}" PROPERTIES
            CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "framework.sdk.cpp_workflow.ui.${workflow}" COMMAND "${target}")
        set_tests_properties("framework.sdk.cpp_workflow.ui.${workflow}" PROPERTIES
            TIMEOUT 30 LABELS "framework;sdk;cpp-consumer;memory-only")
        list(APPEND check_targets "${target}")
    endforeach()
    add_custom_target("${target_prefix}-checks" DEPENDS ${check_targets})
endfunction()
