/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Isolated fixtures retain storage evidence and never overwrite user files. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/data/record_draft_files.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY]; FixtureDirectory(root);
    FixturePath(path, root, "kept.umird");
    UmiRecordDraft *draft = NULL;
    UmiRecordDraftSnapshot *snapshot = calloc(1U, sizeof *snapshot), *loaded = calloc(1U, sizeof *loaded), *guard = calloc(1U, sizeof *guard);
    CHECK(snapshot && loaded && guard);
    CHECK(umi_record_draft_create(&draft) == UMI_STATUS_OK);
    CHECK(umi_record_draft_begin(draft, "fixture.new") == UMI_STATUS_OK);
    CHECK(umi_record_draft_set_content(draft, "unsaved\r\ntext", 13U) == UMI_STATUS_OK);
    CHECK(umi_record_draft_snapshot_capture(draft, snapshot) == UMI_STATUS_OK);
    memset(loaded, 0xa5, sizeof *loaded); *guard = *loaded;
    if (strcmp(mode, "roundtrip") == 0 || strcmp(mode, "exclusive") == 0 || strcmp(mode, "unicode") == 0) {
        const char *leaf = strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9.umird" : "kept.umird";
        FixturePath(path, root, leaf);
        CHECK(umi_record_draft_snapshot_write_new(path, snapshot) == UMI_STATUS_OK);
        if (strcmp(mode, "exclusive") == 0) {
            snapshot->content[0] = 'X';
            CHECK(umi_record_draft_snapshot_write_new(path, snapshot) == UMI_STATUS_ALREADY_EXISTS);
        }
        CHECK(umi_record_draft_snapshot_read(root, leaf, loaded) == UMI_STATUS_OK);
        CHECK(loaded->length == 13U && memcmp(loaded->content, "unsaved\r\ntext", 13U) == 0);
    } else if (strcmp(mode, "malformed") == 0 || strcmp(mode, "oversized") == 0) {
        size_t count = strcmp(mode, "malformed") == 0 ? 32U : UMI_RECORD_DRAFT_SNAPSHOT_MAX_BYTES + 1U;
        unsigned char *bytes = calloc(count, 1U); CHECK(bytes);
        UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, bytes, count) == UMI_STATUS_OK); CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file); free(bytes);
        CHECK(umi_record_draft_snapshot_read(root, "kept.umird", loaded) != UMI_STATUS_OK);
        CHECK(memcmp(loaded, guard, sizeof *guard) == 0);
    } else if (strcmp(mode, "invalid") == 0) {
        CHECK(umi_record_draft_snapshot_write_new("relative.umird", snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_snapshot_read(root, "../kept.umird", loaded) != UMI_STATUS_OK);
        CHECK(memcmp(loaded, guard, sizeof *guard) == 0);
        snapshot->length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1U;
        CHECK(umi_record_draft_snapshot_write_new(path, snapshot) == UMI_STATUS_CAPACITY_EXCEEDED);
        int exists = 1; CHECK(UmiRootedFileExists(root, "kept.umird", &exists) == UMI_STATUS_OK && !exists);
    } else if (strcmp(mode, "missing") == 0) {
        CHECK(umi_record_draft_snapshot_read(root, "absent.umird", loaded) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(loaded, guard, sizeof *guard) == 0);
        char absent[UMI_PATH_CAPACITY]; FixturePath(absent, root, "absent"); FixturePath(path, absent, "kept.umird");
        CHECK(umi_record_draft_snapshot_write_new(path, snapshot) != UMI_STATUS_OK);
    } else if (strcmp(mode, "directory") == 0) {
        CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
        CHECK(umi_record_draft_snapshot_write_new(path, snapshot) != UMI_STATUS_OK);
        CHECK(umi_record_draft_snapshot_read(root, "kept.umird", loaded) != UMI_STATUS_OK);
        CHECK(memcmp(loaded, guard, sizeof *guard) == 0);
    } else CHECK(0 && "Unknown recovery storage case");
    free(snapshot); free(loaded); free(guard); umi_record_draft_destroy(draft);
    return 0;
}
