/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/selection_router.h
 *
 * PURPOSE:
 *   Route domain selections into the existing UmiIdeCommandContext so one
 *   command palette/menu/shortcut execution path understands Problems, Tests,
 *   VCS, symbols, diagnostics, editor selections and AI review selections.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SELECTION_ROUTER_H
#define UMICOM_STUDIO_RUNTIME_SELECTION_ROUTER_H

#include "umicom/ide_integration/command_context.h"
#include "umicom/studio_runtime/selection_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeSelectionRouter {
    UmiStudioRuntimeSelectionState state;
    UmiIdeCommandContext command_context;
    uint64_t revision;
} UmiStudioRuntimeSelectionRouter;

void umi_studio_selection_router_init(
    UmiStudioRuntimeSelectionRouter *router);

UmiStatus umi_studio_selection_router_problem(
    UmiStudioRuntimeSelectionRouter *router,
    size_t problem_index,
    const char *label);

UmiStatus umi_studio_selection_router_test(
    UmiStudioRuntimeSelectionRouter *router,
    const char *test_item_id,
    const char *label);

UmiStatus umi_studio_selection_router_source_control(
    UmiStudioRuntimeSelectionRouter *router,
    const char *path,
    uint32_t line);

UmiStatus umi_studio_selection_router_symbol(
    UmiStudioRuntimeSelectionRouter *router,
    const char *symbol_id,
    const char *label);

UmiStatus umi_studio_selection_router_diagnostic(
    UmiStudioRuntimeSelectionRouter *router,
    size_t diagnostic_index,
    const char *label);

UmiStatus umi_studio_selection_router_debug_frame(
    UmiStudioRuntimeSelectionRouter *router,
    uint64_t frame_id);

UmiStatus umi_studio_selection_router_editor(
    UmiStudioRuntimeSelectionRouter *router,
    const UmiIdeEditorSelection *selection);

UmiStatus umi_studio_selection_router_ai_approval(
    UmiStudioRuntimeSelectionRouter *router,
    const char *approval_id,
    const char *label);

UmiStatus umi_studio_selection_router_ai_patch_file(
    UmiStudioRuntimeSelectionRouter *router,
    const char *patch_id,
    size_t file_index,
    const char *path);

const UmiIdeCommandContext *umi_studio_selection_router_context(
    const UmiStudioRuntimeSelectionRouter *router);

#ifdef __cplusplus
}
#endif
#endif
