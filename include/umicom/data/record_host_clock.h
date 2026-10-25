/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_host_clock.h
 *
 * PURPOSE:
 *   Read trusted time after admission instead of reusing a queued request time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Read trusted time after admission instead of reusing a queued request time. */
#ifndef UMICOM_DATA_RECORD_HOST_CLOCK_H
#define UMICOM_DATA_RECORD_HOST_CLOCK_H
#include "umicom/data/record_host.h"
#include "umicom/platform/clock.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Ordinary host creation uses the system monotonic clock for current-time
 * methods. This factory copies a trusted clock descriptor for controlled hosts
 * and deterministic testing. Its state is borrowed until every host reference
 * is released; the host never disposes it. The monotonic callback must be
 * thread-safe, bounded, and must not call back into the same host.
 * Current-time methods invoke this trusted callback while the host lock is held.
 * Existing explicit-time APIs retain their caller-managed clock contract. */
UmiStatus umi_record_host_create_with_clock(const UmiRecordHostOptions *options,
                                            const UmiClock *clock, UmiRecordHost **out);
UmiStatus umi_record_host_issue_current_session(UmiRecordHost *host, const char *identity_id,
                                                uint64_t lifetime_nanoseconds,
                                                UmiSessionRecord *out);
/* Replace request.now_nanoseconds with the host clock after acquiring its lock.
 * Do not mix sessions issued in another clock domain. A zero clock reading
 * returns UNAVAILABLE without publishing output. Backward readings are refused
 * by session validation. Retired hosts do not invoke the clock.
 * Receipts and recorded timestamps use this same clock domain; monotonic values
 * are not UTC timestamps and must not be rendered as calendar dates. */
UmiStatus umi_record_host_latest_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                         UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_load_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                       uint64_t revision, UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_open_reviewed_current(UmiRecordHost *host,
                                                const UmiRecordAccessRequest *request,
                                                const UmiRecordRevision *reviewed,
                                                UmiAuthorizedRecordDocument *out);
UmiStatus umi_record_host_append_current(UmiRecordHost *host, const UmiRecordAccessRequest *request,
                                         const char *reason, uint64_t observed_at_nanoseconds,
                                         const void *content, size_t length,
                                         uint64_t expected_revision,
                                         UmiAuthorizedRecordRevision *out);
UmiStatus umi_record_host_form_load_current(UmiRecordHost *host,
                                            const UmiRecordAccessRequest *request,
                                            uint64_t revision,
                                            const UmiStructuredFormSchema *schema,
                                            UmiAuthorizedFormRecord *out);
UmiStatus umi_record_host_form_open_reviewed_current(UmiRecordHost *host,
                                                     const UmiRecordAccessRequest *request,
                                                     const UmiRecordRevision *reviewed,
                                                     const UmiStructuredFormSchema *schema,
                                                     UmiAuthorizedFormRecord *out);
UmiStatus umi_record_host_form_save_current(
    UmiRecordHost *host, const UmiRecordAccessRequest *request,
    const UmiStructuredFormSchema *schema, const UmiStructuredForm *form, const char *reason,
    uint64_t observed_at_nanoseconds, uint64_t expected_revision, UmiAuthorizedRecordRevision *out);
#ifdef __cplusplus
}
#endif
#endif
