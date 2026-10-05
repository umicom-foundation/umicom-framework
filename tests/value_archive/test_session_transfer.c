/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/test_session_transfer.c
 * PURPOSE: Verify canonical panel lifetimes and transactional session restoration.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/session_snapshot.h"
#include "umicom/application/experience_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

/* The fixture uses real canonical metadata. No GTK windows, process or broker
 * are created, so session lifetime is checked independently of presentation. */
static const UmiApplicationExperienceDefinition *experience(void)
{
    return umi_application_experience_catalogue_find("org.umicom.trader");
}
static int lifetime(void)
{
    UmiApplicationSession session, restored;
    UmiApplicationSessionSnapshot snapshot;
    CHECK(umi_application_session_init(experience(), &session) == UMI_STATUS_OK);
    CHECK(umi_application_session_snapshot_capture(&session, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.active_panel_count > 0U);
    CHECK(umi_application_session_snapshot_restore(experience(), &snapshot, &restored) == UMI_STATUS_OK);
    for (size_t index = 0U; index < restored.active_panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience(), snapshot.active_panel_ids[index]);
        CHECK(panel != NULL && restored.active_panel_ids[index] == panel->panel_id);
    }
    /* Destroying snapshot contents used to invalidate every restored ID. */
    memset(&snapshot, 0xa5, sizeof(snapshot));
    CHECK(umi_application_session_validate(&restored) == UMI_STATUS_OK);
    char requested[UMI_APPLICATION_RUNTIME_TEXT_CAPACITY];
    (void)snprintf(requested, sizeof(requested), "%s", restored.active_panel_ids[0]);
    CHECK(umi_application_session_deactivate_panel(&restored, requested) == UMI_STATUS_OK);
    CHECK(umi_application_session_activate_panel(&restored, requested) == UMI_STATUS_OK);
    memset(requested, 0xa5, sizeof(requested));
    CHECK(umi_application_session_validate(&restored) == UMI_STATUS_OK);
    return 0;
}
static int refused_snapshot(void)
{
    UmiApplicationSession session;
    UmiApplicationSessionSnapshot snapshot;
    CHECK(umi_application_session_init(experience(), &session) == UMI_STATUS_OK);
    CHECK(umi_application_session_snapshot_capture(&session, &snapshot) == UMI_STATUS_OK);
    unsigned char before[sizeof(session)]; memcpy(before, &session, sizeof(session));
    CHECK(snapshot.active_panel_count > 1U);
    size_t last = snapshot.active_panel_count - 1U;
    (void)snprintf(snapshot.active_panel_ids[last], sizeof(snapshot.active_panel_ids[last]), "missing-panel");
    CHECK(umi_application_session_snapshot_restore(experience(), &snapshot, &session) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    memcpy(snapshot.active_panel_ids[last], snapshot.active_panel_ids[0], sizeof(snapshot.active_panel_ids[last]));
    CHECK(umi_application_session_snapshot_restore(experience(), &snapshot, &session) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    memset(snapshot.application_id, 'x', sizeof(snapshot.application_id));
    CHECK(umi_application_session_snapshot_restore(experience(), &snapshot, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    CHECK(umi_application_session_snapshot_capture(&session, &snapshot) == UMI_STATUS_OK);
    snapshot.active_panel_count = UMI_APPLICATION_RUNTIME_MAX_PANELS + 1U;
    CHECK(umi_application_session_snapshot_restore(experience(), &snapshot, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    return 0;
}
static int archive_review(void)
{
    UmiApplicationSession session, preview;
    UmiApplicationSessionSnapshot snapshot;
    unsigned char bytes[16384];
    size_t size = 0U;
    CHECK(umi_application_session_init(experience(), &session) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_capture(&session, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_preview(experience(), bytes, size, &preview) == UMI_STATUS_OK);
    CHECK(preview.active_panel_count == session.active_panel_count);
    uint64_t observed = session.revision;
    CHECK(umi_application_session_snapshot_capture(&session, &snapshot) == UMI_STATUS_OK);
    snapshot.revision = UINT64_MAX;
    CHECK(umi_application_session_snapshot_archive_encode(&snapshot, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_apply(&session, observed, bytes, size) == UMI_STATUS_OK);
    CHECK(session.revision == observed + 1U);
    unsigned char before[sizeof(session)]; memcpy(before, &session, sizeof(session));
    CHECK(umi_application_session_archive_apply(&session, observed, bytes, size) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    bytes[size - 1U] ^= 1U;
    CHECK(umi_application_session_archive_apply(&session, session.revision, bytes, size) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    bytes[size - 1U] ^= 1U;
    CHECK(umi_application_session_archive_preview(
        umi_application_experience_catalogue_find("org.umicom.studio"), bytes, size, &session) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    /* A valid envelope still cannot introduce a panel unknown to this host. */
    (void)snprintf(snapshot.active_panel_ids[snapshot.active_panel_count - 1U],
        UMI_APPLICATION_RUNTIME_TEXT_CAPACITY, "unavailable-panel");
    CHECK(umi_application_session_snapshot_archive_encode(&snapshot, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_apply(&session, session.revision, bytes, size) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    return 0;
}
/* A reviewed archive still respects the user's current layout lock. */
static int locked_session(void)
{
    UmiApplicationSession session;
    unsigned char bytes[16384];
    size_t size = 0U;
    CHECK(umi_application_session_init(experience(), &session) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_capture(&session, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    CHECK(umi_application_session_set_layout_locked(&session, true) == UMI_STATUS_OK);
    unsigned char before[sizeof(session)]; memcpy(before, &session, sizeof(session));
    CHECK(umi_application_session_archive_apply(&session, session.revision, bytes, size) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    uint64_t locked_revision = session.revision;
    CHECK(umi_application_session_set_layout_locked(&session, false) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_apply(&session, locked_revision, bytes, size) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_application_session_archive_apply(&session, session.revision, bytes, size) == UMI_STATUS_OK);
    return 0;
}
static int exhausted_revision(void)
{
    UmiApplicationSession session;
    unsigned char bytes[16384];
    size_t size = 0U;
    CHECK(umi_application_session_init(experience(), &session) == UMI_STATUS_OK);
    CHECK(umi_application_session_archive_capture(&session, bytes, sizeof(bytes), &size) == UMI_STATUS_OK);
    session.revision = UINT64_MAX;
    unsigned char before[sizeof(session)]; memcpy(before, &session, sizeof(session));
    CHECK(umi_application_session_archive_apply(&session, UINT64_MAX, bytes, size) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    CHECK(umi_application_session_set_layout_locked(&session, !session.layout_locked) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    CHECK(umi_application_session_deactivate_panel(&session, session.active_panel_ids[0]) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    CHECK(umi_application_session_select_layout(&session, session.layout->layout_id) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, &session, sizeof(session)) == 0);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "lifetime") == 0) return lifetime();
    if (strcmp(argv[1], "refused") == 0) return refused_snapshot();
    if (strcmp(argv[1], "archive") == 0) return archive_review();
    if (strcmp(argv[1], "locked") == 0) return locked_session();
    if (strcmp(argv[1], "exhausted") == 0) return exhausted_revision();
    return 2;
}
