/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_join.c
 *
 * PURPOSE:
 *   Describe backend-neutral joins for cost analysis and SQL generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_join.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_join_init(UmiDataQueryJoin *item, const char *join_id, const char *left_table, const char *right_table, const char *condition, bool outer_join) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->join_id,sizeof(item->join_id),join_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->left_table,sizeof(item->left_table),left_table);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->right_table,sizeof(item->right_table),right_table);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->condition,sizeof(item->condition),condition);if(s!=UMI_STATUS_OK)return s;item->outer_join=outer_join;
    return umi_data_query_join_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_join_validate(const UmiDataQueryJoin *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->join_id[0] != '\0' && item->left_table[0] != '\0' && item->right_table[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
