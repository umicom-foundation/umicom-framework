/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/types.c
 *
 * PURPOSE:
 *   Validate and describe application productisation vocabulary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/types.h"

/*
 * Check that productisation asset kind satisfies its contract before another service
 * relies on it.
 */
int umi_productisation_asset_kind_valid(UmiProductisationAssetKind kind)
{
    return kind >= UMI_PRODUCTISATION_ASSET_FEATURE &&
           kind <= UMI_PRODUCTISATION_ASSET_CAPABILITY;
}

/*
 * Check that productisation evidence state satisfies its contract before another service
 * relies on it.
 */
int umi_productisation_evidence_state_valid(
    UmiProductisationEvidenceState state)
{
    return state >= UMI_PRODUCTISATION_EVIDENCE_DECLARED &&
           state <= UMI_PRODUCTISATION_EVIDENCE_BLOCKED;
}

/*
 * Provide the productisation asset kind text operation used by this module and its client
 * applications.
 */
const char *umi_productisation_asset_kind_text(
    UmiProductisationAssetKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_PRODUCTISATION_ASSET_FEATURE: return "feature";
    case UMI_PRODUCTISATION_ASSET_PANEL: return "panel";
    case UMI_PRODUCTISATION_ASSET_LAYOUT: return "layout";
    case UMI_PRODUCTISATION_ASSET_COMPONENT: return "component";
    case UMI_PRODUCTISATION_ASSET_CAPABILITY: return "capability";
    default: return "unknown";
    }
}

/*
 * Provide the productisation evidence state text operation used by this module and its
 * client applications.
 */
const char *umi_productisation_evidence_state_text(
    UmiProductisationEvidenceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_PRODUCTISATION_EVIDENCE_DECLARED: return "declared";
    case UMI_PRODUCTISATION_EVIDENCE_FOUNDATION: return "foundation";
    case UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED: return "implemented";
    case UMI_PRODUCTISATION_EVIDENCE_ADOPTED: return "adopted";
    case UMI_PRODUCTISATION_EVIDENCE_TESTED: return "tested";
    case UMI_PRODUCTISATION_EVIDENCE_ACCEPTED: return "accepted";
    case UMI_PRODUCTISATION_EVIDENCE_BLOCKED: return "blocked";
    default: return "unknown";
    }
}

/*
 * Provide the productisation gap kind text operation used by this module and its client
 * applications.
 */
const char *umi_productisation_gap_kind_text(UmiProductisationGapKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_PRODUCTISATION_GAP_MISSING_CAPABILITY:
        return "missing-capability";
    case UMI_PRODUCTISATION_GAP_CAPABILITY_MATURITY:
        return "capability-maturity";
    case UMI_PRODUCTISATION_GAP_MISSING_COMPONENT:
        return "missing-component";
    case UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION:
        return "application-adoption";
    case UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE:
        return "acceptance-evidence";
    case UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER:
        return "external-adapter";
    default: return "unknown";
    }
}

/*
 * Provide the productisation severity text operation used by this module and its client
 * applications.
 */
const char *umi_productisation_severity_text(UmiProductisationSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
    case UMI_PRODUCTISATION_SEVERITY_LOW: return "low";
    case UMI_PRODUCTISATION_SEVERITY_MEDIUM: return "medium";
    case UMI_PRODUCTISATION_SEVERITY_HIGH: return "high";
    case UMI_PRODUCTISATION_SEVERITY_BLOCKER: return "blocker";
    default: return "unknown";
    }
}

/*
 * Provide the productisation owner text operation used by this module and its client
 * applications.
 */
const char *umi_productisation_owner_text(UmiProductisationOwner owner)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (owner) {
    case UMI_PRODUCTISATION_OWNER_FRAMEWORK: return "framework";
    case UMI_PRODUCTISATION_OWNER_APPLICATION: return "application";
    case UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER:
        return "external-adapter";
    default: return "unknown";
    }
}

/*
 * Provide the productisation stage text operation used by this module and its client
 * applications.
 */
const char *umi_productisation_stage_text(UmiProductisationStage stage)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage) {
    case UMI_PRODUCTISATION_STAGE_FRAMEWORK: return "framework";
    case UMI_PRODUCTISATION_STAGE_STUDIO: return "studio";
    case UMI_PRODUCTISATION_STAGE_TRADER: return "trader";
    case UMI_PRODUCTISATION_STAGE_REMAINING_APPLICATIONS:
        return "remaining-applications";
    case UMI_PRODUCTISATION_STAGE_ACCEPTANCE: return "acceptance";
    default: return "unknown";
    }
}
