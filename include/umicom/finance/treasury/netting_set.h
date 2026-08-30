/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/netting_set.h
 *
 * PURPOSE:
 *   Define gross receivables and payables within a legally enforceable netting set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_NETTING_SET_H
#define UMICOM_FINANCE_TREASURY_NETTING_SET_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryNettingSet {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t receivable_minor;
    int64_t payable_minor;
} UmiTreasuryNettingSet;
UmiStatus umi_treasury_netting_set_init(UmiTreasuryNettingSet *value,
    const char *id,
    int64_t receivable_minor,
    int64_t payable_minor);
bool umi_treasury_netting_set_valid(const UmiTreasuryNettingSet *value);
int64_t umi_treasury_netting_set_net_minor(const UmiTreasuryNettingSet *value);
#ifdef __cplusplus
}
#endif
#endif
