/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/quality_session.h
 *
 * PURPOSE:
 *   Publish the public quality session contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_QUALITY_SESSION_H
#define UMICOM_CODEGUARD_QUALITY_SESSION_H

#include "umicom/codeguard/architecture_policy.h"
#include "umicom/codeguard/documentation_inventory.h"
#include "umicom/codeguard/quality_evaluation.h"
#include "umicom/codeguard/remediation_plan.h"

/* A quality session keeps related evidence together for one workspace. Every
 * member owns its data; no source-file or UI pointer is retained. */
typedef struct UmiCodeGuardQualitySession {
    UmiCodeGuardEvidenceStore evidence;            /* Build and review evidence. */
    UmiCodeGuardArchitecturePolicy architecture;   /* Allowed dependency rules. */
    UmiCodeGuardDuplicateReview duplicates;        /* Duplicate review state. */
    UmiCodeGuardApiComparison api;                  /* Public API compatibility. */
    UmiCodeGuardQualityPolicy policy;               /* Release decision policy. */
    UmiCodeGuardQualityEvaluation evaluation;       /* Latest evaluated result. */
    UmiCodeGuardRemediationPlan remediation;        /* Ordered repair guidance. */
    uint64_t revision;                              /* Monotonic change counter. */
    /* Appended for source compatibility with existing field offsets. */
    UmiCodeGuardDocumentationInventory documentation; /* Source guidance totals. */
} UmiCodeGuardQualitySession;

/* Initialise an empty quality session with Framework default policies. */
void umi_codeguard_quality_session_init(UmiCodeGuardQualitySession *session);

/* Copy one evidence item into the session and advance its revision. */
UmiStatus umi_codeguard_quality_session_record(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardEvidence *evidence);

/* Aggregate one documentation report without retaining its source-file path. */
UmiStatus umi_codeguard_quality_session_record_documentation(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardDocumentationReport *report);

/* Recalculate release decisions and remediation from current evidence. */
UmiStatus umi_codeguard_quality_session_evaluate(UmiCodeGuardQualitySession *session);

#endif
