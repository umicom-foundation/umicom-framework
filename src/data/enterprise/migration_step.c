/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_step.c
 *
 * PURPOSE:
 *   Describe one reversible or irreversible schema/data migration operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_step.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_migration_step_init(UmiDataMigrationStep *item, const char *step_id, const char *description, uint32_t ordinal, bool reversible, bool destructive) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->step_id,sizeof(item->step_id),step_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->description,sizeof(item->description),description);if(s!=UMI_STATUS_OK)return s;item->ordinal=ordinal;item->reversible=reversible;item->destructive=destructive;
    return umi_data_migration_step_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_migration_step_validate(const UmiDataMigrationStep *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->step_id[0] != '\0' && item->description[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
