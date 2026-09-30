/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_library_copy.c
 * PURPOSE: Check copy proposals, revision rejection and saved copy identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/workspace_library.h"
#include "umicom/ui/workspace_library_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Fixture {
    UmiUiWorkspaceCustomisation model, before, restored;
    UmiUiWorkspaceLibrarySnapshot rows, saved_rows;
    UmiUiWorkspaceLibraryCopySuggestion proposal;
} Fixture;
static const UmiUiWorkspaceLibraryPolicy policy = {"test.copy."};
static const UmiUiWorkspaceCheckpointScope scope = {"org.umicom.copy-test", "test", "test.copy."};

static int Seed(Fixture *f)
{
    umi_ui_workspace_customisation_init(&f->model);
    CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.copy.source", "Original") == UMI_STATUS_OK);
    CHECK(umi_ui_workspace_library_snapshot(&f->model, &policy, &f->rows) == UMI_STATUS_OK);
    f->before = f->model; f->saved_rows = f->rows;
    return 0;
}

/* Every rejection leaves the caller's output sentinel and all input bytes. */
static int Reject(Fixture *f, const char *target, UmiStatus expected)
{
    UmiUiWorkspaceLibraryCopySuggestion before;
    memset(&f->proposal, 0x5a, sizeof(f->proposal));
    memcpy(&before, &f->proposal, sizeof(before));
    f->saved_rows = f->rows;
    CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, target, &f->proposal) == expected);
    CHECK(memcmp(&before, &f->proposal, sizeof(before)) == 0);
    CHECK(memcmp(&f->saved_rows, &f->rows, sizeof(f->rows)) == 0);
    return 0;
}

static UmiUiWorkspaceLibraryRequest Request(const UmiUiWorkspaceLibraryCopySuggestion *proposal)
{
    UmiUiWorkspaceLibraryRequest request = {UMI_UI_WORKSPACE_LIBRARY_DUPLICATE,
        proposal->target_layout_id, proposal->new_layout_id, "My copy",
        proposal->expected_customisation_revision, false};
    return request;
}

