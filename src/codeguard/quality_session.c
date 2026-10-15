/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_session.c
 *
 * PURPOSE:
 *   Coordinate reusable quality evidence, documentation totals, release
 *   decisions and remediation guidance for one application workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/codeguard/quality_session.h"

#include <string.h>

/* Establish deterministic empty state before any application records evidence. */
void umi_codeguard_quality_session_init(UmiCodeGuardQualitySession *session)
{
    /* NULL-safe initialisation supports optional quality services during startup. */
    if (session == NULL) {
        return;
    }
    (void)memset(session, 0, sizeof(*session));
    umi_codeguard_architecture_policy_default(&session->architecture);
    umi_codeguard_quality_policy_default(&session->policy);
    umi_codeguard_documentation_inventory_init(&session->documentation);
    session->revision = 1U;
}

/* Copy one Framework evidence record and publish a revision only after success. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_codeguard_quality_session_record(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardEvidence *evidence)
{
    UmiStatus status;
    /* The evidence store validates the item, but the session itself is mandatory. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_evidence_add(&session->evidence, evidence);
    /* Failed evidence must not make observers believe the session changed. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}
#endif

UmiStatus umi_codeguard_quality_session_record(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardEvidence *evidence)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (session != NULL && session->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /* The evidence store validates the item, but the session itself is mandatory. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_evidence_add(&session->evidence, evidence);
    /* Failed evidence must not make observers believe the session changed. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}

/* Stream one file report into the workspace inventory and advance its revision. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_codeguard_quality_session_record_documentation(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardDocumentationReport *report)
{
    UmiStatus status;
    /* A missing session cannot own or publish aggregate documentation evidence. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_documentation_inventory_record(
        &session->documentation, report);
    /* Only a completely recorded report changes the observable session revision. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}
#endif

UmiStatus umi_codeguard_quality_session_record_documentation(
    UmiCodeGuardQualitySession *session,
    const UmiCodeGuardDocumentationReport *report)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (session != NULL && session->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /* A missing session cannot own or publish aggregate documentation evidence. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_documentation_inventory_record(
        &session->documentation, report);
    /* Only a completely recorded report changes the observable session revision. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}

/* Evaluate all accumulated evidence using the current reusable quality policy. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_codeguard_quality_session_evaluate(UmiCodeGuardQualitySession *session)
{
    UmiStatus status;
    /* A missing session contains no evidence from which to make a decision. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_quality_evaluate(
        &session->policy,
        &session->evidence,
        &session->api,
        &session->duplicates,
        &session->evaluation);
    /* Remediation is rebuilt only when evaluation produced a valid decision. */
    if (status == UMI_STATUS_OK) {
        status = umi_codeguard_remediation_plan_build(
            &session->evidence, &session->remediation);
    }
    /* Publish one revision after both evaluation stages succeed. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}
#endif

UmiStatus umi_codeguard_quality_session_evaluate(UmiCodeGuardQualitySession *session)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (session != NULL && session->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status;
    /* A missing session contains no evidence from which to make a decision. */
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_codeguard_quality_evaluate(
        &session->policy,
        &session->evidence,
        &session->api,
        &session->duplicates,
        &session->evaluation);
    /* Remediation is rebuilt only when evaluation produced a valid decision. */
    if (status == UMI_STATUS_OK) {
        status = umi_codeguard_remediation_plan_build(
            &session->evidence, &session->remediation);
    }
    /* Publish one revision after both evaluation stages succeed. */
    if (status == UMI_STATUS_OK) {
        session->revision += 1U;
    }
    return status;
}
