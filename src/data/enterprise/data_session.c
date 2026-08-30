/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_session.c
 *
 * PURPOSE:
 *   Describe a caller Data Server session with routing/transaction context and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_session.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_session_init(UmiDataSession *item, const char *session_id, const char *principal_id, UmiDataConsistency consistency, uint64_t started_at) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->session_id,sizeof(item->session_id),session_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->principal_id,sizeof(item->principal_id),principal_id);if(s!=UMI_STATUS_OK)return s;item->consistency=consistency;item->started_at=started_at;item->last_activity=started_at;item->transaction_open=false;
    return umi_data_data_session_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_session_validate(const UmiDataSession *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->session_id[0] != '\0' && item->principal_id[0] != '\0' && item->consistency >= UMI_DATA_CONSISTENCY_EVENTUAL && item->consistency <= UMI_DATA_CONSISTENCY_STRONG)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