static int Run(Fixture *f, const char *name, UmiDataServer *server)
{
    CHECK(Seed(f) == 0);
    if (strcmp(name, "suggest") == 0) {
        UmiUiWorkspaceLibraryCopySuggestion repeated;
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        CHECK(strcmp(f->proposal.new_layout_id, "test.copy.source.copy.1") == 0);
        CHECK(strcmp(f->proposal.target_layout_id, "test.copy.source") == 0);
        CHECK(f->proposal.expected_customisation_revision == f->model.revision);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &repeated) == UMI_STATUS_OK);
        CHECK(strcmp(repeated.new_layout_id, f->proposal.new_layout_id) == 0);
        CHECK(memcmp(&f->rows, &f->saved_rows, sizeof(f->rows)) == 0);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
        UmiUiWorkspaceLibraryRequest request = Request(&f->proposal);
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, &f->rows) == UMI_STATUS_OK);
        CHECK(f->rows.layout_count == 2U && f->rows.rows[1].active);
        CHECK(strcmp(f->rows.rows[1].name, "My copy") == 0);
        CHECK(memcmp(&f->model.layouts[0], &f->before.layouts[0], sizeof(f->model.layouts[0])) == 0);
    } else if (strcmp(name, "collisions") == 0) {
        CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.copy.source.copy.1", "One") == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, "test.copy.source.copy.3", "Three") == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_snapshot(&f->model, &policy, &f->rows) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        CHECK(strcmp(f->proposal.new_layout_id, "test.copy.source.copy.2") == 0);
        UmiUiWorkspaceLibraryRequest request = Request(&f->proposal);
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, &f->rows) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        CHECK(strcmp(f->proposal.new_layout_id, "test.copy.source.copy.4") == 0);
    } else if (strcmp(name, "malformed") == 0) {
        UmiUiWorkspaceLibrarySnapshot valid = f->rows;
        CHECK(umi_ui_workspace_library_suggest_copy(NULL, "x", &f->proposal) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "x", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Reject(f, NULL, UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(Reject(f, "", UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(Reject(f, "bad/id", UMI_STATUS_INVALID_ARGUMENT) == 0);
        CHECK(Reject(f, "test.copy.missing", UMI_STATUS_NOT_FOUND) == 0);
        f->rows.layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS + 1U;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; memset(f->rows.rows[0].layout_id, 'a', sizeof(f->rows.rows[0].layout_id));
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; memset(f->rows.rows[0].name, 'a', sizeof(f->rows.rows[0].name));
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; strcpy(f->rows.rows[0].name, "\xed\xa0\x80");
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; strcpy(f->rows.rows[0].layout_id, "bad/id");
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; f->rows.rows[0].active = false;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; f->rows.rows[0].window_count = UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS + 1U;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; f->rows.rows[1] = f->rows.rows[0]; f->rows.layout_count = 2U;
        f->rows.rows[1].active = false;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        strcpy(f->rows.rows[1].layout_id, "test.copy.other"); f->rows.rows[1].active = true;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_INVALID_STATE) == 0);
        f->rows = valid; f->rows.editing = true;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_BUSY) == 0);
        f->rows = valid; f->rows.layout_count = 0U;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_NOT_FOUND) == 0);
    } else if (strcmp(name, "capacity") == 0) {
        char long_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 1U];
        memset(long_id, 'a', sizeof(long_id)); long_id[sizeof(long_id) - 1U] = '\0';
        CHECK(Reject(f, long_id, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        f->rows.customisation_revision = UINT64_MAX;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        f->rows.customisation_revision = 1U;
        for (size_t n = 1U; n < UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS; ++n) {
            f->rows.rows[n] = f->rows.rows[0]; f->rows.rows[n].active = false;
            (void)snprintf(f->rows.rows[n].layout_id, sizeof(f->rows.rows[n].layout_id), "test.copy.full.%zu", n);
        }
        f->rows.layout_count = UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS;
        CHECK(Reject(f, "test.copy.source", UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        f->rows.layout_count = 1U;
        memset(f->rows.rows[0].layout_id, 'a', sizeof(f->rows.rows[0].layout_id));
        f->rows.rows[0].layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY - 1U] = '\0';
        CHECK(Reject(f, f->rows.rows[0].layout_id, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
        f->rows.rows[0].layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY - sizeof(".copy.1")] = '\0';
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, f->rows.rows[0].layout_id, &f->proposal) == UMI_STATUS_OK);
        CHECK(strlen(f->proposal.new_layout_id) == UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY - 1U);
        /* One-digit suffixes fit exactly; a collision requiring two digits
         * must fail without shortening the complete source namespace. */
        for (size_t n = 1U; n <= 9U; ++n) {
            f->rows.rows[n] = f->rows.rows[0]; f->rows.rows[n].active = false;
            const char suffix[] = {'.', 'c', 'o', 'p', 'y', '.', (char)('0' + n), '\0'};
            const size_t source_length = strlen(f->rows.rows[0].layout_id);
            CHECK(source_length + sizeof(suffix) == sizeof(f->rows.rows[n].layout_id));
            memcpy(f->rows.rows[n].layout_id + source_length, suffix, sizeof(suffix));
        }
        f->rows.layout_count = 10U;
        CHECK(Reject(f, f->rows.rows[0].layout_id, UMI_STATUS_CAPACITY_EXCEEDED) == 0);
    } else if (strcmp(name, "alias") == 0) {
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, f->rows.rows[0].layout_id, &f->proposal) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, f->proposal.target_layout_id, &f->proposal) == UMI_STATUS_OK);
        CHECK(strcmp(f->proposal.new_layout_id, "test.copy.source.copy.1") == 0);
        f->saved_rows = f->rows;
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source",
            (UmiUiWorkspaceLibraryCopySuggestion *)(void *)&f->rows) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&f->rows, &f->saved_rows, sizeof(f->rows)) == 0);
    } else if (strcmp(name, "stale") == 0) {
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        UmiUiWorkspaceLibraryRequest request = Request(&f->proposal);
        CHECK(umi_ui_workspace_customisation_create_blank_layout(&f->model, f->proposal.new_layout_id, "Other writer") == UMI_STATUS_OK);
        f->before = f->model;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, NULL) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
        request.expected_customisation_revision = f->model.revision;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, NULL) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
        CHECK(umi_ui_workspace_library_snapshot(&f->model, &policy, &f->rows) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        CHECK(strcmp(f->proposal.new_layout_id, "test.copy.source.copy.2") == 0);
    } else if (strcmp(name, "ownership") == 0) {
        const UmiUiWorkspaceLibraryPolicy wrong_owner = {"other.product."};
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        UmiUiWorkspaceLibraryRequest request = Request(&f->proposal);
        CHECK(umi_ui_workspace_library_apply(&f->model, &wrong_owner, &request, NULL) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&f->model, &f->before, sizeof(f->model)) == 0);
    } else if (strcmp(name, "checkpoint") == 0) {
        UmiUiWorkspaceLibraryCheckpointReport report;
        CHECK(umi_ui_workspace_library_suggest_copy(&f->rows, "test.copy.source", &f->proposal) == UMI_STATUS_OK);
        UmiUiWorkspaceLibraryRequest request = Request(&f->proposal);
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, NULL) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_checkpoint_save(server, &scope, &f->model, 123U, 0U, &report) == UMI_STATUS_OK);
        request.action = UMI_UI_WORKSPACE_LIBRARY_REMOVE; request.target_layout_id = f->proposal.new_layout_id;
        request.expected_customisation_revision = f->model.revision; request.confirmed = true;
        CHECK(umi_ui_workspace_library_apply(&f->model, &policy, &request, NULL) == UMI_STATUS_OK);
        CHECK(umi_ui_workspace_library_checkpoint_load_candidate(server, &scope, &f->model, &f->restored, &report) == UMI_STATUS_OK);
        CHECK(f->restored.layout_count == 2U && !report.checkpoint.durable);
        CHECK(strcmp(f->restored.active_layout_id, "test.copy.source.copy.1") == 0);
        CHECK(strcmp(f->restored.layouts[1].name, "My copy") == 0);
    } else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    UmiDataServer *server = NULL;
    if (fixture == NULL) return 1;
    if (umi_data_server_create_memory(&server) != UMI_STATUS_OK) { free(fixture); return 1; }
    int result = Run(fixture, argv[1], server);
    umi_data_server_destroy(server); free(fixture);
    return result;
}
