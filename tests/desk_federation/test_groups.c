/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_groups.c
 *
 * PURPOSE:
 *   Exercise saved-set validation, replacement, capacity and session capture.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

int TestGroups(const char *name, const char *path)
{
    TestFixture *fixture = calloc(1U, sizeof *fixture);
    UmiDeskWorkspace workspace = TestWorkspace("finance", 2U), read;
    UmiDeskFederationSnapshot snapshot;
    (void)path;
    CHECK(fixture != NULL); OK(TestOpen(fixture, NULL));
    if (strcmp(name, "inert_create_reload") == 0) {
        OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot));
        CHECK(snapshot.loaded && snapshot.revision == 0U && snapshot.workspaceCount == 0U);
        CHECK(umi_data_server_count(fixture->server) == 0U && fixture->host.requestCount == 0U && fixture->host.readCount == 0U);
    } else if (strcmp(name, "save_roundtrip") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationReload(fixture->service));
        OK(UmiDeskFederationWorkspaceFind(fixture->service, workspace.kind, workspace.id, &read));
        CHECK(read.memberCount == 2U && strcmp(read.title, workspace.title) == 0 && strcmp(read.members[1].value, workspace.members[1].value) == 0);
        CHECK(TestRevision(fixture->service) == 1U && fixture->host.requestCount == 0U);
    } else if (strcmp(name, "replace_and_shrink") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        workspace.memberCount = 1U; strcpy(workspace.title, "Morning finance");
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationReload(fixture->service));
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read));
        CHECK(read.memberCount == 1U && strcmp(read.title, "Morning finance") == 0);
        CHECK(TestRevision(fixture->service) == 2U && umi_data_server_count(fixture->server) == 3U);
    } else if (strcmp(name, "remove_middle") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        strcpy(workspace.id, "second"); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        strcpy(workspace.id, "third"); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationRemoveWorkspace(fixture->service, UMI_DESK_WORKSPACE_GROUP, "second"));
        OK(UmiDeskFederationReload(fixture->service));
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 1U, &read)); CHECK(strcmp(read.id, "third") == 0);
        CHECK(UmiDeskFederationWorkspaceAt(fixture->service, 2U, &read) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "remove_last") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationRemoveWorkspace(fixture->service, workspace.kind, workspace.id));
        OK(UmiDeskFederationReload(fixture->service));
        CHECK(umi_data_server_count(fixture->server) == 1U && TestRevision(fixture->service) == 2U);
    } else if (strcmp(name, "duplicate_members") == 0) {
        workspace.members[1] = workspace.members[0];
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(TestRevision(fixture->service) == 0U);
    } else if (strcmp(name, "empty_members") == 0) {
        workspace.memberCount = 0U; CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "member_capacity") == 0) {
        workspace = TestWorkspace("maximum", UMI_DESK_FEDERATION_MAX_APPS);
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        OK(UmiDeskFederationReload(fixture->service));
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(read.memberCount == 64U);
        workspace.memberCount = 65U; CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "workspace_capacity") == 0) {
        for (size_t index = 0U; index < UMI_DESK_FEDERATION_MAX_WORKSPACES; ++index) {
            (void)snprintf(workspace.id, sizeof workspace.id, "group%zu", index);
            OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        }
        strcpy(workspace.id, "overflow"); CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(workspace.id, "group0"); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
    } else if (strcmp(name, "utf8_title") == 0) {
        strcpy(workspace.title, "Finance - \xc3\x89quipe - \xd8\xb9\xd9\x85\xd9\x84");
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace)); OK(UmiDeskFederationReload(fixture->service));
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(strcmp(read.title, workspace.title) == 0);
    } else if (strcmp(name, "malformed_title") == 0) {
        const char *const bad[] = {"bad\nname", "bad\x7f", "bad\xc0\xaf", "bad\xed\xa0\x80", "bad\xf4\x90\x80\x80", "bad\xe2\x80\xa8", "bad\xc2\x80"};
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index) {
            strcpy(workspace.title, bad[index]); CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(name, "unterminated_fields") == 0) {
        memset(workspace.id, 'x', sizeof workspace.id); CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
        workspace = TestWorkspace("finance", 2U); memset(workspace.title, 'x', sizeof workspace.title);
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
        workspace = TestWorkspace("finance", 2U); memset(workspace.members[0].value, 'x', sizeof workspace.members[0].value);
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "self_rejected") == 0) {
        strcpy(workspace.members[0].value, "org.umicom.desktop");
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "group_session_namespaces") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace)); workspace.kind = UMI_DESK_WORKSPACE_SESSION;
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace)); OK(UmiDeskFederationSnapshotGet(fixture->service, &snapshot));
        CHECK(snapshot.groupCount == 1U && snapshot.sessionCount == 1U);
    } else if (strcmp(name, "capture_running") == 0) {
        fixture->host.catalogue.apps[0].running = true; fixture->host.catalogue.apps[0].state = UMI_DESK_FEDERATION_RUNNING;
        fixture->host.catalogue.apps[0].processToken = UINT64_C(987654321);
        OK(UmiDeskFederationCaptureSession(fixture->service, "morning", "Morning work"));
        OK(UmiDeskFederationWorkspaceFind(fixture->service, UMI_DESK_WORKSPACE_SESSION, "morning", &read));
        CHECK(read.memberCount == 1U && strcmp(read.members[0].value, "org.umicom.bank") == 0 && fixture->host.requestCount == 0U);
    } else if (strcmp(name, "capture_empty") == 0) {
        CHECK(UmiDeskFederationCaptureSession(fixture->service, "morning", "Morning") == UMI_STATUS_INVALID_STATE);
        CHECK(TestRevision(fixture->service) == 0U);
    } else if (strcmp(name, "capture_transient") == 0) {
        fixture->host.catalogue.apps[0].running = true; fixture->host.catalogue.apps[0].state = UMI_DESK_FEDERATION_STOPPING;
        CHECK(UmiDeskFederationCaptureSession(fixture->service, "morning", "Morning") == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "copied_ownership") == 0) {
        OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace)); memset(&workspace, 0x5A, sizeof workspace);
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(strcmp(read.id, "finance") == 0);
        memset(&read, 0x5A, sizeof read); OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(strcmp(read.id, "finance") == 0);
    } else if (strcmp(name, "invalid_kind_id") == 0) {
        workspace.kind = (UmiDeskWorkspaceKind)100; CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
        workspace = TestWorkspace("../finance", 1U); CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskFederationRemoveWorkspace(fixture->service, (UmiDeskWorkspaceKind)100, "finance") == UMI_STATUS_INVALID_ARGUMENT);
    } else { CHECK(false); }
    TestClose(fixture); free(fixture); return 0;
}
