/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/state_migration.c
 *
 * PURPOSE:
 *   Define explicit schema migration behavior. Version 1 requires no migration;
 *   future versions must add deliberate transforms here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/state_migration.h"

UmiStatus umi_ai_developer_state_migrate(
    UmiSessionStore *store,
    const char *key_prefix,
    uint32_t from_version,
    uint32_t to_version,
    int *out_migrated)
{
    if (store == NULL || key_prefix == NULL ||
        out_migrated == NULL ||
        from_version == 0U ||
        to_version == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_migrated = 0;

    if (from_version == to_version) return UMI_STATUS_OK;

    /*
     * No historical schema exists before version 1. A future schema increment
     * must implement a bounded, tested transform rather than guessing.
     */
    return UMI_STATUS_NOT_IMPLEMENTED;
}
