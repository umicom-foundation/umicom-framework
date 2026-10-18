/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/grade_specification.h
 *
 * PURPOSE:
 *   Define a named quality grade tied to a canonical commodity.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_GRADE_SPECIFICATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_GRADE_SPECIFICATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity grade specification data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityGradeSpecification {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    char grade_code[UMI_COMMODITY_CODE_CAPACITY];
    bool active;
} UmiCommodityGradeSpecification;

/* Initialise a bounded grade specification record for reusable Framework workflows. */
UmiStatus umi_commodity_grade_specification_init(UmiCommodityGradeSpecification *value, const char *id, const char *commodity_id, const char *grade_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_grade_specification_valid(const UmiCommodityGradeSpecification *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_grade_specification_archive_encode(const UmiCommodityGradeSpecification *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_grade_specification_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityGradeSpecification *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCommodityGradeSpecification object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_commodity_grade_specification_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCommodityGradeSpecification *workspace, UmiCommodityGradeSpecification *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_commodity_grade_specification_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCommodityGradeSpecification *expected, UmiCommodityGradeSpecification *value);


#ifdef __cplusplus
}
#endif

#endif
