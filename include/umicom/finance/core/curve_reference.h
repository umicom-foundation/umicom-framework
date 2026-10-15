/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/curve_reference.h
 *
 * PURPOSE:
 *   Define provider-neutral curve references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_CURVE_REFERENCE_H
#define UMICOM_FINANCE_CORE_CURVE_REFERENCE_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the curve reference data shared with callers of this public contract.
 */
typedef struct UmiCurveReference { UmiFinancialId reference_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiCurveReference;
/* Initialize the typed financial record. */ UmiStatus umi_curve_reference_init(UmiCurveReference *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_curve_reference_is_valid(const UmiCurveReference *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_curve_reference_archive_encode(const UmiCurveReference *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_curve_reference_archive_decode(const void *bytes, size_t byte_count,
    UmiCurveReference *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_curve_reference_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCurveReference *expected, UmiCurveReference *value);


#ifdef __cplusplus
}
#endif

#endif
