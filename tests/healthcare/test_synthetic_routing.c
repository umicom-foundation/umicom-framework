/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_synthetic_routing.c
 * PURPOSE: Verify manual worklist routing cannot decide or alter treatment.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 * Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/synthetic_routing.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "Routing check failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiHealthcareSyntheticRegistry *patients = NULL;
    UmiHealthcareSyntheticEncounterBook *encounters = NULL;
    UmiHealthcareSyntheticRoutingBook *routes = NULL;
    UmiHealthcareSyntheticPatient patient = {0};
    UmiHealthcareSyntheticEncounter encounter = {0};
    UmiHealthcareSyntheticRoute route = {0};
    UmiHealthcareSyntheticRoute sentinel = {0};
    size_t count = 0U;
    size_t index;

    CHECK(UmiHealthcareSyntheticRoutingBookCreate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiHealthcareSyntheticRegistryCreate(&patients) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterBookCreate(&encounters) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRoutingBookCreate(&routes) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRegistryRegister(
              patients, "SYN-000001", &patient) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticEncounterOpen(
              encounters, patients, patient.patientId, &encounter) == UMI_STATUS_OK);

    sentinel.revision = 707U;
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, encounter.encounterId,
              UMI_HEALTHCARE_WORKLIST_GENERAL, &sentinel) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(sentinel.revision == 707U);
    CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
              encounters, encounter.encounterId, encounter.revision,
              &encounter) == UMI_STATUS_OK);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, encounter.encounterId,
              (UmiHealthcareSyntheticWorklist)0, &sentinel) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel.revision == 707U);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, encounter.encounterId,
              UMI_HEALTHCARE_WORKLIST_GENERAL, &route) ==
          UMI_STATUS_OK);
    CHECK(strcmp(route.encounterId, encounter.encounterId) == 0);
    CHECK(route.worklist == UMI_HEALTHCARE_WORKLIST_GENERAL);
    CHECK(route.revision == 1U);
    CHECK(route.sourceEncounterRevision == encounter.revision);
    CHECK(encounter.stage == UMI_HEALTHCARE_INTAKE_COMPLETE);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, encounter.encounterId,
              UMI_HEALTHCARE_WORKLIST_SPECIALIST, &sentinel) ==
          UMI_STATUS_ALREADY_EXISTS);
    CHECK(sentinel.revision == 707U);
    CHECK(UmiHealthcareSyntheticRouteRequest(
              routes, encounters, "ENC-999999",
              UMI_HEALTHCARE_WORKLIST_GENERAL, &sentinel) == UMI_STATUS_NOT_FOUND);
    CHECK(sentinel.revision == 707U);
    CHECK(UmiHealthcareSyntheticRouteFind(
              routes, encounter.encounterId, &sentinel) == UMI_STATUS_OK);
    CHECK(sentinel.worklist == UMI_HEALTHCARE_WORKLIST_GENERAL);
    sentinel.revision = 707U;
    CHECK(UmiHealthcareSyntheticRouteFind(
              routes, "ENC-999999", &sentinel) == UMI_STATUS_NOT_FOUND);
    CHECK(sentinel.revision == 707U);

    /* Every route is a manually selected queue; no rule engine or clinical
     * outcome is inferred from any patient or encounter identity. */
    for (index = 2U; index <= UMI_HEALTHCARE_SYNTHETIC_MAX_ROUTES;
         ++index) {
        CHECK(UmiHealthcareSyntheticEncounterOpen(
                  encounters, patients, patient.patientId, &encounter) == UMI_STATUS_OK);
        CHECK(UmiHealthcareSyntheticEncounterMarkIntakeComplete(
                  encounters, encounter.encounterId, encounter.revision,
                  &encounter) == UMI_STATUS_OK);
        CHECK(UmiHealthcareSyntheticRouteRequest(
                  routes, encounters, encounter.encounterId,
                  UMI_HEALTHCARE_WORKLIST_REHABILITATION, &route) == UMI_STATUS_OK);
        CHECK(route.revision == index);
    }
    CHECK(UmiHealthcareSyntheticRouteCount(routes, &count) == UMI_STATUS_OK);
    CHECK(count == UMI_HEALTHCARE_SYNTHETIC_MAX_ROUTES);
    CHECK(UmiHealthcareSyntheticRouteCount(NULL, &count) ==
          UMI_STATUS_INVALID_ARGUMENT);
    UmiHealthcareSyntheticRoutingBookDestroy(routes);
    UmiHealthcareSyntheticEncounterBookDestroy(encounters);
    UmiHealthcareSyntheticRegistryDestroy(patients);
    UmiHealthcareSyntheticRoutingBookDestroy(NULL);
    puts("Synthetic manual worklist routing contracts passed");
    return 0;
}
