/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/ai_context_sync.h
 *
 * PURPOSE:
 *   Translate Studio cross-domain selection into the AI Developer Experience
 *   command context, including approval/patch selection and host actor identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_AI_CONTEXT_SYNC_H
#define UMICOM_STUDIO_RUNTIME_AI_CONTEXT_SYNC_H

#include "umicom/ai_developer_experience/command_context.h"
#include "umicom/studio_runtime/selection_router.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_ai_context_sync(
    const UmiStudioRuntimeSelectionRouter *selection,
    const char *actor_id,
    UmiAiDeveloperCommandContext *out_context);

#ifdef __cplusplus
}
#endif
#endif
