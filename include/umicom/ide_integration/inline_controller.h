/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/inline_controller.h
 *
 * PURPOSE:
 *   Own the active inline AI suggestion, request sequence, acceptance conflict
 *   checks and recent outcome history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_INLINE_CONTROLLER_H
#define UMICOM_IDE_INTEGRATION_INLINE_CONTROLLER_H

#include "umicom/ide_integration/edit_adapter.h"
#include "umicom/ide_integration/inline_executor.h"
#include "umicom/ide_integration/inline_history.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide inline controller data shared with callers of this public contract.
 */
typedef struct UmiIdeInlineController {
    UmiIdeInlineExecutor executor;
    UmiIdeEditorEditAdapter edits;
    UmiIdeInlineSuggestion active;
    UmiIdeInlineHistory *history;
    uint64_t next_sequence;
    uint64_t revision;
} UmiIdeInlineController;

/**
 * Initialise ide inline controller from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_inline_controller_init(
    UmiIdeInlineController *controller,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits);

/**
 * Provide the ide inline controller deinit operation used by this module and its client
 * applications.
 */
void umi_ide_inline_controller_deinit(UmiIdeInlineController *controller);

/**
 * Provide the ide inline controller request operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_request(
    UmiIdeInlineController *controller,
    const UmiIdeEditorSelection *context);

/**
 * Provide the ide inline controller accept operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_accept(
    UmiIdeInlineController *controller);

/**
 * Provide the ide inline controller reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_reject(
    UmiIdeInlineController *controller);

/**
 * Provide the ide inline controller cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_cancel(
    UmiIdeInlineController *controller);

/**
 * Provide the ide inline controller snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_snapshot(
    const UmiIdeInlineController *controller,
    UmiIdeInlineSuggestion *out_suggestion);

#ifdef __cplusplus
}
#endif
#endif
