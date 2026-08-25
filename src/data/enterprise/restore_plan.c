/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/restore_plan.c
 *
 * PURPOSE:
 *   Describe a restore target and safety gates before destructive data replacement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/restore_plan.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_restore_plan_init(UmiDataRestorePlan *item, const char *restore_id, const char *backup_id, uint64_t expected_schema_fingerprint, bool verify_only) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->restore_id,sizeof(item->restore_id),restore_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->backup_id,sizeof(item->backup_id),backup_id);if(s!=UMI_STATUS_OK)return s;item->expected_schema_fingerprint=expected_schema_fingerprint;item->verify_only=verify_only;item->preserve_existing=true;item->approved=false;
    return umi_data_restore_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_restore_plan_validate(const UmiDataRestorePlan *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->restore_id[0] != '\0' && item->backup_id[0] != '\0' && item->expected_schema_fingerprint != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
