/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/session_snapshot.c
 *
 * PURPOSE:
 *   Capture and restore bounded session state while validating every panel against canonical metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/session_snapshot.h"
#include "../../base/value_archive_internal.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *target, size_t capacity, const char *source)
{
    int written;
    /* Configure the optional target only when its feature has created it. */
    if (target == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(target, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/*
 * Provide the application session snapshot capture operation used by this module and its
 * client applications.
 */
/* These direct-output implementations are retained for engineering review.
 * The replacements below stage a complete candidate and borrow panel strings
 * from canonical metadata. A bad later panel can no longer partly replace a
 * live session or leave it pointing into a temporary snapshot. */
#if 0
UmiStatus umi_application_session_snapshot_capture(
    const UmiApplicationSession *session,
    UmiApplicationSessionSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_session_validate(session) != UMI_STATUS_OK || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = sizeof(*out_snapshot);
    result = copy_text(out_snapshot->application_id, sizeof(out_snapshot->application_id),
                       session->experience->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    result = copy_text(out_snapshot->layout_id, sizeof(out_snapshot->layout_id),
                       session->layout->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->active_panel_count; ++index) {
        result = copy_text(out_snapshot->active_panel_ids[index],
                           sizeof(out_snapshot->active_panel_ids[index]),
                           session->active_panel_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    out_snapshot->active_panel_count = session->active_panel_count;
    out_snapshot->layout_locked = session->layout_locked;
    out_snapshot->revision = session->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the application session snapshot restore operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_snapshot_restore(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationSessionSnapshot *snapshot,
    UmiApplicationSession *out_session)
{
    size_t index;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || snapshot == NULL || out_session == NULL ||
        snapshot->structure_size != sizeof(*snapshot) ||
        strcmp(snapshot->application_id, experience->application_id) != 0 ||
        snapshot->active_panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_session_init(experience, out_session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    out_session->layout_locked = false;
    result = umi_application_session_select_layout(out_session, snapshot->layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    out_session->active_panel_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot->active_panel_count; ++index) {
        result = umi_application_session_activate_panel(
            out_session, snapshot->active_panel_ids[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
    }
    out_session->layout_locked = snapshot->layout_locked;
    out_session->revision = snapshot->revision;
    return UMI_STATUS_OK;
}

#endif

/* Validate the fixed arrays before any string lookup. The unused panel slots
 * are deliberately ignored: only active entries belong to the saved value. */
static UmiStatus snapshot_shape(const UmiApplicationSessionSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->structure_size != sizeof(*snapshot) ||
        snapshot->active_panel_count > UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(snapshot->application_id, '\0', sizeof(snapshot->application_id)) == NULL ||
        memchr(snapshot->layout_id, '\0', sizeof(snapshot->layout_id)) == NULL ||
        snapshot->application_id[0] == '\0' || snapshot->layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < snapshot->active_panel_count; ++index) {
        const char *id = snapshot->active_panel_ids[index];
        if (memchr(id, '\0', sizeof(snapshot->active_panel_ids[index])) == NULL || id[0] == '\0')
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < index; ++previous)
            if (strcmp(id, snapshot->active_panel_ids[previous]) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}

/* Capturing a session does not publish a partial description if a catalogue
 * identity is too large. All strings are copied into the local candidate. */
UmiStatus umi_application_session_snapshot_capture(
    const UmiApplicationSession *session, UmiApplicationSessionSnapshot *out_snapshot)
{
    if (out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_application_session_validate(session);
    if (status != UMI_STATUS_OK) return status;
    UmiApplicationSessionSnapshot candidate = {0};
    candidate.structure_size = (uint32_t)sizeof(candidate);
    status = copy_text(candidate.application_id, sizeof(candidate.application_id), session->experience->application_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(candidate.layout_id, sizeof(candidate.layout_id), session->layout->layout_id);
    for (size_t index = 0U; status == UMI_STATUS_OK && index < session->active_panel_count; ++index)
        status = copy_text(candidate.active_panel_ids[index], sizeof(candidate.active_panel_ids[index]), session->active_panel_ids[index]);
    if (status != UMI_STATUS_OK) return status;
    candidate.active_panel_count = session->active_panel_count;
    candidate.layout_locked = session->layout_locked;
    candidate.revision = session->revision;
    status = snapshot_shape(&candidate);
    if (status == UMI_STATUS_OK) *out_snapshot = candidate;
    return status;
}

/* Resolve the entire saved session against this host's immutable catalogue.
 * The resulting panel pointers belong to that catalogue, never to snapshot.
 * This compatibility API retains the snapshot revision as descriptive data;
 * use archive_apply for a checked update of an already live session. */
UmiStatus umi_application_session_snapshot_restore(
    const UmiApplicationExperienceDefinition *experience,
    const UmiApplicationSessionSnapshot *snapshot, UmiApplicationSession *out_session)
{
    if (experience == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = snapshot_shape(snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(snapshot->application_id, experience->application_id) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationSession candidate;
    status = umi_application_session_init(experience, &candidate);
    if (status != UMI_STATUS_OK) return status;
    candidate.layout_locked = false;
    status = umi_application_session_select_layout(&candidate, snapshot->layout_id);
    if (status != UMI_STATUS_OK) return status;
    candidate.active_panel_count = 0U;
    memset(candidate.active_panel_ids, 0, sizeof(candidate.active_panel_ids));
    for (size_t index = 0U; index < snapshot->active_panel_count; ++index) {
        status = umi_application_session_activate_panel(&candidate, snapshot->active_panel_ids[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    candidate.layout_locked = snapshot->layout_locked;
    candidate.revision = snapshot->revision;
    *out_session = candidate;
    return UMI_STATUS_OK;
}

/* Counts precede variable arrays so the decoder can refuse impossible sizes
 * before indexing them. The schema incorporates public capacities, not ABI
 * padding. Extending the layout requires a deliberately new schema identity. */
static uint64_t session_archive_schema(void)
{
    return ((UINT64_C(0x5c27969c7c297b0d) ^ (uint64_t)UMI_APPLICATION_RUNTIME_MAX_PANELS) *
        UINT64_C(1099511628211)) ^ (uint64_t)UMI_APPLICATION_RUNTIME_TEXT_CAPACITY;
}
static size_t session_archive_bound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE + 24U +
        (2U + UMI_APPLICATION_RUNTIME_MAX_PANELS) * (8U + UMI_APPLICATION_RUNTIME_TEXT_CAPACITY - 1U);
}
static void session_archive_write(UmiArchiveWriter *writer, const UmiApplicationSessionSnapshot *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->layout_id, sizeof(value->layout_id));
    UmiArchiveWriteUnsigned(writer, value->active_panel_count);
    for (size_t index = 0U; index < value->active_panel_count; ++index)
        UmiArchiveWriteText(writer, value->active_panel_ids[index], sizeof(value->active_panel_ids[index]));
    UmiArchiveWriteUnsigned(writer, value->layout_locked ? 1U : 0U);
    UmiArchiveWriteUnsigned(writer, value->revision);
}
static void session_archive_read(UmiArchiveReader *reader, UmiApplicationSessionSnapshot *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->layout_id, sizeof(value->layout_id));
    value->active_panel_count = (size_t)UmiArchiveReadUnsigned(reader, UMI_APPLICATION_RUNTIME_MAX_PANELS);
    for (size_t index = 0U; reader->status == UMI_STATUS_OK && index < value->active_panel_count; ++index)
        UmiArchiveReadText(reader, value->active_panel_ids[index], sizeof(value->active_panel_ids[index]));
    value->layout_locked = UmiArchiveReadUnsigned(reader, 1U) != 0U;
    value->revision = UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_application_session_snapshot_archive_encode,
    umi_application_session_snapshot_archive_decode, UmiApplicationSessionSnapshot,
    session_archive_schema, session_archive_bound, session_archive_write,
    session_archive_read, snapshot_shape)

/* These operations expose the same ownership rules to thin products. A
 * preview owns no widgets, processes or broker connection and makes no I/O. */
UmiStatus umi_application_session_archive_capture(const UmiApplicationSession *session,
    void *bytes, size_t capacity, size_t *out_size)
{
    UmiApplicationSessionSnapshot snapshot;
    UmiStatus status = umi_application_session_snapshot_capture(session, &snapshot);
    return status == UMI_STATUS_OK
        ? umi_application_session_snapshot_archive_encode(&snapshot, bytes, capacity, out_size) : status;
}
UmiStatus umi_application_session_archive_preview(const UmiApplicationExperienceDefinition *experience,
    const void *bytes, size_t byte_count, UmiApplicationSession *out_candidate)
{
    UmiApplicationSessionSnapshot snapshot;
    UmiStatus status = umi_application_session_snapshot_archive_decode(bytes, byte_count, &snapshot);
    return status == UMI_STATUS_OK
        ? umi_application_session_snapshot_restore(experience, &snapshot, out_candidate) : status;
}
UmiStatus umi_application_session_archive_apply(UmiApplicationSession *session,
    uint64_t expected_revision, const void *bytes, size_t byte_count)
{
    UmiStatus status = umi_application_session_validate(session);
    if (status != UMI_STATUS_OK) return status;
    if (session->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    if (expected_revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Import follows the same layout lock as ordinary selection. The host
     * must unlock explicitly and obtain a new observation before replacing
     * this session; saved bytes cannot bypass the user's current lock. */
    if (session->layout_locked) return UMI_STATUS_PERMISSION_DENIED;
    UmiApplicationSession candidate;
    status = umi_application_session_archive_preview(session->experience, bytes, byte_count, &candidate);
    if (status != UMI_STATUS_OK) return status;
    /* The saved counter cannot authorize replacement. Publication advances
     * the local counter observed by the host, even if the archive is older. */
    candidate.revision = expected_revision + 1U;
    *session = candidate;
    return UMI_STATUS_OK;
}
