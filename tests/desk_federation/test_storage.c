/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desk_federation/test_storage.c
 *
 * PURPOSE:
 *   Test rollback, stale writers and corrupt snapshots using actual Data Server storage.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

static UmiStatus StoreText(UmiDataServer *server, const char *key, const char *text)
{
    return umi_data_server_set(server, key, text);
}
int TestStorage(const char *name, const char *path)
{
    TestFixture *fixture = calloc(1U, sizeof *fixture);
    UmiDeskWorkspace workspace = TestWorkspace("finance", 2U), read;
    UmiDeskFederationPlan plan;
    UmiDeskFederationReport report;
    bool sqlite = strncmp(name, "sqlite_", 7U) == 0;
    UmiStatus status;
    CHECK(fixture != NULL);
    if (sqlite) { CHECK(path != NULL); (void)remove(path); }
    status = TestOpen(fixture, sqlite ? path : NULL);
    if (sqlite && status == UMI_STATUS_UNAVAILABLE) { free(fixture); return 77; }
    OK(status); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
    if (strcmp(name, "borrowed_transaction") == 0) {
        OK(umi_data_server_begin(fixture->server));
        strcpy(workspace.title, "Uncommitted caller data");
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(fixture->server) && TestRevision(fixture->service) == 1U);
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_BUSY);
        OK(umi_data_server_rollback(fixture->server));
    } else if (strcmp(name, "other_namespace_preserved") == 0) {
        char text[80]; OK(StoreText(fixture->server, "bank/reference", "not-a-desk-record"));
        OK(UmiDeskFederationRemoveWorkspace(fixture->service, workspace.kind, workspace.id));
        OK(umi_data_server_get(fixture->server, "bank/reference", text, sizeof text)); CHECK(strcmp(text, "not-a-desk-record") == 0);
    } else if (strcmp(name, "corrupt_header") == 0) {
        OK(StoreText(fixture->server, "desk-federation/workspace/0", "DF2|1|finance|4142|2"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
        OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(strcmp(read.title, workspace.title) == 0 && TestRevision(fixture->service) == 1U);
    } else if (strcmp(name, "missing_member") == 0) {
        OK(umi_data_server_delete(fixture->server, "desk-federation/workspace/0/member/1"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "missing_meta") == 0) {
        OK(umi_data_server_delete(fixture->server, "desk-federation/meta"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "extra_namespace_record") == 0) {
        OK(StoreText(fixture->server, "desk-federation/orphan", "unexpected"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "numeric_meta_rejected") == 0) {
        const char *const bad[] = {"DF1|-1|1", "DF1|01|1", "DF1|18446744073709551616|1", "DF1|1|25", "DF1|1|1|extra", "DF2|1|1"};
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index) {
            OK(StoreText(fixture->server, "desk-federation/meta", bad[index])); CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
        }
    } else if (strcmp(name, "duplicate_stored_members") == 0) {
        OK(StoreText(fixture->server, "desk-federation/workspace/0/member/1", "org.umicom.bank"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "bad_hex_title") == 0) {
        const char *const bad[] = {"DF1|1|finance|0|2", "DF1|1|finance|00|2", "DF1|1|finance|gg|2", "DF1|1|finance|c0af|2", "DF1|1|finance|4142|2|extra"};
        for (size_t index = 0U; index < sizeof bad / sizeof bad[0]; ++index) {
            OK(StoreText(fixture->server, "desk-federation/workspace/0", bad[index])); CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
        }
    } else if (strcmp(name, "revision_exhaustion") == 0) {
        OK(StoreText(fixture->server, "desk-federation/meta", "DF1|18446744073709551615|1"));
        OK(UmiDeskFederationReload(fixture->service));
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(name, "reload_cancels_review") == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        OK(StoreText(fixture->server, "desk-federation/meta", "BROKEN"));
        CHECK(UmiDeskFederationReload(fixture->service) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "same_revision_damage_blocks_review") == 0) {
        OK(StoreText(fixture->server, "desk-federation/workspace/0/member/1", "org.umicom.exchange"));
        CHECK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan) == UMI_STATUS_PARSE_ERROR);
        CHECK(fixture->host.requestCount == 0U);
    } else if (strcmp(name, "same_revision_damage_blocks_execute") == 0) {
        OK(UmiDeskFederationPrepareWorkspace(fixture->service, workspace.kind, workspace.id, &plan));
        OK(StoreText(fixture->server, "desk-federation/workspace/0/member/1", "org.umicom.exchange"));
        CHECK(UmiDeskFederationExecute(fixture->service, plan.token, &report) == UMI_STATUS_PARSE_ERROR);
        CHECK(fixture->host.requestCount == 0U);
    } else if (strcmp(name, "same_revision_damage_blocks_write") == 0) {
        char value[160];
        OK(StoreText(fixture->server, "desk-federation/workspace/0/member/1", "org.umicom.exchange"));
        strcpy(workspace.title, "Must not mask external damage");
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_PARSE_ERROR);
        CHECK(TestRevision(fixture->service) == 1U);
        OK(umi_data_server_get(fixture->server, "desk-federation/workspace/0/member/1", value, sizeof value));
        CHECK(strcmp(value, "org.umicom.exchange") == 0);
    } else if (strcmp(name, "data_server_copy_boundaries") == 0) {
        char original[4096], result[4097];
        memset(original, 'x', sizeof original - 1U); original[sizeof original - 1U] = '\0';
        OK(StoreText(fixture->server, "copy-test/full", original));
        for (size_t capacity = 1U; capacity < sizeof original; ++capacity) {
            memset(result, 'q', sizeof result);
            CHECK(umi_data_server_get(fixture->server, "copy-test/full", result, capacity) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(result[0] == '\0' && result[1] == 'q' && result[capacity] == 'q');
        }
        memset(result, 'q', sizeof result);
        OK(umi_data_server_get(fixture->server, "copy-test/full", result, sizeof original));
        CHECK(strcmp(result, original) == 0 && result[4096] == 'q');
        OK(StoreText(fixture->server, "copy-test/empty", ""));
        OK(umi_data_server_get(fixture->server, "copy-test/empty", result, 1U)); CHECK(result[0] == '\0');
        CHECK(umi_data_server_get(fixture->server, "copy-test/empty", result, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "sqlite_restart") == 0) {
        TestClose(fixture); OK(TestOpen(fixture, path)); OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read));
        CHECK(strcmp(read.id, "finance") == 0 && read.memberCount == 2U && TestRevision(fixture->service) == 1U);
    } else if (strcmp(name, "sqlite_stale_writer") == 0 || strcmp(name, "sqlite_stale_review") == 0) {
        TestFixture *other = calloc(1U, sizeof *other); CHECK(other != NULL); OK(TestOpen(other, path));
        OK(UmiDeskFederationPrepareWorkspace(other->service, workspace.kind, workspace.id, &plan));
        strcpy(workspace.title, "Changed elsewhere"); OK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace));
        if (strcmp(name, "sqlite_stale_writer") == 0) {
            strcpy(workspace.title, "Stale overwrite"); CHECK(UmiDeskFederationSaveWorkspace(other->service, &workspace) == UMI_STATUS_BUSY);
            CHECK(TestRevision(other->service) == 1U);
        } else {
            CHECK(UmiDeskFederationExecute(other->service, plan.token, &report) == UMI_STATUS_BUSY);
            CHECK(other->host.requestCount == 0U);
        }
        OK(UmiDeskFederationReload(other->service)); OK(UmiDeskFederationWorkspaceAt(other->service, 0U, &read));
        CHECK(strcmp(read.title, "Changed elsewhere") == 0);
        TestClose(other); free(other);
    } else if (strcmp(name, "sqlite_write_rollback") == 0) {
        OK(umi_data_server_execute(fixture->server,
            "CREATE TRIGGER desk_fail BEFORE INSERT ON umicom_kv WHEN NEW.key='desk-federation/meta' BEGIN SELECT RAISE(ABORT,'injected failure'); END;"));
        strcpy(workspace.title, "Must not be committed");
        CHECK(UmiDeskFederationSaveWorkspace(fixture->service, &workspace) == UMI_STATUS_IO_ERROR);
        CHECK(!umi_data_server_in_transaction(fixture->server) && TestRevision(fixture->service) == 1U);
        OK(umi_data_server_execute(fixture->server, "DROP TRIGGER desk_fail;"));
        OK(UmiDeskFederationReload(fixture->service)); OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read));
        CHECK(strcmp(read.title, "Finance work") == 0);
    } else if (strcmp(name, "sqlite_delete_rollback") == 0) {
        OK(umi_data_server_execute(fixture->server,
            "CREATE TRIGGER desk_fail BEFORE DELETE ON umicom_kv WHEN OLD.key='desk-federation/workspace/0/member/1' BEGIN SELECT RAISE(ABORT,'injected failure'); END;"));
        CHECK(UmiDeskFederationRemoveWorkspace(fixture->service, workspace.kind, workspace.id) == UMI_STATUS_IO_ERROR);
        OK(umi_data_server_execute(fixture->server, "DROP TRIGGER desk_fail;"));
        OK(UmiDeskFederationReload(fixture->service)); OK(UmiDeskFederationWorkspaceAt(fixture->service, 0U, &read)); CHECK(read.memberCount == 2U);
    } else CHECK(false);
    TestClose(fixture); free(fixture);
    if (sqlite) (void)remove(path);
    return 0;
}
