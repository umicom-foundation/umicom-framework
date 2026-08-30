/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_index.c
 *
 * PURPOSE:
 *   Describe an index and its uniqueness/coverage characteristics for compatibility and planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_index.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_schema_index_init(UmiDataSchemaIndex *item, const char *index_id, const char *table_id, const char *key_expression, bool unique) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->index_id,sizeof(item->index_id),index_id); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->table_id,sizeof(item->table_id),table_id); if(s!=UMI_STATUS_OK)return s; s=umi_data_enterprise_copy_text(item->key_expression,sizeof(item->key_expression),key_expression); if(s!=UMI_STATUS_OK)return s; item->unique=unique; item->covering=false;
    return umi_data_schema_index_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_schema_index_validate(const UmiDataSchemaIndex *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->index_id[0] != '\0' && item->table_id[0] != '\0' && item->key_expression[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
