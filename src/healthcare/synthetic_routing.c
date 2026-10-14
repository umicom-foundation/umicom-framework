/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/synthetic_routing.c
 *
 * PURPOSE:
 *   Provide in-memory manual worklist requests, separate from encounter
 *   intake. No clinical rule is evaluated and no treatment state is changed.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_routing.h"

#include <stdlib.h>
#include <string.h>

struct UmiHealthcareSyntheticRoutingBook {
    UmiHealthcareSyntheticRoute records[UMI_HEALTHCARE_SYNTHETIC_MAX_ROUTES];
    size_t count;
    uint64_t latestRevision;
};

static int UmiHealthcareSyntheticWorklistValid(
    UmiHealthcareSyntheticWorklist worklist)
{
    switch (worklist) {
        case UMI_HEALTHCARE_WORKLIST_GENERAL:
        case UMI_HEALTHCARE_WORKLIST_REHABILITATION:
        case UMI_HEALTHCARE_WORKLIST_SPECIALIST:
            return 1;
        default:
            return 0;
    }
}

UmiStatus UmiHealthcareSyntheticRoutingBookCreate(
    UmiHealthcareSyntheticRoutingBook **outBook)
{
    if (outBook == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outBook = (UmiHealthcareSyntheticRoutingBook *)calloc(1U, sizeof(**outBook));
    return *outBook != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void UmiHealthcareSyntheticRoutingBookDestroy(
    UmiHealthcareSyntheticRoutingBook *book)
{
    free(book);
}

UmiStatus UmiHealthcareSyntheticRouteRequest(
    UmiHealthcareSyntheticRoutingBook *book,
    const UmiHealthcareSyntheticEncounterBook *encounters,
    const char *encounterId,
    UmiHealthcareSyntheticWorklist worklist,
    UmiHealthcareSyntheticRoute *outRoute)
{
    UmiHealthcareSyntheticEncounter completedIntake = {0};
    UmiHealthcareSyntheticRoute candidate = {0};
    UmiStatus status;
    size_t index;

    if (book == NULL || encounters == NULL || outRoute == NULL ||
        !UmiHealthcareSyntheticWorklistValid(worklist)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* The encounter book, not the caller, supplies the intake state and
     * source revision. A fabricated snapshot cannot cross this boundary. */
    status = UmiHealthcareSyntheticEncounterFind(
        encounters, encounterId, &completedIntake);
    if (status != UMI_STATUS_OK) return status;
    if (completedIntake.stage != UMI_HEALTHCARE_INTAKE_COMPLETE ||
        completedIntake.revision == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].encounterId,
                   completedIntake.encounterId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (book->count >= UMI_HEALTHCARE_SYNTHETIC_MAX_ROUTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* The caller supplies a manual destination. No symptoms, health data,
     * automatic triage, diagnosis or treatment approval enters this service. */
    (void)memcpy(candidate.encounterId, completedIntake.encounterId,
                 sizeof(candidate.encounterId));
    candidate.worklist = worklist;
    candidate.sourceEncounterRevision = completedIntake.revision;
    candidate.revision = book->latestRevision + 1U;
    book->records[book->count] = candidate;
    ++book->count;
    book->latestRevision = candidate.revision;
    *outRoute = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiHealthcareSyntheticRouteFind(
    const UmiHealthcareSyntheticRoutingBook *book,
    const char *encounterId,
    UmiHealthcareSyntheticRoute *outRoute)
{
    size_t index;

    if (book == NULL || outRoute == NULL ||
        UmiHealthcareSyntheticEncounterIdValidate(encounterId) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < book->count; ++index) {
        if (strcmp(book->records[index].encounterId, encounterId) == 0) {
            *outRoute = book->records[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiHealthcareSyntheticRouteCount(
    const UmiHealthcareSyntheticRoutingBook *book,
    size_t *outCount)
{
    if (book == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCount = book->count;
    return UMI_STATUS_OK;
}
