/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/confirmation_matching.c
 *
 * PURPOSE:
 *   Implement score confirmation field matching and expose exact-match status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation_matching.h"
#include <string.h>
UmiStatus umi_treasury_confirmation_matching_init(UmiTreasuryConfirmationMatching *value,
    const char *id,
    uint32_t matched_fields,
    uint32_t total_fields) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->matched_fields=matched_fields;
    value->total_fields=total_fields;
    return umi_treasury_confirmation_matching_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_confirmation_matching_valid(const UmiTreasuryConfirmationMatching *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->total_fields > 0U && value->matched_fields <= value->total_fields);
}

bool umi_treasury_confirmation_matching_exact(const UmiTreasuryConfirmationMatching *value) {
    if (value == NULL) return (bool)0;
    return value->matched_fields == value->total_fields;
}
