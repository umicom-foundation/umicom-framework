/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_server_profile.c
 *
 * PURPOSE:
 *   Describe logical Data Server operating limits and consistency defaults for deployment profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_server_profile.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_server_profile_init(UmiDataServerProfile *item, const char *profile_id, size_t minimum_pool_size, size_t maximum_pool_size, uint64_t query_row_limit) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->profile_id,sizeof(item->profile_id),profile_id);if(s!=UMI_STATUS_OK)return s;item->minimum_pool_size=minimum_pool_size;item->maximum_pool_size=maximum_pool_size;item->query_row_limit=query_row_limit;item->default_consistency=UMI_DATA_CONSISTENCY_SESSION;item->migrations_enabled=true;
    return umi_data_data_server_profile_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_server_profile_validate(const UmiDataServerProfile *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->profile_id[0] != '\0' && item->minimum_pool_size <= item->maximum_pool_size && item->maximum_pool_size <= UMI_DATA_ENTERPRISE_MAX_ITEMS && item->query_row_limit > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
