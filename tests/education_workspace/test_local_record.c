/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_local_record.c
 * PURPOSE: Verify explicit learning database locations and preservation of saved study records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/education_workspace/local_record.h"
#include "umicom/platform/filesystem.h"
static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 2);
    char parent[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
    FixtureDirectory(parent);
    FixturePath(path, parent, "progress caf\xc3\xa9.sqlite");
    UmiDataServer *server = NULL;
    UmiEducationWorkspace *workspace = NULL;
    const char *mode = argv[1];
    if (strcmp(mode, "invalid") == 0)
    {
        const char *invalid[] = {"", "learning.sqlite",
                                 ":memory:", "file:learning.sqlite?mode=memory"};
        for (size_t index = 0U; index < sizeof invalid / sizeof invalid[0]; ++index)
        {
            CHECK(UmiEducationLocalRecordOpenAt(invalid[index], "learner", "Study", &server,
                                                &workspace) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(server == NULL && workspace == NULL);
        }
        CHECK(UmiEducationLocalRecordOpenAt(path, "bad learner", "Study", &server, &workspace) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_fs_exists(path));
        CHECK(UmiEducationLocalRecordOpenAt(path, "learner", "", &server, &workspace) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_fs_exists(path));
        CHECK(UmiEducationLocalRecordOpenAt(parent, "learner", "Study", &server, &workspace) ==
              UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(mode, "missing-parent") == 0)
    {
        FixturePath(other, parent, "absent/learning.sqlite");
        CHECK(UmiEducationLocalRecordOpenAt(other, "learner", "Study", &server, &workspace) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(server == NULL && workspace == NULL && !umi_fs_exists(other));
        return 0;
    }
    UmiStatus status =
        UmiEducationLocalRecordOpenAt(path, "learner", "First name", &server, &workspace);
    if (status == UMI_STATUS_NOT_IMPLEMENTED)
        return 77;
    CHECK(status == UMI_STATUS_OK && server != NULL && workspace != NULL && umi_fs_is_file(path));
    const char *lesson = UmiEducationLessonAt(0U)->id;
    CHECK(UmiEducationSaveNote(workspace, lesson, "Keep this learning draft") == UMI_STATUS_OK);
    UmiEducationClose(workspace);
    umi_data_server_destroy(server);
    workspace = NULL;
    server = NULL;
    if (strcmp(mode, "corrupt") == 0)
    {
        FixturePath(other, parent, "unrelated.txt");
        CHECK(umi_fs_write_text(other, "Existing non-database material") == UMI_STATUS_OK);
        CHECK(UmiEducationLocalRecordOpenAt(other, "learner", "Study", &server, &workspace) !=
              UMI_STATUS_OK);
        CHECK(server == NULL && workspace == NULL);
        size_t length = 0U;
        unsigned char *bytes = FixtureRead(other, &length);
        CHECK(length == strlen("Existing non-database material") &&
              memcmp(bytes, "Existing non-database material", length) == 0);
        free(bytes);
    }
    else if (strcmp(mode, "independent") == 0)
    {
        FixturePath(other, parent, "other.sqlite");
        CHECK(UmiEducationLocalRecordOpenAt(other, "learner", "Second file", &server, &workspace) ==
              UMI_STATUS_OK);
        UmiEducationProgress progress;
        CHECK(UmiEducationProgressRead(workspace, lesson, &progress) == UMI_STATUS_OK &&
              progress.note[0] == '\0');
        UmiEducationClose(workspace);
        umi_data_server_destroy(server);
        workspace = NULL;
        server = NULL;
    }
    else
        CHECK(strcmp(mode, "reopen") == 0);
    CHECK(UmiEducationLocalRecordOpenAt(path, "learner", "Different requested name", &server,
                                        &workspace) == UMI_STATUS_OK);
    UmiEducationProgress progress;
    UmiEducationSnapshot snapshot;
    CHECK(UmiEducationProgressRead(workspace, lesson, &progress) == UMI_STATUS_OK);
    CHECK(strcmp(progress.note, "Keep this learning draft") == 0);
    CHECK(UmiEducationSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK);
    CHECK(strcmp(snapshot.displayName, "First name") == 0 && snapshot.revision == 1U);
    UmiEducationClose(workspace);
    umi_data_server_destroy(server);
    return 0;
}
#include "../native_process/utf8_entry.inc"
