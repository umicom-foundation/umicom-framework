/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/synthetic_routing.h
 *
 * PURPOSE:
 *   Keep manually requested administrative worklist routing independent of
 *   synthetic encounter intake and of clinical treatment decisions.
 *
 * SAFETY AND OWNERSHIP:
 *   A queue assignment is never a diagnosis, recommendation, prescription or
 *   clinical approval. No algorithms select queues. An authorised human and
 *   secure clinical workflow are NOT implemented. Only synthetic identifiers
 *   are supported; routing remains in memory and is not clinically usable.
 *   This component is not thread-safe.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_SYNTHETIC_ROUTING_H
#define UMICOM_HEALTHCARE_SYNTHETIC_ROUTING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/healthcare/synthetic_encounter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HEALTHCARE_SYNTHETIC_MAX_ROUTES 32U

typedef struct UmiHealthcareSyntheticRoutingBook UmiHealthcareSyntheticRoutingBook;

typedef enum UmiHealthcareSyntheticWorklist {
    UMI_HEALTHCARE_WORKLIST_GENERAL = 1,
    UMI_HEALTHCARE_WORKLIST_REHABILITATION = 2,
    UMI_HEALTHCARE_WORKLIST_SPECIALIST = 3
} UmiHealthcareSyntheticWorklist;

typedef struct UmiHealthcareSyntheticRoute {
    char encounterId[UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY];
    UmiHealthcareSyntheticWorklist worklist;
    uint64_t sourceEncounterRevision;
    uint64_t revision;
} UmiHealthcareSyntheticRoute;

UmiStatus UmiHealthcareSyntheticRoutingBookCreate(
    UmiHealthcareSyntheticRoutingBook **outBook);
void UmiHealthcareSyntheticRoutingBookDestroy(
    UmiHealthcareSyntheticRoutingBook *book);

/* The caller explicitly chooses a worklist, while the routing service
 * resolves the encounter from its authoritative Framework intake book.
 * Copied or fabricated caller snapshots cannot declare an intake complete.
 * No mutation of the encounter book or treatment state occurs. A second
 * route for the same encounter is refused, never silently overwritten. */
UmiStatus UmiHealthcareSyntheticRouteRequest(
    UmiHealthcareSyntheticRoutingBook *book,
    const UmiHealthcareSyntheticEncounterBook *encounters,
    const char *encounterId,
    UmiHealthcareSyntheticWorklist worklist,
    UmiHealthcareSyntheticRoute *outRoute);

UmiStatus UmiHealthcareSyntheticRouteFind(
    const UmiHealthcareSyntheticRoutingBook *book,
    const char *encounterId,
    UmiHealthcareSyntheticRoute *outRoute);
UmiStatus UmiHealthcareSyntheticRouteCount(
    const UmiHealthcareSyntheticRoutingBook *book,
    size_t *outCount);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_HEALTHCARE_SYNTHETIC_ROUTING_H */
