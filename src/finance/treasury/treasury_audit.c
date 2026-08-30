/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_audit.c
 *
 * PURPOSE:
 *   Implement record treasury audit evidence with actor and monotonically increasing sequence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_audit.h"
#include <string.h>
UmiStatus umi_treasury_treasury_audit_init(UmiTreasuryTreasuryAudit *value,
    const char *id,
    const char *actor_id,
    uint64_t sequence) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    status=umi_treasury_id_copy(value->actor_id,sizeof value->actor_id,actor_id);
    if(status!=UMI_STATUS_OK)return status;
    value->sequence=sequence;
    return umi_treasury_treasury_audit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_treasury_audit_valid(const UmiTreasuryTreasuryAudit *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && umi_treasury_id_valid(value->actor_id) && value->sequence > 0U);
}

bool umi_treasury_treasury_audit_sequenced(const UmiTreasuryTreasuryAudit *value) {
    if (value == NULL) return (bool)0;
    return value->sequence > 0U;
}
