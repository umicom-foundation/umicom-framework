#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSnapshotContracts.cmake
# PURPOSE: Integrate reviewed snapshot owners, public headers and consumers.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_snapshot_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set_property(GLOBAL PROPERTY UMICOM_SNAPSHOT_CONTRACT_ROOT "${_snapshot_root}")
# Each entry identifies an existing value-only registry. Specialised stores
# and previously validated recent-items/file-set services keep their own rules.
set(_snapshot_entries
    platform/bookmarks
    platform/file_operation_queue
    platform/resource_location
    platform/workspace_history
    ui/command_history
    ui/command_surface
    ui/context_menu
    ui/dock_model
    ui/drag_drop
    ui/extension_point
    ui/list_model
    ui/navigation_stack
    ui/notification_item
    ui/output_channel
    ui/panel_model
    ui/problem
    ui/progress
    ui/property_inspector
    ui/selection_model
    ui/sort_filter_model
    ui/status_item
    ui/tab_model
    ui/task_monitor
    ui/tree_model
    ui/view_state
    ui/welcome_view
    project/build_node
    project/capability
    project/configuration
    project/dependency
    project/descriptor
    project/environment
    project/launch_profile
    project/reference
    project/target
    project/task
    project/template
    project/variable
    source_control/branch
    source_control/change
    source_control/change_set
    source_control/commit
    source_control/diff_session
    source_control/history_entry
    source_control/operation
    source_control/remote
    source_control/repository
    source_control/staging
    source_control/tag
    debug/breakpoint
    debug/console_entry
    debug/event
    debug/exception
    debug/launch_configuration
    debug/module
    debug/scope
    debug/session
    debug/source
    debug/stack_frame
    debug/thread
    debug/variable
    debug/watch
    frontend/binding
    frontend/render_tree
    frontend/signal
    frontend/transport
    frontend/web_session
    frontend/web_style
    frontend/widget_tree
)
# Additional reviewed value-only owners use the same contract and consumers.
# Existing specialised validation in test discovery/suite remains separate.
list(APPEND _snapshot_entries
    chart/annotation
    chart/crosshair
    chart/drawing
    chart/extension
    chart/marker
    chart/pane
    chart/scale
    chart/stream
    designer/action_binding
    designer/alignment
    designer/clipboard
    designer/property_schema
    designer/signal_binding
    designer/template_palette
    editor/code_action
    editor/completion
    editor/configuration
    editor/cursor
    editor/diagnostic
    editor/diff_hunk
    editor/document
    editor/fold_region
    editor/marker
    editor/selection_range
    editor/symbol
    language/code_action
    language/completion
    language/definition
    language/diagnostic
    language/document
    language/folding_range
    language/formatting
    language/hover
    language/inlay_hint
    language/provider
    language/reference
    language/rename
    language/semantic_token
    language/signature
    language/symbol
    product/installation_state
    product/marketplace
    product/metadata_provider
    product/update_policy
    test_platform/attachment
    test_platform/benchmark
    test_platform/coverage
    test_platform/output
    test_platform/result
    test_platform/run_profile
    test_platform/run_session
)
set_property(GLOBAL PROPERTY UMICOM_SNAPSHOT_CONTRACT_ENTRIES "${_snapshot_entries}")

function(umicom_snapshot_contracts_declare_headers)
    if(NOT COMMAND _umicom_release_declare_pair)
        return()
    endif()
    _umicom_release_declare_pair(umicom_base include/umicom/base/snapshot_validation.h
        src/base/snapshot_validation.c)
    get_property(_entries GLOBAL PROPERTY UMICOM_SNAPSHOT_CONTRACT_ENTRIES)
    foreach(_entry IN LISTS _entries)
        string(REPLACE "/" ";" _parts "${_entry}")
        list(GET _parts 0 _group)
        _umicom_release_declare_pair("umicom_${_group}" "include/umicom/${_entry}.h"
            "src/${_entry}.c")
    endforeach()
endfunction()

set_property(GLOBAL PROPERTY UMICOM_SNAPSHOT_DOCUMENT_CONTRACTS
    editor/code_action
    editor/completion
    editor/diagnostic
    editor/symbol
    language/code_action
    language/completion
    language/diagnostic
    language/folding_range
    language/hover
    language/inlay_hint
    language/reference
    language/semantic_token
    language/signature
    language/symbol
)

if(BUILD_TESTING)
    add_subdirectory("${_snapshot_root}/tests/snapshot_contracts"
        "${CMAKE_CURRENT_BINARY_DIR}/snapshot-contracts")
endif()
include(GNUInstallDirs)
install(DIRECTORY "${_snapshot_root}/examples/snapshot_contracts/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/snapshot_contracts"
    COMPONENT Framework)
