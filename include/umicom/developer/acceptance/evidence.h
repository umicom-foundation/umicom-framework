/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/acceptance/evidence.h
 *
 * PURPOSE:
 *   Store bounded acceptance evidence without replacing the existing developer capability catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_ACCEPTANCE_EVIDENCE_H
#define UMICOM_DEVELOPER_ACCEPTANCE_EVIDENCE_H

#include "umicom/developer/acceptance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer acceptance evidence store data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperAcceptanceEvidenceStore {
    UmiDeveloperAcceptanceEvidence items[UMI_DEVELOPER_ACCEPTANCE_MAX_EVIDENCE];
    size_t count;
    uint64_t revision;
} UmiDeveloperAcceptanceEvidenceStore;

/* Initialise an empty caller-owned evidence store with a non-zero revision. */
void umi_developer_acceptance_evidence_store_init(
    UmiDeveloperAcceptanceEvidenceStore *store);

/*
 * Add or replace evidence by capability id. This lets focused tests/build
 * probes update verification state without mutating capability definitions.
 */
UmiStatus umi_developer_acceptance_evidence_upsert(
    UmiDeveloperAcceptanceEvidenceStore *store,
    const UmiDeveloperAcceptanceEvidence *evidence);

/* Find evidence by existing developer capability id; storage remains store-owned. */
const UmiDeveloperAcceptanceEvidence *umi_developer_acceptance_evidence_find(
    const UmiDeveloperAcceptanceEvidenceStore *store,
    const char *capability_id);

#ifdef __cplusplus
}
#endif
#endif
