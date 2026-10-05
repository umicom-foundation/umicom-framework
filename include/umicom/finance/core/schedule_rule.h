/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/schedule_rule.h
 *
 * PURPOSE:
 *   Define reusable generation rules for accrual and payment schedules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SCHEDULE_RULE_H
#define UMICOM_FINANCE_CORE_SCHEDULE_RULE_H

#include "umicom/finance/core/tenor.h"
#include "umicom/base/value_archive.h"
#include "umicom/finance/core/business_day_convention.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the schedule rule data shared with callers of this public contract.
 */
typedef struct UmiScheduleRule { UmiFinancialDate start; UmiFinancialDate end; UmiTenor frequency; UmiBusinessDayConvention convention; } UmiScheduleRule;
/* Validate a schedule rule. */ bool umi_schedule_rule_is_valid(const UmiScheduleRule *r);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_schedule_rule_archive_encode(const UmiScheduleRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_schedule_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiScheduleRule *value);

#ifdef __cplusplus
}
#endif

#endif
