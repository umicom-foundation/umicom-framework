/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/inline_controller.h
 *
 * PURPOSE:
 *   Own the active inline AI suggestion, request sequence, acceptance conflict
 *   checks and recent outcome history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_INLINE_CONTROLLER_H
#define UMICOM_IDE_INTEGRATION_INLINE_CONTROLLER_H

#include "umicom/ide_integration/edit_adapter.h"
#include "umicom/ide_integration/inline_executor.h"
#include "umicom/ide_integration/inline_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeInlineController {
    UmiIdeInlineExecutor executor;
    UmiIdeEditorEditAdapter edits;
    UmiIdeInlineSuggestion active;
    UmiIdeInlineHistory *history;
    uint64_t next_sequence;
    uint64_t revision;
} UmiIdeInlineController;

UmiStatus umi_ide_inline_controller_init(
    UmiIdeInlineController *controller,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits);

void umi_ide_inline_controller_deinit(UmiIdeInlineController *controller);

UmiStatus umi_ide_inline_controller_request(
    UmiIdeInlineController *controller,
    const UmiIdeEditorSelection *context);

UmiStatus umi_ide_inline_controller_accept(
    UmiIdeInlineController *controller);

UmiStatus umi_ide_inline_controller_reject(
    UmiIdeInlineController *controller);

UmiStatus umi_ide_inline_controller_cancel(
    UmiIdeInlineController *controller);

UmiStatus umi_ide_inline_controller_snapshot(
    const UmiIdeInlineController *controller,
    UmiIdeInlineSuggestion *out_suggestion);

#ifdef __cplusplus
}
#endif
#endif
