/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_lifecycle.c
 * PURPOSE: Implement track requested application lifecycle intent and bounded transition state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_lifecycle.h"
#include "umicom/base/text.h"
#include "../../base/record_update_internal.h"

#include <string.h>

/*
 * Initialise desktop application lifecycle from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_application_lifecycle_init(UmiDesktopApplicationLifecycleSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_LIFECYCLE_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop application lifecycle satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_desktop_application_lifecycle_validate(const UmiDesktopApplicationLifecycleSnapshot *value)
{
    /* Validate each fixed field before any domain helper treats it as a C
     * string. Add new inline text members here when extending this record. */
    if (value == NULL || memchr(value->id, '\0', sizeof(value->id)) == NULL ||
        memchr(value->subject_id, '\0', sizeof(value->subject_id)) == NULL ||
        memchr(value->detail, '\0', sizeof(value->detail)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_LIFECYCLE_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set subject operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_desktop_application_lifecycle_set_subject(UmiDesktopApplicationLifecycleSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}
#endif
UmiStatus umi_desktop_application_lifecycle_set_subject(UmiDesktopApplicationLifecycleSnapshot *value, const char *subject_id)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Subject identities keep their existing nonempty, bounded contract. */
    if (subject_id[0] == '\0' || !UmiRecordTextFits(subject_id, sizeof(value->subject_id)))
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->subject_id, sizeof(value->subject_id),
                           subject_id, &value->revision);
}

/*
 * Provide the desktop application lifecycle set detail operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_desktop_application_lifecycle_set_detail(UmiDesktopApplicationLifecycleSnapshot *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}
#endif
UmiStatus umi_desktop_application_lifecycle_set_detail(UmiDesktopApplicationLifecycleSnapshot *value, const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}

/*
 * Provide the desktop application lifecycle set state operation used by this module and
 * its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_desktop_application_lifecycle_set_state(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_desktop_application_lifecycle_set_state(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t state)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set priority operation used by this module and
 * its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_desktop_application_lifecycle_set_priority(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t priority)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_desktop_application_lifecycle_set_priority(UmiDesktopApplicationLifecycleSnapshot *value, uint32_t priority)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle set enabled operation used by this module and
 * its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_desktop_application_lifecycle_set_enabled(UmiDesktopApplicationLifecycleSnapshot *value, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_desktop_application_lifecycle_set_enabled(UmiDesktopApplicationLifecycleSnapshot *value, bool enabled)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop application lifecycle same identity operation used by this module
 * and its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_desktop_application_lifecycle_same_identity(const UmiDesktopApplicationLifecycleSnapshot *left, const UmiDesktopApplicationLifecycleSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}
#endif
bool umi_desktop_application_lifecycle_same_identity(const UmiDesktopApplicationLifecycleSnapshot *left, const UmiDesktopApplicationLifecycleSnapshot *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop application lifecycle transition allowed operation used by this
 * module and its client applications.
 */
bool umi_desktop_application_lifecycle_transition_allowed(UmiDesktopControlLifecycleState from_state, UmiDesktopControlLifecycleState to_state)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (from_state == to_state) return true;
    /* Select the behaviour associated with the requested command or state value. */
    switch (from_state) {
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_RUNNING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_RUNNING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPING:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED;
        case UMI_DESKTOP_CONTROL_LIFECYCLE_FAILED:
            return to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STARTING ||
                   to_state == UMI_DESKTOP_CONTROL_LIFECYCLE_STOPPED;
        default: return false;
    }
}

/* A caller can reject an invalid application lifecycle identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_desktop_application_lifecycle_init_checked,
    UmiDesktopApplicationLifecycleSnapshot, umi_desktop_application_lifecycle_init, umi_desktop_application_lifecycle_validate)
