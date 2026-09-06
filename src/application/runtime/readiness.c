/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/readiness.c
 *
 * PURPOSE:
 *   Aggregate immutable feature state, priority and ownership without duplicating product logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/readiness.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_plan.h"

/* Append one text fragment while always leaving room for the terminating byte. */
static void append_reason(char *destination,
                          size_t capacity,
                          const char *text)
{
    size_t used;

    /* Ignore an absent fragment instead of dereferencing a null diagnostic value. */
    if (destination == NULL || capacity == 0U || text == NULL) return;
    used = strlen(destination);
    /* Keep the destination terminated when earlier fragments already filled it. */
    if (used >= capacity - 1U) {
        destination[capacity - 1U] = '\0';
        return;
    }
    /* Copy only the part that fits so a long provider message cannot overflow the report. */
    while (*text != '\0' && used + 1U < capacity) {
        destination[used] = *text;
        used += 1U;
        text += 1;
    }
    destination[used] = '\0';
}

/*
 * Provide the application readiness report operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_readiness_report(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationReadinessReport *out_report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = sizeof(*out_report);
    out_report->feature_count = experience->feature_count;
    out_report->readiness_percent = umi_application_experience_readiness_percent(experience);
    out_report->next_feature = umi_application_experience_next_feature(experience);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature = &experience->features[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->state == UMI_EXPERIENCE_FEATURE_PLANNED) out_report->planned_count += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (feature->state == UMI_EXPERIENCE_FEATURE_FOUNDATION) out_report->foundation_count += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (feature->state == UMI_EXPERIENCE_FEATURE_IMPLEMENTED) out_report->implemented_count += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (feature->state == UMI_EXPERIENCE_FEATURE_VERIFIED) out_report->verified_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED &&
            feature->priority == UMI_EXPERIENCE_PRIORITY_P0) out_report->p0_open_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED &&
            feature->priority == UMI_EXPERIENCE_PRIORITY_P1) out_report->p1_open_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->owner == UMI_EXPERIENCE_OWNER_FRAMEWORK) out_report->framework_owned_count += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION) out_report->application_owned_count += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (feature->owner == UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER) out_report->external_adapter_count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application readiness has open priority operation used by this module and
 * its client applications.
 */
int umi_application_readiness_has_open_priority(
    const UmiApplicationReadinessReport *report)
{
    return report != NULL && (report->p0_open_count > 0U || report->p1_open_count > 0U);
}

/*
 * Check the canonical Framework experience before the application picker marks
 * a runtime record as launchable.  Returning a report with UMI_STATUS_OK for a
 * blocked state lets the caller explain the problem instead of losing it as a
 * generic launch error.
 */
UmiStatus umi_application_launch_readiness_check(
    const char *application_id,
    UmiApplicationLaunchReadiness *out_readiness)
{
    const UmiApplicationExperienceDefinition *experience;
    UmiStatus status;
    size_t identifier_length;

    /* Reject missing output storage or an empty identifier before reading it. */
    if (application_id == NULL || application_id[0] == '\0' ||
        out_readiness == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    identifier_length = strlen(application_id);
    /* Keep the copied identity bounded so all later diagnostic text is safe. */
    if (identifier_length >= sizeof(out_readiness->application_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(out_readiness, 0, sizeof(*out_readiness));
    out_readiness->structure_size = (uint32_t)sizeof(*out_readiness);
    (void)memcpy(out_readiness->application_id,
                 application_id,
                 identifier_length + 1U);

    /* Resolve aliases through the one Framework catalogue used by all products. */
    experience = umi_application_experience_catalogue_find(application_id);
    if (experience == NULL) {
        out_readiness->state =
            UMI_APPLICATION_LAUNCH_READINESS_MISSING_EXPERIENCE;
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "No Framework experience is registered for '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      out_readiness->application_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "'.");
        return UMI_STATUS_OK;
    }

    /* Copy counts before validation so diagnostics can show an incomplete recipe. */
    out_readiness->layout_count = experience->layout_count;
    out_readiness->panel_count = experience->panel_count;
    /* A launch surface cannot be opened when it has no panels to display. */
    if (experience->panel_count == 0U || experience->panels == NULL) {
        out_readiness->state = UMI_APPLICATION_LAUNCH_READINESS_NO_PANELS;
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "The Framework experience for '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      out_readiness->application_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "' has no panels.");
        return UMI_STATUS_OK;
    }
    /* A panel collection is not enough; the user also needs a starting layout. */
    if (experience->layout_count == 0U || experience->layouts == NULL) {
        out_readiness->state = UMI_APPLICATION_LAUNCH_READINESS_NO_LAYOUT;
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "The Framework experience for '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      out_readiness->application_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "' has no layout.");
        return UMI_STATUS_OK;
    }

    /* Validate nested panel and layout references before calculating readiness. */
    status = umi_application_experience_validate(experience);
    /* Keep invalid catalogue data out of the executable launch path. */
    if (status != UMI_STATUS_OK) {
        out_readiness->state =
            UMI_APPLICATION_LAUNCH_READINESS_INVALID_EXPERIENCE;
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "Framework experience validation failed for '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      out_readiness->application_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "': ");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      umi_status_text(status));
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      ".");
        return UMI_STATUS_OK;
    }

    /* A valid experience must also resolve the layout named as its default. */
    if (umi_application_experience_layout_find(
            experience, experience->default_layout_id) == NULL) {
        out_readiness->state =
            UMI_APPLICATION_LAUNCH_READINESS_INVALID_EXPERIENCE;
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "Default layout '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      experience->default_layout_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "' is missing for '");
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      out_readiness->application_id);
        append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                      "'.");
        return UMI_STATUS_OK;
    }

    out_readiness->feature_readiness_percent =
        umi_application_experience_readiness_percent(experience);
    out_readiness->state = UMI_APPLICATION_LAUNCH_READINESS_READY;
    out_readiness->launchable = true;
    append_reason(out_readiness->reason, sizeof(out_readiness->reason),
                  "Default Framework layout is ready.");
    return UMI_STATUS_OK;
}

