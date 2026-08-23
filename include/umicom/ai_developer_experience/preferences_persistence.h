/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/preferences_persistence.h
 *
 * PURPOSE:
 *   Persist AI developer presentation preferences using UmiSessionStore.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_PERSISTENCE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PREFERENCES_PERSISTENCE_H
#include "umicom/ai_developer_experience/persistence_codec.h"
#include "umicom/ai_developer_experience/preferences.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_preferences_save(
    UmiSessionStore *store,
    const char *key_prefix,
    const UmiAiDeveloperPreferences *preferences);

UmiStatus umi_ai_developer_preferences_restore(
    const UmiSessionStore *store,
    const char *key_prefix,
    UmiAiDeveloperPreferences *preferences,
    int *out_restored);

#ifdef __cplusplus
}
#endif
#endif
