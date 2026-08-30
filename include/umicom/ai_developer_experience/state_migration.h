/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/state_migration.h
 *
 * PURPOSE:
 *   Provide an explicit durable-state migration boundary for future schema
 *   changes instead of silently interpreting newer session data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_STATE_MIGRATION_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_STATE_MIGRATION_H

#include "umicom/ai_developer_experience/state_schema.h"
#include "umicom/platform/session_store.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_state_migrate(
    UmiSessionStore *store,
    const char *key_prefix,
    uint32_t from_version,
    uint32_t to_version,
    int *out_migrated);

#ifdef __cplusplus
}
#endif
#endif
