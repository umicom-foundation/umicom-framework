/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_projection.c
 *
 * PURPOSE:
 *   Describe one selected field/alias for portable query result shapes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_projection.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_projection_init(UmiDataQueryProjection *item, const char *projection_id, const char *field, const char *alias) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->projection_id,sizeof(item->projection_id),projection_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->field,sizeof(item->field),field);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->alias,sizeof(item->alias),alias);if(s!=UMI_STATUS_OK)return s;item->hidden=false;
    return umi_data_query_projection_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_projection_validate(const UmiDataQueryProjection *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->projection_id[0] != '\0' && item->field[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
