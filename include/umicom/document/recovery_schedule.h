/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/recovery_schedule.h
 * PURPOSE: Schedule changed recovery drafts without marking a snapshot saved before storage succeeds.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_RECOVERY_SCHEDULE_H
#define UMICOM_DOCUMENT_RECOVERY_SCHEDULE_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_RECOVERY_INTERVAL_MINIMUM_MS UINT64_C(1000)
#define UMI_DOCUMENT_RECOVERY_INTERVAL_MAXIMUM_MS UINT64_C(3600000)
    typedef struct UmiDocumentRecoverySchedule UmiDocumentRecoverySchedule;
    typedef struct UmiDocumentRecoveryObservation
    {
        UmiDocumentId document_id;
        uint64_t text_revision, store_revision;
        int dirty;
    } UmiDocumentRecoveryObservation;
    typedef struct UmiDocumentRecoveryTicket
    {
        const UmiDocumentRecoverySchedule *owner;
        uint64_t sequence;
        UmiDocumentRecoveryObservation captured;
    } UmiDocumentRecoveryTicket;
    typedef struct UmiDocumentRecoveryScheduleInfo
    {
        size_t documents;
        uint64_t interval_ms, saved_snapshots;
        int enabled, pending, paused_after_failure;
        UmiStatus last_status;
    } UmiDocumentRecoveryScheduleInfo;
    /* This owner-thread service performs no I/O and starts no timer. It remembers
 * successful source identities, not source text. Feed monotonically increasing
 * millisecond timestamps from one clock, and document IDs from one coordinator
 * lifetime. Disabling never cancels a write already started by a caller. */
    UmiStatus UmiDocumentRecoveryScheduleCreate(UmiDocumentRecoverySchedule **out_schedule);
    void UmiDocumentRecoveryScheduleDestroy(UmiDocumentRecoverySchedule *schedule);
    /* Explicitly enable, disable or resume after failure. Each enabled document
 * waits one interval after this call. Invalid time/interval changes nothing. */
    UmiStatus UmiDocumentRecoveryScheduleConfigure(UmiDocumentRecoverySchedule *schedule, int enabled,
                                                   uint64_t interval_ms, uint64_t now_ms);
    /* Replace the complete observed set, preserving successful identities of
 * surviving IDs. Reject duplicates, zero revisions, invalid flags or excess
 * capacity before changing state. Closed documents disappear from tracking;
 * a pending ticket still completes safely after its document closes. */
    UmiStatus UmiDocumentRecoveryScheduleObserve(UmiDocumentRecoverySchedule *schedule,
                                                 const UmiDocumentRecoveryObservation *documents,
                                                 size_t count, uint64_t now_ms);
    /* Reserve one due changed dirty document in fair rotation. Only one ticket can
 * be pending. NOT_FOUND means no work is due; UNAVAILABLE means disabled or
 * paused after a storage failure. Failure clears out_ticket. Capture source
 * immediately on the same owning thread, then give copied source to a worker.
 * Tickets borrow their schedule identity until Finish; do not outlive it. */
    UmiStatus UmiDocumentRecoveryScheduleBegin(UmiDocumentRecoverySchedule *schedule, uint64_t now_ms,
                                               UmiDocumentRecoveryTicket *out_ticket);
    /* Acknowledge this exact pending ticket with the real storage result. Only OK
 * records its captured revisions as saved. Newer observed revisions remain due
 * after the interval. Any failure pauses future work until an explicit enable;
 * it must not silently retry or claim that unsaved source has been protected.
 * A forged, stale or other-owner ticket changes nothing. */
    UmiStatus UmiDocumentRecoveryScheduleFinish(UmiDocumentRecoverySchedule *schedule,
                                                const UmiDocumentRecoveryTicket *ticket, UmiStatus result,
                                                uint64_t now_ms);
    /* Pause before a ticket exists, for example when observation or capture
 * preparation fails. A pending ticket must instead Finish with its result. */
    UmiStatus UmiDocumentRecoveryScheduleSuspend(UmiDocumentRecoverySchedule *schedule, UmiStatus reason,
                                                 uint64_t now_ms);
    UmiStatus UmiDocumentRecoveryScheduleInspect(const UmiDocumentRecoverySchedule *schedule,
                                                 UmiDocumentRecoveryScheduleInfo *out_info);
    /* Collect complete current observations without synchronizing or saving. The
 * owner uses its existing pending-text cache, so unchanged drafts are not copied
 * repeatedly. Output is all-or-nothing. No I/O occurs and cursor movement alone
 * does not create another recovery candidate. */
    UmiStatus UmiDocumentCoordinatorObserveRecovery(UmiDocumentCoordinator *coordinator,
                                                    UmiDocumentRecoveryObservation *out_documents,
                                                    size_t capacity, size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
