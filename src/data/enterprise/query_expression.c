/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_expression.c
 *
 * PURPOSE:
 *   Represent a portable query expression node for later backend translation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_expression.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_expression_init(UmiDataQueryExpression *item, const char *expression_id, const char *field, const char *operation, const char *value) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->expression_id,sizeof(item->expression_id),expression_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field,sizeof(item->field),field);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation,sizeof(item->operation),operation);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->value,sizeof(item->value),value);if(s!=UMI_STATUS_OK)return s;item->parameterized=true;
    return umi_data_query_expression_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_expression_validate(const UmiDataQueryExpression *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->expression_id[0] != '\0' && item->field[0] != '\0' && item->operation[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
