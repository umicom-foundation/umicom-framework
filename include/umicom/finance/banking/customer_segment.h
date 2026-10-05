/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/customer_segment.h
 *
 * PURPOSE:
 *   Assign a reusable banking customer segment for product and service policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_CUSTOMER_SEGMENT_H
#define UMICOM_FINANCE_BANKING_CUSTOMER_SEGMENT_H
#include "umicom/finance/banking/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking customer segment data shared with callers of this public contract.
 */
typedef struct UmiBankingCustomerSegment {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    UmiBankingSegment segment;
} UmiBankingCustomerSegment;
/**
 * Initialise banking customer segment from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_customer_segment_init(UmiBankingCustomerSegment *value,
    const char *id,
    const char *customer_id,
    UmiBankingSegment segment);
/**
 * Check that banking customer segment satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_customer_segment_valid(const UmiBankingCustomerSegment *value);
/**
 * Provide the banking customer segment institutional operation used by this module and its
 * client applications.
 */
bool umi_banking_customer_segment_institutional(const UmiBankingCustomerSegment *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_banking_customer_segment_archive_encode(const UmiBankingCustomerSegment *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_banking_customer_segment_archive_decode(const void *bytes, size_t byte_count,
    UmiBankingCustomerSegment *value);

#ifdef __cplusplus
}
#endif
#endif
