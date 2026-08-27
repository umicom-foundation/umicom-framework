/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/evidence.h
 *
 * PURPOSE:
 *   Record bounded implementation, adoption, test and acceptance evidence for
 *   the assets declared by Framework application experiences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EVIDENCE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EVIDENCE_H

#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductisationEvidence {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiProductisationAssetKind asset_kind;
    char asset_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char frontend_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiProductisationEvidenceState state;
    char reference[UMI_PRODUCTISATION_REFERENCE_CAPACITY];
    uint64_t revision;
} UmiProductisationEvidence;

typedef struct UmiProductisationEvidenceLedger {
    UmiProductisationEvidence entries[UMI_PRODUCTISATION_MAX_EVIDENCE];
    size_t count;
    uint64_t next_revision;
} UmiProductisationEvidenceLedger;

void umi_productisation_evidence_ledger_init(
    UmiProductisationEvidenceLedger *ledger);
UmiStatus umi_productisation_evidence_record(
    UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id,
    UmiProductisationEvidenceState state,
    const char *reference);
const UmiProductisationEvidence *umi_productisation_evidence_find(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id);
UmiProductisationEvidenceState umi_productisation_evidence_effective_state(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    UmiProductisationEvidenceState fallback_state);

#ifdef __cplusplus
}
#endif

#endif
