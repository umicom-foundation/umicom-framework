/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/backup_plan.c
 *
 * PURPOSE:
 *   Describe a reviewable full/incremental backup request and retention class.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/backup_plan.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_backup_plan_init(UmiDataBackupPlan *item, const char *backup_id, const char *destination, uint64_t schema_fingerprint, bool incremental) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->destination,sizeof(item->destination),destination);if(s!=UMI_STATUS_OK)return s;item->schema_fingerprint=schema_fingerprint;item->incremental=incremental;item->include_blobs=true;item->encrypted=true;
    return umi_data_backup_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_backup_plan_validate(const UmiDataBackupPlan *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->backup_id[0] != '\0' && item->destination[0] != '\0' && item->schema_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
