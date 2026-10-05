/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/counterparty_exposure.h
 *
 * PURPOSE:
 *   Represent counterparty current and potential future exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COUNTERPARTY_EXPOSURE_H
#define UMICOM_FINANCE_TREASURY_COUNTERPARTY_EXPOSURE_H
#include "umicom/finance/treasury/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury counterparty exposure data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCounterpartyExposure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t current_minor;
    int64_t potential_future_minor;
} UmiTreasuryCounterpartyExposure;
/**
 * Initialise treasury counterparty exposure from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_counterparty_exposure_init(UmiTreasuryCounterpartyExposure *value,
    const char *id,
    int64_t current_minor,
    int64_t potential_future_minor);
/**
 * Check that treasury counterparty exposure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_counterparty_exposure_valid(const UmiTreasuryCounterpartyExposure *value);
/**
 * Provide the treasury counterparty exposure total minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_counterparty_exposure_total_minor(const UmiTreasuryCounterpartyExposure *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_counterparty_exposure_archive_encode(const UmiTreasuryCounterpartyExposure *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_counterparty_exposure_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCounterpartyExposure *value);

#ifdef __cplusplus
}
#endif
#endif
