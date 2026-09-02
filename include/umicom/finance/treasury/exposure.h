/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/exposure.h
 *
 * PURPOSE:
 *   Represent gross and net financial exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_EXPOSURE_H
#define UMICOM_FINANCE_TREASURY_EXPOSURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury exposure data shared with callers of this public contract.
 */
typedef struct UmiTreasuryExposure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t gross_minor;
    int64_t net_minor;
} UmiTreasuryExposure;
/**
 * Initialise treasury exposure from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_treasury_exposure_init(UmiTreasuryExposure *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor);
/**
 * Check that treasury exposure satisfies its contract before another service relies on it.
 */
bool umi_treasury_exposure_valid(const UmiTreasuryExposure *value);
/**
 * Provide the treasury exposure net absolute minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_exposure_net_absolute_minor(const UmiTreasuryExposure *value);
#ifdef __cplusplus
}
#endif
#endif
