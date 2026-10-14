/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/synthetic_encounter.h
 *
 * PURPOSE:
 *   Model an in-memory, synthetic-only encounter intake lifecycle. Encounter
 *   ownership is independent of application windows and of administrative
 *   routing. The contract carries no symptoms, medical history or diagnoses.
 *
 * SAFETY AND OWNERSHIP:
 *   Development and deterministic testing only. No real patient information,
 *   clinical assessment, treatment decision, or prescription is accepted.
 *   No persistence, authorisation, clinical audit or concurrency is provided.
 *   The caller owns the opaque book; patient registry is borrowed for Open.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *   Copyright (c) 2026 Umicom Foundation
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_HEALTHCARE_SYNTHETIC_ENCOUNTER_H
#define UMICOM_HEALTHCARE_SYNTHETIC_ENCOUNTER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/healthcare/synthetic_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY 11U
#define UMI_HEALTHCARE_SYNTHETIC_MAX_ENCOUNTERS 32U

typedef struct UmiHealthcareSyntheticEncounterBook UmiHealthcareSyntheticEncounterBook;

typedef enum UmiHealthcareSyntheticIntakeStage {
    UMI_HEALTHCARE_INTAKE_OPEN = 1,
    UMI_HEALTHCARE_INTAKE_COMPLETE = 2
} UmiHealthcareSyntheticIntakeStage;

typedef struct UmiHealthcareSyntheticEncounter {
    char encounterId[UMI_HEALTHCARE_SYNTHETIC_ENCOUNTER_ID_CAPACITY];
    char patientId[UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY];
    UmiHealthcareSyntheticIntakeStage stage;
    uint64_t revision;
} UmiHealthcareSyntheticEncounter;

/* Recognise ENC- and exactly six digits. Inputs must be NUL-terminated. */
UmiStatus UmiHealthcareSyntheticEncounterIdValidate(const char *encounterId);

/* Allocate an empty book. Destroy after all dependent routing snapshots are no
 * longer needed. On failure the output pointer is set to NULL. */
UmiStatus UmiHealthcareSyntheticEncounterBookCreate(
    UmiHealthcareSyntheticEncounterBook **outBook);
void UmiHealthcareSyntheticEncounterBookDestroy(
    UmiHealthcareSyntheticEncounterBook *book);

/* Open an administrative intake for an existing synthetic patient. The
 * identifier registry is read during this call, not retained. Multiple
 * encounters for the same test patient are allowed. On failure outEncounter
 * and the book are unchanged. */
UmiStatus UmiHealthcareSyntheticEncounterOpen(
    UmiHealthcareSyntheticEncounterBook *book,
    const UmiHealthcareSyntheticRegistry *registry,
    const char *syntheticPatientId,
    UmiHealthcareSyntheticEncounter *outEncounter);

/* Record that the synthetic intake checkpoint was completed by its caller;
 * this is NOT a medical assessment or a clinical safety determination.
 * Compare expectedRevision to prevent an accidental stale update. */
UmiStatus UmiHealthcareSyntheticEncounterMarkIntakeComplete(
    UmiHealthcareSyntheticEncounterBook *book,
    const char *encounterId,
    uint64_t expectedRevision,
    UmiHealthcareSyntheticEncounter *outEncounter);

/* Snapshots are copied to the caller; neither query changes the book. */
UmiStatus UmiHealthcareSyntheticEncounterFind(
    const UmiHealthcareSyntheticEncounterBook *book,
    const char *encounterId,
    UmiHealthcareSyntheticEncounter *outEncounter);
UmiStatus UmiHealthcareSyntheticEncounterCount(
    const UmiHealthcareSyntheticEncounterBook *book,
    size_t *outCount);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_HEALTHCARE_SYNTHETIC_ENCOUNTER_H */
