/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/evidence.c
 *
 * PURPOSE:
 *   Maintain validated product implementation and acceptance evidence without
 *   persistence or frontend ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/evidence.h"

#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/experience_catalogue.h"
#include "umicom/runtime/capability_catalogue.h"

/* Provide the normalise text operation used by this module and its client applications. */
static const char *normalise_text(const char *text)
{
    return text != NULL ? text : "";
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    source = normalise_text(source);
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

/* Provide the asset exists operation used by this module and its client applications. */
static int asset_exists(const UmiApplicationExperienceDefinition *definition,
                        UmiProductisationAssetKind kind,
                        const char *asset_id)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_PRODUCTISATION_ASSET_FEATURE:
        return umi_application_experience_feature_find(definition,
                                                        asset_id) != NULL;
    case UMI_PRODUCTISATION_ASSET_PANEL:
        return umi_application_experience_panel_find(definition,
                                                      asset_id) != NULL;
    case UMI_PRODUCTISATION_ASSET_LAYOUT:
        return umi_application_experience_layout_find(definition,
                                                       asset_id) != NULL;
    case UMI_PRODUCTISATION_ASSET_COMPONENT:
        return umi_application_component_catalogue_find(asset_id) != NULL;
    case UMI_PRODUCTISATION_ASSET_CAPABILITY:
        return umi_framework_capability_catalogue_find(asset_id) != NULL;
    default:
        return 0;
    }
}

/*
 * Initialise productisation evidence ledger from caller-provided values so later
 * operations receive a known state.
 */
void umi_productisation_evidence_ledger_init(
    UmiProductisationEvidenceLedger *ledger)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL) return;
    (void)memset(ledger, 0, sizeof(*ledger));
    ledger->next_revision = 1U;
}

/*
 * Find productisation evidence while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiProductisationEvidence *umi_productisation_evidence_find(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id)
{
    size_t index;
    const char *frontend = normalise_text(frontend_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || application_id == NULL || asset_id == NULL)
        return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < ledger->count; ++index) {
        const UmiProductisationEvidence *entry = &ledger->entries[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->asset_kind == asset_kind &&
            strcmp(entry->application_id, application_id) == 0 &&
            strcmp(entry->asset_id, asset_id) == 0 &&
            strcmp(entry->frontend_id, frontend) == 0)
            return entry;
    }
    return NULL;
}

/*
 * Provide the productisation evidence record operation used by this module and its client
 * applications.
 */
UmiStatus umi_productisation_evidence_record(
    UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id,
    UmiProductisationEvidenceState state,
    const char *reference)
{
    const UmiApplicationExperienceDefinition *definition;
    UmiProductisationEvidence *entry = NULL;
    UmiProductisationEvidence candidate;
    const UmiProductisationEvidence *existing;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || application_id == NULL || application_id[0] == '\0' ||
        asset_id == NULL || asset_id[0] == '\0' ||
        !umi_productisation_asset_kind_valid(asset_kind) ||
        !umi_productisation_evidence_state_valid(state))
        return UMI_STATUS_INVALID_ARGUMENT;
    definition = umi_application_experience_catalogue_find(application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || !asset_exists(definition, asset_kind, asset_id))
        return UMI_STATUS_NOT_FOUND;

    existing = umi_productisation_evidence_find(
        ledger, application_id, asset_kind, asset_id, frontend_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL)
        entry = &ledger->entries[(size_t)(existing - ledger->entries)];
    /* Use this fallback path when the earlier condition does not apply. */
    else {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (ledger->count >= UMI_PRODUCTISATION_MAX_EVIDENCE)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memset(&candidate, 0, sizeof(candidate));
        status = copy_text(candidate.application_id,
                           sizeof(candidate.application_id), application_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(candidate.asset_id,
                           sizeof(candidate.asset_id), asset_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(candidate.frontend_id,
                           sizeof(candidate.frontend_id),
                           frontend_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_text(candidate.reference,
                           sizeof(candidate.reference), reference);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        candidate.asset_kind = asset_kind;
        candidate.state = state;
        candidate.revision = ledger->next_revision++;
        ledger->entries[ledger->count++] = candidate;
        return UMI_STATUS_OK;
    }
    status = copy_text(entry->reference, sizeof(entry->reference), reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry->state = state;
    entry->revision = ledger->next_revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the productisation evidence effective state operation used by this module and
 * its client applications.
 */
UmiProductisationEvidenceState umi_productisation_evidence_effective_state(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    UmiProductisationEvidenceState fallback_state)
{
    size_t index;
    UmiProductisationEvidenceState best = fallback_state;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || application_id == NULL || asset_id == NULL)
        return fallback_state;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < ledger->count; ++index) {
        const UmiProductisationEvidence *entry = &ledger->entries[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->asset_kind == asset_kind &&
            strcmp(entry->application_id, application_id) == 0 &&
            strcmp(entry->asset_id, asset_id) == 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (entry->state == UMI_PRODUCTISATION_EVIDENCE_BLOCKED)
                return UMI_PRODUCTISATION_EVIDENCE_BLOCKED;
            /* Apply this branch only when its contract condition is satisfied. */
            if (entry->state > best) best = entry->state;
        }
    }
    return best;
}
