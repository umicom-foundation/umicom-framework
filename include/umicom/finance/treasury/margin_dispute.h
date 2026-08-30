/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_dispute.h
 *
 * PURPOSE:
 *   Track margin dispute amount and resolution state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#define UMICOM_FINANCE_TREASURY_MARGIN_DISPUTE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryMarginDispute {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t disputed_minor;
    int64_t resolved_minor;
} UmiTreasuryMarginDispute;
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor);
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value);
int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value);
#ifdef __cplusplus
}
#endif
#endif
