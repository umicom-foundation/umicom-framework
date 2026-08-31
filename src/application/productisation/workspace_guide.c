/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/workspace_guide.c
 *
 * PURPOSE:
 *   Build one beginner-friendly application workspace guide from canonical
 *   experience, layout, feature and product adoption evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/workspace_guide.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

/* Copy text into a bounded public field and always preserve a terminator. */
static UmiStatus workspace_guide_copy_text(char *destination,
                                           size_t capacity,
                                           const char *source)
{
    size_t length;

    /* A missing pointer is a caller error because every copied field is required. */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Reject truncation so saved identifiers never become ambiguous. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Record the maturity totals used by a welcome page progress summary. */
static void workspace_guide_count_feature(
    UmiProductWorkspaceGuide *guide,
    const UmiExperienceFeatureDefinition *feature)
{
    /* Each state has a separate counter so the UI does not infer maturity. */
    switch (feature->state) {
    case UMI_EXPERIENCE_FEATURE_PLANNED:
        guide->planned_feature_count += 1U;
        break;
    case UMI_EXPERIENCE_FEATURE_FOUNDATION:
        guide->foundation_feature_count += 1U;
        break;
    case UMI_EXPERIENCE_FEATURE_IMPLEMENTED:
        guide->implemented_feature_count += 1U;
        break;
    case UMI_EXPERIENCE_FEATURE_VERIFIED:
        guide->verified_feature_count += 1U;
        break;
    default:
        /* Experience validation rejects unknown states before this helper runs. */
        break;
    }
}

/* Prefer urgent unfinished work, then the least mature item at that priority. */
static int workspace_guide_feature_preferred(
    const UmiExperienceFeatureDefinition *candidate,
    const UmiExperienceFeatureDefinition *current)
{
    /* The first unfinished feature becomes the initial recommendation. */
    if (current == NULL) {
        return 1;
    }

    /* A lower numerical priority represents more urgent product work. */
    if (candidate->priority != current->priority) {
        return candidate->priority < current->priority;
    }

    /* At equal priority, address the least mature contract first. */
    return candidate->state < current->state;
}

/* Copy the selected roadmap item into stable snapshot-owned storage. */
static UmiStatus workspace_guide_set_next_feature(
    UmiProductWorkspaceGuide *guide,
    const UmiExperienceFeatureDefinition *feature)
{
    UmiStatus status;

    status = workspace_guide_copy_text(
        guide->next_feature_id, sizeof(guide->next_feature_id),
        feature->feature_id);
    /* Stop when a public field cannot safely contain the canonical identifier. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = workspace_guide_copy_text(
        guide->next_feature_title, sizeof(guide->next_feature_title),
        feature->title);
    /* Preserve error details instead of publishing a partially copied title. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = workspace_guide_copy_text(
        guide->next_feature_summary, sizeof(guide->next_feature_summary),
        feature->summary);
    /* A summary must be complete because it is shown directly to beginners. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    guide->next_feature_state = feature->state;
    guide->next_feature_priority = feature->priority;
    guide->next_feature_owner = feature->owner;
    guide->has_next_feature = 1;
    return UMI_STATUS_OK;
}

/* Build a complete cross-application workspace guide without allocating memory. */
UmiStatus umi_product_workspace_guide_build(
    const UmiProductApplicationAdoption *adoption,
    UmiProductWorkspaceGuide *out_guide)
{
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceFeatureDefinition *next_feature = NULL;
    UmiProductApplicationAdoptionSnapshot adoption_snapshot;
    size_t index;
    UmiStatus status;

    /* The output address is required before Framework can initialise it. */
    if (out_guide == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_product_application_adoption_snapshot(
        adoption, &adoption_snapshot);
    /* Adoption validation also proves the canonical experience is present. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    experience = umi_application_experience_catalogue_find(
        adoption->application_id);
    /* Defend against catalogue changes between snapshot and lookup operations. */
    if (experience == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* A bounded guide cannot silently omit valid Framework layout choices. */
    if (experience->layout_count >
        UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(out_guide, 0, sizeof(*out_guide));
    out_guide->structure_size = (uint32_t)sizeof(*out_guide);
    status = workspace_guide_copy_text(
        out_guide->application_id, sizeof(out_guide->application_id),
        adoption->application_id);
    /* Every copy is checked because IDs form saved workspace keys. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = workspace_guide_copy_text(
        out_guide->display_name, sizeof(out_guide->display_name),
        adoption->display_name);
    /* Do not return a guide with a truncated product name. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = workspace_guide_copy_text(
        out_guide->executable_id, sizeof(out_guide->executable_id),
        adoption->executable_id);
    /* The launcher needs the exact executable identifier. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = workspace_guide_copy_text(
        out_guide->recommended_layout_id,
        sizeof(out_guide->recommended_layout_id),
        experience->default_layout_id);
    /* The recommended layout must match a canonical saved-layout key. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Project every canonical layout into one toolkit-neutral welcome choice. */
    for (index = 0U; index < experience->layout_count; ++index) {
        const UmiExperienceLayoutDefinition *layout =
            &experience->layouts[index];
        UmiProductWorkspaceGuideChoice *choice =
            &out_guide->choices[out_guide->choice_count];

        status = workspace_guide_copy_text(
            choice->layout_id, sizeof(choice->layout_id), layout->layout_id);
        /* Abort instead of exposing a choice with an invalid stable ID. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = workspace_guide_copy_text(
            choice->title, sizeof(choice->title), layout->title);
        /* Titles are copied into the snapshot for safe toolkit rendering. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = workspace_guide_copy_text(
            choice->description, sizeof(choice->description),
            layout->description);
        /* Complete descriptions help a new user choose the right workspace. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        choice->panel_count = layout->panel_count;
        choice->flags = layout->flags;
        choice->default_layout =
            strcmp(layout->layout_id, experience->default_layout_id) == 0;
        out_guide->total_panel_placements += layout->panel_count;
        out_guide->choice_count += 1U;
    }

    /* Summarise truthful maturity and select one unfinished roadmap item. */
    for (index = 0U; index < experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &experience->features[index];

        workspace_guide_count_feature(out_guide, feature);
        /* Verified items are complete and do not need to be recommended. */
        if (feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED &&
            workspace_guide_feature_preferred(feature, next_feature)) {
            next_feature = feature;
        }
    }

    /* A fully verified application intentionally has no next feature. */
    if (next_feature != NULL) {
        status = workspace_guide_set_next_feature(out_guide, next_feature);
        /* Preserve a clear failure when roadmap text exceeds public limits. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    out_guide->readiness_percent =
        umi_application_experience_readiness_percent(experience);
    out_guide->acceptance_ready = adoption_snapshot.acceptance_ready;
    return umi_product_workspace_guide_validate(out_guide);
}

/* Validate identifiers, bounds and the single recommended layout relationship. */
UmiStatus umi_product_workspace_guide_validate(
    const UmiProductWorkspaceGuide *guide)
{
    const UmiProductWorkspaceGuideChoice *recommended;
    size_t default_count = 0U;
    size_t index;

    /* Reject uninitialised or structurally incompatible snapshots. */
    if (guide == NULL ||
        guide->structure_size != (uint32_t)sizeof(*guide) ||
        guide->application_id[0] == '\0' || guide->display_name[0] == '\0' ||
        guide->executable_id[0] == '\0' ||
        guide->recommended_layout_id[0] == '\0' ||
        guide->choice_count == 0U ||
        guide->choice_count > UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY ||
        guide->readiness_percent > 100U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Check every public choice before a UI treats it as trusted data. */
    for (index = 0U; index < guide->choice_count; ++index) {
        const UmiProductWorkspaceGuideChoice *choice = &guide->choices[index];

        /* Empty identities or layouts without panels cannot guide a user. */
        if (choice->layout_id[0] == '\0' || choice->title[0] == '\0' ||
            choice->description[0] == '\0' || choice->panel_count == 0U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Count default markers so exactly one recommendation can be proven. */
        if (choice->default_layout) {
            default_count += 1U;
        }
    }

    recommended = umi_product_workspace_guide_recommended(guide);
    /* The recommended identifier and default marker must describe one choice. */
    if (default_count != 1U || recommended == NULL ||
        !recommended->default_layout) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}

/* Return a safe borrowed entry from the fixed-capacity snapshot. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_at(
    const UmiProductWorkspaceGuide *guide,
    size_t index)
{
    /* Invalid snapshots and out-of-range indexes never expose array memory. */
    if (guide == NULL ||
        guide->structure_size != (uint32_t)sizeof(*guide) ||
        index >= guide->choice_count ||
        index >= UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY) {
        return NULL;
    }

    return &guide->choices[index];
}

/* Find a layout choice by exact stable identifier. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_find(
    const UmiProductWorkspaceGuide *guide,
    const char *layout_id)
{
    size_t index;

    /* Empty search values cannot identify a canonical layout. */
    if (guide == NULL || layout_id == NULL || layout_id[0] == '\0' ||
        guide->choice_count > UMI_PRODUCT_WORKSPACE_GUIDE_LAYOUT_CAPACITY) {
        return NULL;
    }

    /* Search only the initialised portion of the fixed array. */
    for (index = 0U; index < guide->choice_count; ++index) {
        const UmiProductWorkspaceGuideChoice *choice =
            umi_product_workspace_guide_at(guide, index);

        /* A successful exact match returns borrowed snapshot memory. */
        if (choice != NULL && strcmp(choice->layout_id, layout_id) == 0) {
            return choice;
        }
    }

    return NULL;
}

/* Resolve the canonical default layout as the welcome-page recommendation. */
const UmiProductWorkspaceGuideChoice *umi_product_workspace_guide_recommended(
    const UmiProductWorkspaceGuide *guide)
{
    /* A null guide has no recommendation to resolve. */
    if (guide == NULL) {
        return NULL;
    }

    return umi_product_workspace_guide_find(
        guide, guide->recommended_layout_id);
}
