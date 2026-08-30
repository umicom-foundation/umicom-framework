/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/connection_slot.c
 *
 * PURPOSE:
 *   Describe one pooled backend connection lease slot without owning the backend handle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/connection_slot.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_connection_slot_init(UmiDataConnectionSlot *item, const char *slot_id, uint64_t connection_token) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->slot_id,sizeof(item->slot_id),slot_id);if(s!=UMI_STATUS_OK)return s;item->connection_token=connection_token;item->healthy=true;item->leased=false;
    return umi_data_connection_slot_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_connection_slot_validate(const UmiDataConnectionSlot *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->slot_id[0] != '\0' && item->connection_token != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
