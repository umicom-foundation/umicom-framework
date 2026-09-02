/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/views/contract.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Studio Contract view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_CONTRACT_H
#define UMICOM_STUDIO_RUNTIME_VIEW_CONTRACT_H
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise studio contract view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_studio_contract_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