/* Return one stable label for logs, tables and GUI launch-pickers. */
const char *umi_application_launch_readiness_state_text(
    UmiApplicationLaunchReadinessState state)
{
    switch (state) {
    case UMI_APPLICATION_LAUNCH_READINESS_READY:
        return "ready";
    case UMI_APPLICATION_LAUNCH_READINESS_MISSING_EXPERIENCE:
        return "missing-experience";
    case UMI_APPLICATION_LAUNCH_READINESS_INVALID_EXPERIENCE:
        return "invalid-experience";
    case UMI_APPLICATION_LAUNCH_READINESS_NO_LAYOUT:
        return "no-layout";
    case UMI_APPLICATION_LAUNCH_READINESS_NO_PANELS:
        return "no-panels";
    case UMI_APPLICATION_LAUNCH_READINESS_UNKNOWN:
    default:
        return "unknown";
    }
}

/* Copy a diagnostic value into a fixed field without allowing a long ID to overflow it. */
static void copy_bounded_text(char *destination,
                              size_t capacity,
                              const char *source)
{
    size_t index;

    /* Leave the destination empty when the source is not available. */
    if (destination == NULL || capacity == 0U || source == NULL) return;
    for (index = 0U; index + 1U < capacity && source[index] != '\0'; ++index) {
        destination[index] = source[index];
    }
    destination[index] = '\0';
}

/* Check the whole catalogue once so every launcher sees the same portfolio truth. */
UmiStatus umi_application_launch_readiness_summary(
    UmiApplicationLaunchReadinessSummary *out_summary)
{
    size_t index;
    size_t readiness_percent_total = 0U;
    const size_t application_count = umi_application_experience_catalogue_count();

    /* Reject missing output storage before writing the summary. */
    if (out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    out_summary->structure_size = (uint32_t)sizeof(*out_summary);
    out_summary->application_count = application_count;

    /* Visit each canonical experience exactly once and retain its gate result. */
    for (index = 0U; index < application_count; ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UmiApplicationLaunchReadiness readiness;
        UmiStatus status;

        /* A missing catalogue entry is a broken Framework contract, not a launchable app. */
        if (experience == NULL || experience->application_id == NULL) {
            out_summary->blocked_count += 1U;
            out_summary->invalid_experience_count += 1U;
            if (out_summary->first_blocked_application_id[0] == '\0') {
                copy_bounded_text(out_summary->first_blocked_application_id,
                                  sizeof(out_summary->first_blocked_application_id),
                                  "<missing>");
                copy_bounded_text(out_summary->first_blocked_reason,
                                  sizeof(out_summary->first_blocked_reason),
                                  "The catalogue entry is missing an application ID.");
            }
            continue;
        }

        status = umi_application_launch_readiness_check(
            experience->application_id, &readiness);
        /* Preserve an unexpected contract error as a blocked portfolio item. */
        if (status != UMI_STATUS_OK) {
            out_summary->blocked_count += 1U;
            out_summary->invalid_experience_count += 1U;
            if (out_summary->first_blocked_application_id[0] == '\0') {
                copy_bounded_text(out_summary->first_blocked_application_id,
                                  sizeof(out_summary->first_blocked_application_id),
                                  experience->application_id);
                copy_bounded_text(out_summary->first_blocked_reason,
                                  sizeof(out_summary->first_blocked_reason),
                                  umi_status_text(status));
            }
            continue;
        }

        readiness_percent_total += readiness.feature_readiness_percent;
        if (readiness.launchable) {
            out_summary->ready_count += 1U;
            continue;
        }

        /* Keep blocked reasons grouped so a UI can show actionable totals. */
        out_summary->blocked_count += 1U;
        switch (readiness.state) {
        case UMI_APPLICATION_LAUNCH_READINESS_MISSING_EXPERIENCE:
            out_summary->missing_experience_count += 1U;
            break;
        case UMI_APPLICATION_LAUNCH_READINESS_NO_LAYOUT:
            out_summary->no_layout_count += 1U;
            break;
        case UMI_APPLICATION_LAUNCH_READINESS_NO_PANELS:
            out_summary->no_panels_count += 1U;
            break;
        case UMI_APPLICATION_LAUNCH_READINESS_INVALID_EXPERIENCE:
        case UMI_APPLICATION_LAUNCH_READINESS_UNKNOWN:
        default:
            out_summary->invalid_experience_count += 1U;
            break;
        }
        if (out_summary->first_blocked_application_id[0] == '\0') {
            copy_bounded_text(out_summary->first_blocked_application_id,
                              sizeof(out_summary->first_blocked_application_id),
                              readiness.application_id);
            copy_bounded_text(out_summary->first_blocked_reason,
                              sizeof(out_summary->first_blocked_reason),
                              readiness.reason);
        }
    }

    /* Avoid division by zero if a future build publishes an empty catalogue. */
    if (application_count > 0U) {
        out_summary->average_feature_readiness_percent =
            (unsigned)(readiness_percent_total / application_count);
    }
    return UMI_STATUS_OK;
}
