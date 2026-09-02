/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/preferences_persistence.h
 *
 * PURPOSE:
 *   Persist AI developer presentation preferences using UmiSessionStore.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_PERSISTENCE_H
#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_developer_experience/preferences.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Write ai developer preferences in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ai_developer_preferences_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPreferences *preferences);

/**
 * Provide the ai developer preferences restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_preferences_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPreferences *preferences,
    int *out_restored);

#ifdef __cplusplus
}
#endif
#endif
