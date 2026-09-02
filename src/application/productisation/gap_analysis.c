/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/gap_analysis.c
 *
 * PURPOSE:
 *   Produce a source-of-truth gap report that distinguishes shared Framework
 *   work from thin-product adoption and external adapter delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/gap_analysis.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    source = source != NULL ? source : "";
    length = strlen(source);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the add gap operation used by this module and its client applications. */
static UmiStatus add_gap(UmiProductisationGapReport *report,
                         UmiProductisationGapKind kind,
                         UmiProductisationSeverity severity,
                         UmiProductisationOwner owner,
                         UmiProductisationAssetKind asset_kind,
                         const char *application_id,
                         const char *asset_id,
                         const char *capability_id,
                         size_t affected_application_count,
                         const char *format,
                         ...)
{
    UmiProductisationGap *gap;
    va_list arguments;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->gap_count >= UMI_PRODUCTISATION_MAX_GAPS) {
        report->capacity_exhausted = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    gap = &report->gaps[report->gap_count++];
    (void)memset(gap, 0, sizeof(*gap));
    gap->kind = kind;
    gap->severity = severity;
    gap->owner = owner;
    gap->asset_kind = asset_kind;
    gap->affected_application_count = affected_application_count;
    status = copy_text(gap->application_id, sizeof(gap->application_id),
                       application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(gap->asset_id, sizeof(gap->asset_id), asset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(gap->capability_id, sizeof(gap->capability_id),
                       capability_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    va_start(arguments, format);
    (void)vsnprintf(gap->summary, sizeof(gap->summary), format, arguments);
    va_end(arguments);

    /* Apply this branch only when its contract condition is satisfied. */
    if (owner == UMI_PRODUCTISATION_OWNER_FRAMEWORK)
        report->framework_gap_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (owner == UMI_PRODUCTISATION_OWNER_APPLICATION)
        report->application_gap_count += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else report->external_gap_count += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (severity == UMI_PRODUCTISATION_SEVERITY_BLOCKER)
        report->blocker_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (severity == UMI_PRODUCTISATION_SEVERITY_HIGH)
        report->high_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (severity == UMI_PRODUCTISATION_SEVERITY_MEDIUM)
        report->medium_count += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else report->low_count += 1U;
    return UMI_STATUS_OK;
}

/* Provide the feature fallback operation used by this module and its client applications. */
static UmiProductisationEvidenceState feature_fallback(
    UmiExperienceFeatureState state)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (state == UMI_EXPERIENCE_FEATURE_VERIFIED)
        return UMI_PRODUCTISATION_EVIDENCE_ACCEPTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state == UMI_EXPERIENCE_FEATURE_IMPLEMENTED)
        return UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state == UMI_EXPERIENCE_FEATURE_FOUNDATION)
        return UMI_PRODUCTISATION_EVIDENCE_FOUNDATION;
    return UMI_PRODUCTISATION_EVIDENCE_DECLARED;
}

/* Provide the feature severity operation used by this module and its client applications. */
static UmiProductisationSeverity feature_severity(
    const UmiExperienceFeatureDefinition *feature)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature->priority == UMI_EXPERIENCE_PRIORITY_P0)
        return UMI_PRODUCTISATION_SEVERITY_BLOCKER;
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature->priority == UMI_EXPERIENCE_PRIORITY_P1)
        return UMI_PRODUCTISATION_SEVERITY_HIGH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature->priority == UMI_EXPERIENCE_PRIORITY_P2)
        return UMI_PRODUCTISATION_SEVERITY_MEDIUM;
    return UMI_PRODUCTISATION_SEVERITY_LOW;
}

/*
 * Provide the add capability gaps operation used by this module and its client
 * applications.
 */
