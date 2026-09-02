/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/status_model.h
 *
 * PURPOSE:
 *   Build a professional Studio status-bar model from authoritative IDE context
 *   and current editor selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_STATUS_MODEL_H
#define UMICOM_STUDIO_RUNTIME_STATUS_MODEL_H

#include "umicom/ide_integration/platform.h"
#include "umicom/studio_runtime/selection_router.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the studio runtime status model data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeStatusModel {
    UmiStudioRuntimeStatusItem items[UMI_STUDIO_RUNTIME_STATUS_ITEM_CAPACITY];
    size_t item_count;
    uint64_t revision;
} UmiStudioRuntimeStatusModel;

/**
 * Initialise studio status model from caller-provided values so later operations receive a
 * known state.
 */
void umi_studio_status_model_init(UmiStudioRuntimeStatusModel *model);

/**
 * Provide the studio status model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_status_model_build(
    UmiStudioRuntimeStatusModel *model,
    const UmiIdeIntegrationPlatformSnapshot *snapshot,
    const UmiStudioRuntimeSelectionRouter *selection);

/**
 * Find studio status model while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiStudioRuntimeStatusItem *umi_studio_status_model_find(
    const UmiStudioRuntimeStatusModel *model,
    UmiStudioRuntimeStatusKind kind);

#ifdef __cplusplus
}
#endif
#endif
