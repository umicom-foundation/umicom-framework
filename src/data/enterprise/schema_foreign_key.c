/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_foreign_key.c
 *
 * PURPOSE:
 *   Describe referential constraints in a backend-neutral form for migration ordering and ORM relations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_foreign_key.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_foreign_key_init(UmiDataSchemaForeignKey *item, const char *constraint_id, const char *source_table, const char *source_column, const char *target_table, const char *target_column) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->constraint_id,sizeof(item->constraint_id),constraint_id); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->source_table,sizeof(item->source_table),source_table); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->source_column,sizeof(item->source_column),source_column); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->target_table,sizeof(item->target_table),target_table); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->target_column,sizeof(item->target_column),target_column); if(s!=UMI_STATUS_OK)return s; item->cascade_delete=false;
    return umi_data_schema_foreign_key_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_foreign_key_validate(const UmiDataSchemaForeignKey *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->constraint_id[0] != '\0' && item->source_table[0] != '\0' && item->target_table[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