static UmiStatus add_capability_gaps(
    const UmiProductCapabilityMatrix *matrix,
    UmiProductisationGapReport *report)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < matrix->usage_count; ++index) {
        const UmiProductCapabilityUsage *usage = &matrix->usages[index];
        UmiStatus status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (usage->missing_from_framework) {
            status = add_gap(report,
                UMI_PRODUCTISATION_GAP_MISSING_CAPABILITY,
                UMI_PRODUCTISATION_SEVERITY_BLOCKER,
                UMI_PRODUCTISATION_OWNER_FRAMEWORK,
                UMI_PRODUCTISATION_ASSET_CAPABILITY,
                "*", usage->capability_id, usage->capability_id,
                usage->application_count,
                "Register and implement capability '%s' used by %zu applications.",
                usage->capability_id, usage->application_count);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (usage->shared_across_applications &&
                   usage->maturity != UMI_CAPABILITY_IMPLEMENTED) {
            const UmiProductisationSeverity severity =
                usage->maturity == UMI_CAPABILITY_PLANNED
                ? UMI_PRODUCTISATION_SEVERITY_BLOCKER
                : UMI_PRODUCTISATION_SEVERITY_HIGH;
            status = add_gap(report,
                UMI_PRODUCTISATION_GAP_CAPABILITY_MATURITY, severity,
                UMI_PRODUCTISATION_OWNER_FRAMEWORK,
                UMI_PRODUCTISATION_ASSET_CAPABILITY,
                "*", usage->capability_id, usage->capability_id,
                usage->application_count,
                "Complete shared Framework capability '%s' before duplicating it in %zu products.",
                usage->capability_id, usage->application_count);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (usage->shared_across_applications &&
            usage->panel_reference_count > 0U &&
            usage->component_count == 0U) {
            status = add_gap(report,
                UMI_PRODUCTISATION_GAP_MISSING_COMPONENT,
                UMI_PRODUCTISATION_SEVERITY_MEDIUM,
                UMI_PRODUCTISATION_OWNER_FRAMEWORK,
                UMI_PRODUCTISATION_ASSET_COMPONENT,
                "*", usage->capability_id, usage->capability_id,
                usage->application_count,
                "Add a reusable component projection for '%s' used by %zu applications and %zu panels.",
                usage->capability_id, usage->application_count,
                usage->panel_reference_count);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the add feature gaps operation used by this module and its client applications. */
static UmiStatus add_feature_gaps(
    const UmiApplicationExperienceDefinition *definition,
    const UmiProductCapabilityMatrix *matrix,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationGapReport *report)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &definition->features[index];
        const UmiProductisationEvidenceState state =
            umi_productisation_evidence_effective_state(
                ledger, definition->application_id,
                UMI_PRODUCTISATION_ASSET_FEATURE, feature->feature_id,
                feature_fallback(feature->state));
        const UmiProductCapabilityUsage *usage =
            feature->required_capability != NULL
            ? umi_product_capability_matrix_find(
                matrix, feature->required_capability) : NULL;
        UmiProductisationOwner owner;
        UmiProductisationGapKind kind;
        UmiProductisationSeverity severity;
        UmiStatus status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (state == UMI_PRODUCTISATION_EVIDENCE_ACCEPTED) continue;
        severity = feature_severity(feature);
        /* Apply this branch only when its contract condition is satisfied. */
        if (state == UMI_PRODUCTISATION_EVIDENCE_BLOCKED)
            severity = UMI_PRODUCTISATION_SEVERITY_BLOCKER;

        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->owner == UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER) {
            owner = UMI_PRODUCTISATION_OWNER_EXTERNAL_ADAPTER;
            kind = UMI_PRODUCTISATION_GAP_EXTERNAL_ADAPTER;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION) {
            owner = UMI_PRODUCTISATION_OWNER_APPLICATION;
            kind = UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (state < UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED &&
                   (usage == NULL || !usage->shared_across_applications)) {
            owner = UMI_PRODUCTISATION_OWNER_FRAMEWORK;
            kind = UMI_PRODUCTISATION_GAP_CAPABILITY_MATURITY;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            owner = UMI_PRODUCTISATION_OWNER_APPLICATION;
            kind = UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE;
        }
        status = add_gap(report, kind, severity, owner,
            UMI_PRODUCTISATION_ASSET_FEATURE,
            definition->application_id, feature->feature_id,
            feature->required_capability,
            usage != NULL ? usage->application_count : 1U,
            "Move feature '%s' from %s to accepted with executable product evidence.",
            feature->feature_id,
            umi_productisation_evidence_state_text(state));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Provide the add surface gaps operation used by this module and its client applications. */
static UmiStatus add_surface_gaps(
    const UmiApplicationExperienceDefinition *definition,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationGapReport *report)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel = &definition->panels[index];
        const UmiProductisationEvidenceState state =
            umi_productisation_evidence_effective_state(
                ledger, definition->application_id,
                UMI_PRODUCTISATION_ASSET_PANEL, panel->panel_id,
                UMI_PRODUCTISATION_EVIDENCE_DECLARED);
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (state >= UMI_PRODUCTISATION_EVIDENCE_ACCEPTED &&
            state != UMI_PRODUCTISATION_EVIDENCE_BLOCKED) continue;
        status = add_gap(report,
            state >= UMI_PRODUCTISATION_EVIDENCE_TESTED
                ? UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE
                : UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION,
            (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U
                ? UMI_PRODUCTISATION_SEVERITY_HIGH
                : UMI_PRODUCTISATION_SEVERITY_MEDIUM,
            UMI_PRODUCTISATION_OWNER_APPLICATION,
            UMI_PRODUCTISATION_ASSET_PANEL,
            definition->application_id, panel->panel_id,
            panel->required_capability, 1U,
            "Render, integrate and accept panel '%s' in its declared workbench layouts.",
            panel->panel_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout =
            &definition->layouts[index];
        const UmiProductisationEvidenceState state =
            umi_productisation_evidence_effective_state(
                ledger, definition->application_id,
                UMI_PRODUCTISATION_ASSET_LAYOUT, layout->layout_id,
                UMI_PRODUCTISATION_EVIDENCE_DECLARED);
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (state >= UMI_PRODUCTISATION_EVIDENCE_ACCEPTED &&
            state != UMI_PRODUCTISATION_EVIDENCE_BLOCKED) continue;
        status = add_gap(report,
            state >= UMI_PRODUCTISATION_EVIDENCE_TESTED
                ? UMI_PRODUCTISATION_GAP_ACCEPTANCE_EVIDENCE
                : UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION,
            UMI_PRODUCTISATION_SEVERITY_MEDIUM,
            UMI_PRODUCTISATION_OWNER_APPLICATION,
            UMI_PRODUCTISATION_ASSET_LAYOUT,
            definition->application_id, layout->layout_id, "", 1U,
            "Compose, persist, restore and accept layout '%s'.",
            layout->layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Perform productisation gap analysis through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_productisation_gap_analysis_run(
    const UmiProductCapabilityMatrix *matrix,
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductisationGapReport *out_report)
{
    size_t application_index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    status = add_capability_gaps(matrix, out_report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (application_index = 0U;
         application_index < umi_application_experience_catalogue_count();
         ++application_index) {
        const UmiApplicationExperienceDefinition *definition =
            umi_application_experience_catalogue_at(application_index);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (definition == NULL) return UMI_STATUS_INVALID_STATE;
        status = add_feature_gaps(definition, matrix, ledger, out_report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = add_surface_gaps(definition, ledger, out_report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Find productisation gap report while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiProductisationGap *umi_productisation_gap_report_at(
    const UmiProductisationGapReport *report,
    size_t index)
{
    return report != NULL && index < report->gap_count
        ? &report->gaps[index] : NULL;
}
