/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/presentation_persistence.h
 *
 * PURPOSE:
 *   Persist toolkit-neutral active pane and selection state through Session Store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PRESENTATION_PERSISTENCE_H
#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_developer_experience/presentation_state.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write ai developer presentation in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_ai_developer_presentation_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPresentationState *state);

/**
 * Provide the ai developer presentation restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPresentationState *state,
    int *out_restored);

#ifdef __cplusplus
}
#endif
#endif
