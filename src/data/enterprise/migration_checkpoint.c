/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_checkpoint.c
 *
 * PURPOSE:
 *   Record resumable migration position and pre/post schema fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_checkpoint.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_migration_checkpoint_init(UmiDataMigrationCheckpoint *item, const char *checkpoint_id, const char *migration_id, size_t completed_steps, uint64_t source_fingerprint, uint64_t current_fingerprint) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->checkpoint_id,sizeof(item->checkpoint_id),checkpoint_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->migration_id,sizeof(item->migration_id),migration_id);if(s!=UMI_STATUS_OK)return s;item->completed_steps=completed_steps;item->source_fingerprint=source_fingerprint;item->current_fingerprint=current_fingerprint;item->committed=false;
    return umi_data_migration_checkpoint_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_migration_checkpoint_validate(const UmiDataMigrationCheckpoint *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->checkpoint_id[0] != '\0' && item->migration_id[0] != '\0' && item->source_fingerprint != 0U && item->current_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
