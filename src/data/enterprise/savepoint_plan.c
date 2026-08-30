/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/savepoint_plan.c
 *
 * PURPOSE:
 *   Describe explicit savepoints for backend adapters that support nested recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/savepoint_plan.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_savepoint_plan_init(UmiDataSavepointPlan *item, const char *savepoint_id, uint32_t ordinal) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->savepoint_id,sizeof(item->savepoint_id),savepoint_id);if(s!=UMI_STATUS_OK)return s;item->ordinal=ordinal;item->release_on_success=true;item->rollback_on_failure=true;
    return umi_data_savepoint_plan_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_savepoint_plan_validate(const UmiDataSavepointPlan *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->savepoint_id[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
