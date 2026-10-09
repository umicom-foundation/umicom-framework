/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_project_workflow.c
 * PURPOSE: Verify exported lesson metadata and exclusive file creation without running a compiler.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/developer_project/new_project.h"
#include "umicom/education_workspace/project_workflow.h"
#include "umicom/platform/filesystem.h"

static int ProgramMain(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(destination, root, "lesson caf\xc3\xa9");
    UmiEducationProjectWorkflow *workflow = calloc(1U, sizeof *workflow);
    UmiEducationProjectWorkflow *sentinel = calloc(1U, sizeof *sentinel);
    CHECK(workflow != NULL && sentinel != NULL);
    memset(workflow, 0x5b, sizeof *workflow);
    *sentinel = *workflow;
    size_t written = 99U;
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiEducationProjectExportForDevelopment("notes", "relative", &written, workflow) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(written == 0U && memcmp(workflow, sentinel, sizeof *workflow) == 0);
        CHECK(!umi_fs_exists(destination));
        CHECK(UmiEducationProjectWorkflowDescribe("unknown", destination, workflow) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(workflow, sentinel, sizeof *workflow) == 0);
        CHECK(UmiEducationProjectWorkflowDescribe(NULL, destination, workflow) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEducationProjectWorkflowDescribe("notes", destination, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "existing") == 0)
    {
        CHECK(umi_fs_make_directories(destination) == UMI_STATUS_OK);
        FixturePath(source, destination, "keep.txt");
        CHECK(umi_fs_write_text(source, "keep my work") == UMI_STATUS_OK);
        CHECK(UmiEducationProjectExportForDevelopment("notes", destination, &written, workflow) ==
              UMI_STATUS_ALREADY_EXISTS);
        CHECK(written == 0U && memcmp(workflow, sentinel, sizeof *workflow) == 0);
        size_t bytes = 0U;
        unsigned char *contents = FixtureRead(source, &bytes);
        CHECK(bytes == strlen("keep my work") && memcmp(contents, "keep my work", bytes) == 0);
        free(contents);
    }
    else
    {
        CHECK(strcmp(mode, "notes") == 0 || strcmp(mode, "assembly") == 0 ||
              strcmp(mode, "framework") == 0);
        CHECK(UmiEducationProjectWorkflowDescribe(mode, destination, workflow) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(destination)); /* Describing must not reserve a path. */
        CHECK(workflow->project.trusted == 0 && workflow->build.build_testing);
        CHECK(strcmp(workflow->project.entry_point, "main.c") == 0);
        CHECK(strcmp(workflow->build.generator, "Ninja") == 0 &&
              strcmp(workflow->build.configuration, "Debug") == 0);
        CHECK(strcmp(workflow->build.build_directory, "build/default") == 0);
        CHECK(workflow->requires_framework_sdk == (strcmp(mode, "framework") == 0));
        CHECK(UmiEducationProjectExportForDevelopment(mode, destination, &written, workflow) ==
              UMI_STATUS_OK);
        CHECK(written == UmiEducationProjectFileCount(mode) && written > 0U);
        CHECK(UmiDeveloperProjectEntryPath(&workflow->project, source, sizeof source) ==
              UMI_STATUS_OK);
        CHECK(umi_fs_is_file(source));
        const char *program = strcmp(mode, "notes") == 0      ? "umicom-notes"
                              : strcmp(mode, "assembly") == 0 ? "umicom-stock-total"
                                                              : "umicom-framework-notes";
        CHECK(strstr(workflow->build.run_program, program) != NULL);
        /* Every exported byte is still the canonical lesson; IDE metadata does
         * not inject account paths, hidden defaults or a second source copy. */
        for (size_t index = 0U; index < written; ++index)
        {
            UmiEducationProjectFile file;
            CHECK(UmiEducationProjectFileAt(mode, index, &file) == UMI_STATUS_OK);
            FixturePath(source, destination, file.name);
            size_t bytes = 0U;
            unsigned char *contents = FixtureRead(source, &bytes);
            CHECK(bytes == file.size && memcmp(contents, file.bytes, bytes) == 0);
            free(contents);
        }
    }
    free(workflow);
    free(sentinel);
    return 0;
}
#include "../native_process/utf8_entry.inc"
