/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/quality_measure.h
 *
 * PURPOSE:
 *   Define an inclusive numeric quality requirement such as density or sulphur.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_MEASURE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_QUALITY_MEASURE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity quality measure data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityQualityMeasure {
    char name[UMI_COMMODITY_NAME_CAPACITY];
    char unit_code[UMI_COMMODITY_CODE_CAPACITY];
    int64_t minimum;
    int64_t maximum;
    int32_t scale;
} UmiCommodityQualityMeasure;

/* Initialise a bounded quality measure record for reusable Framework workflows. */
UmiStatus umi_commodity_quality_measure_init(UmiCommodityQualityMeasure *value, const char *name, const char *unit_code, int64_t minimum, int64_t maximum, int32_t scale);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_quality_measure_valid(const UmiCommodityQualityMeasure *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_quality_measure_archive_encode(const UmiCommodityQualityMeasure *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_quality_measure_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityQualityMeasure *value);

#ifdef __cplusplus
}
#endif

#endif
