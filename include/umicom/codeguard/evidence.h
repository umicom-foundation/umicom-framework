/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/evidence.h
 *
 * PURPOSE:
 *   Publish the public evidence contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_EVIDENCE_H
#define UMICOM_CODEGUARD_EVIDENCE_H
#include "umicom/codeguard/quality_types.h"
typedef struct UmiCodeGuardEvidence {
    char id[UMI_CODEGUARD_QUALITY_ID_CAPACITY];
    UmiCodeGuardEvidenceKind kind;
    UmiCodeGuardEvidenceState state;
    size_t observed;
    size_t allowed;
    char path[UMI_CODEGUARD_QUALITY_PATH_CAPACITY];
    size_t line;
    char summary[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
    char remediation[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardEvidence;
typedef struct UmiCodeGuardEvidenceStore {
    UmiCodeGuardEvidence items[UMI_CODEGUARD_QUALITY_MAX_EVIDENCE];
    size_t count;
    uint64_t revision;
} UmiCodeGuardEvidenceStore;
UmiStatus umi_codeguard_evidence_add(UmiCodeGuardEvidenceStore *store,const UmiCodeGuardEvidence *evidence);
const UmiCodeGuardEvidence *umi_codeguard_evidence_find(const UmiCodeGuardEvidenceStore *store,const char *id);
size_t umi_codeguard_evidence_count_state(const UmiCodeGuardEvidenceStore *store,UmiCodeGuardEvidenceState state);
size_t umi_codeguard_evidence_count_kind(const UmiCodeGuardEvidenceStore *store,UmiCodeGuardEvidenceKind kind);
#endif
