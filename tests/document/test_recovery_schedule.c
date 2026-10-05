/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_schedule.c
 * PURPOSE: Check periodic recovery timing and acknowledgements without using wall-clock waits or writing files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_schedule.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"disabled",
                                            "interval",
                                            "due",
                                            "acknowledged",
                                            "unchanged",
                                            "newer-text",
                                            "newer-store",
                                            "clean",
                                            "fairness",
                                            "failed-write",
                                            "resume",
                                            "close-pending",
                                            "disable-pending",
                                            "stale-ticket",
                                            "forged-ticket",
                                            "other-owner",
                                            "backward-clock",
                                            "overflow-time",
                                            "invalid-interval",
                                            "invalid-flag",
                                            "duplicate",
                                            "zero-id",
                                            "zero-revision",
                                            "invalid-dirty",
                                            "capacity",
                                            "null-observe",
                                            "empty-set",
                                            "removed-before-begin",
                                            "reorder",
                                            "pending-busy",
                                            "null-output",
                                            "suspend",
                                            "suspend-busy",
                                            "suspend-ok"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiDocumentRecoverySchedule *schedule = NULL, *other = NULL;
    CHECK(UmiDocumentRecoveryScheduleCreate(&schedule) == UMI_STATUS_OK);
    UmiDocumentRecoveryTicket ticket = {0}, next = {0};
    UmiDocumentRecoveryObservation observations[2] = {{1U, 1U, 1U, 1}, {2U, 1U, 1U, 1}};
    CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, 2U, 100U) == UMI_STATUS_OK);
    if (strcmp(mode, "disabled") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 60100U, &ticket) == UMI_STATUS_UNAVAILABLE &&
              ticket.sequence == 0U);
        goto done;
    }
    CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 1, 1000U, 100U) == UMI_STATUS_OK);
    if (strcmp(mode, "interval") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1099U, &ticket) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    UmiStatus invalid = UMI_STATUS_OK;
    if (strcmp(mode, "backward-clock") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 99U, &ticket) == UMI_STATUS_INVALID_ARGUMENT);
        goto inspect;
    }
    if (strcmp(mode, "overflow-time") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 1, 1000U, UINT64_MAX) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        goto inspect;
    }
    if (strcmp(mode, "invalid-interval") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 1, 999U, 100U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 1,
                                                   UMI_DOCUMENT_RECOVERY_INTERVAL_MAXIMUM_MS + 1U,
                                                   100U) == UMI_STATUS_INVALID_ARGUMENT);
        goto inspect;
    }
    if (strcmp(mode, "invalid-flag") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 2, 1000U, 100U) == UMI_STATUS_INVALID_ARGUMENT);
        goto inspect;
    }
    if (strcmp(mode, "duplicate") == 0)
    {
        observations[1].document_id = 1U;
        invalid = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "zero-id") == 0)
    {
        observations[1].document_id = 0U;
        invalid = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "zero-revision") == 0)
    {
        observations[1].text_revision = 0U;
        invalid = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-dirty") == 0)
    {
        observations[1].dirty = 2;
        invalid = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (invalid != UMI_STATUS_OK)
    {
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, 2U, 100U) == invalid);
        goto inspect;
    }
    if (strcmp(mode, "capacity") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, UMI_DOCUMENT_MAX_WORKING_COPIES + 1U,
                                                 100U) == UMI_STATUS_CAPACITY_EXCEEDED);
        goto inspect;
    }
    if (strcmp(mode, "null-observe") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, NULL, 1U, 100U) == UMI_STATUS_INVALID_ARGUMENT);
        goto inspect;
    }
    if (strcmp(mode, "empty-set") == 0 || strcmp(mode, "removed-before-begin") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, NULL, 0U, 1000U) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &ticket) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    if (strcmp(mode, "clean") == 0)
    {
        observations[0].dirty = observations[1].dirty = 0;
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, 2U, 100U) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &ticket) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "suspend") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleSuspend(schedule, UMI_STATUS_OUT_OF_MEMORY, 1100U) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &ticket) == UMI_STATUS_UNAVAILABLE);
        goto done;
    }
    if (strcmp(mode, "suspend-ok") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleSuspend(schedule, UMI_STATUS_OK, 1100U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &ticket) == UMI_STATUS_OK &&
          ticket.captured.document_id == 1U);
    if (strcmp(mode, "due") == 0)
        goto done;
    if (strcmp(mode, "suspend-busy") == 0)
        CHECK(UmiDocumentRecoveryScheduleSuspend(schedule, UMI_STATUS_IO_ERROR, 1100U) == UMI_STATUS_BUSY);
    if (strcmp(mode, "pending-busy") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &next) == UMI_STATUS_BUSY &&
              next.sequence == 0U);
        goto done;
    }
    if (strcmp(mode, "forged-ticket") == 0)
    {
        next = ticket;
        ++next.captured.text_revision;
        CHECK(UmiDocumentRecoveryScheduleFinish(schedule, &next, UMI_STATUS_OK, 1100U) ==
              UMI_STATUS_INVALID_STATE);
    }
    if (strcmp(mode, "other-owner") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleCreate(&other) == UMI_STATUS_OK);
        CHECK(UmiDocumentRecoveryScheduleFinish(other, &ticket, UMI_STATUS_OK, 1100U) ==
              UMI_STATUS_INVALID_STATE);
    }
    if (strcmp(mode, "newer-text") == 0 || strcmp(mode, "newer-store") == 0)
    {
        if (strcmp(mode, "newer-text") == 0)
            ++observations[0].text_revision;
        else
            ++observations[0].store_revision;
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, 2U, 1100U) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "close-pending") == 0)
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations + 1U, 1U, 1100U) == UMI_STATUS_OK);
    if (strcmp(mode, "disable-pending") == 0)
        CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 0, 1000U, 1100U) == UMI_STATUS_OK);
    UmiStatus result = (strcmp(mode, "failed-write") == 0 || strcmp(mode, "resume") == 0)
                           ? UMI_STATUS_IO_ERROR
                           : UMI_STATUS_OK;
    CHECK(UmiDocumentRecoveryScheduleFinish(schedule, &ticket, result, 1100U) == UMI_STATUS_OK);
    if (strcmp(mode, "stale-ticket") == 0)
        CHECK(UmiDocumentRecoveryScheduleFinish(schedule, &ticket, UMI_STATUS_OK, 1100U) ==
              UMI_STATUS_INVALID_STATE);
    if (strcmp(mode, "failed-write") == 0 || strcmp(mode, "resume") == 0)
    {
        UmiDocumentRecoveryScheduleInfo info;
        CHECK(UmiDocumentRecoveryScheduleInspect(schedule, &info) == UMI_STATUS_OK &&
              info.saved_snapshots == 0U && info.paused_after_failure &&
              info.last_status == UMI_STATUS_IO_ERROR);
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &next) == UMI_STATUS_UNAVAILABLE);
        if (strcmp(mode, "resume") == 0)
        {
            CHECK(UmiDocumentRecoveryScheduleConfigure(schedule, 1, 1000U, 1200U) == UMI_STATUS_OK);
            CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 2199U, &next) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 2200U, &next) == UMI_STATUS_OK);
        }
        goto done;
    }
    if (strcmp(mode, "disable-pending") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &next) == UMI_STATUS_UNAVAILABLE);
        goto done;
    }
    if (strcmp(mode, "reorder") == 0)
    {
        UmiDocumentRecoveryObservation swap = observations[0];
        observations[0] = observations[1];
        observations[1] = swap;
        CHECK(UmiDocumentRecoveryScheduleObserve(schedule, observations, 2U, 1100U) == UMI_STATUS_OK);
    }
    CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 1100U, &next) == UMI_STATUS_OK &&
          next.captured.document_id == 2U);
    CHECK(UmiDocumentRecoveryScheduleFinish(schedule, &next, UMI_STATUS_OK, 1100U) == UMI_STATUS_OK);
    if (strcmp(mode, "newer-text") == 0 || strcmp(mode, "newer-store") == 0)
    {
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 2099U, &next) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 2100U, &next) == UMI_STATUS_OK &&
              next.captured.document_id == 1U &&
              (next.captured.text_revision == 2U || next.captured.store_revision == 2U));
    }
    else
        CHECK(UmiDocumentRecoveryScheduleBegin(schedule, 2100U, &next) == UMI_STATUS_NOT_FOUND);
    goto done;
inspect:
{
    UmiDocumentRecoveryScheduleInfo info;
    CHECK(UmiDocumentRecoveryScheduleInspect(schedule, &info) == UMI_STATUS_OK && info.documents == 2U &&
          info.enabled && info.interval_ms == 1000U && !info.pending);
}
done:
    UmiDocumentRecoveryScheduleDestroy(other);
    UmiDocumentRecoveryScheduleDestroy(schedule);
    return 0;
}
