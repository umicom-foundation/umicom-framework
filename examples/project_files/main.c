/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/project_files/main.c
 *
 * PURPOSE:
 *   Create the Notes project structure through Framework workspace authority,
 *   then read its index without overwriting an existing developer project.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/workspace_files.h"
#include "umicom/platform/file_index.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

/* Return a useful failure message while keeping any created files available. */
static int Report(UmiStatus status, const char *operation)
{
    if (status == UMI_STATUS_OK) return 0;
    (void)fprintf(stderr, "%s: %s\n", operation, umi_status_text(status));
    return 1;
}

int main(int argc, char **argv)
{
    UmiWorkspaceGraph *workspace = NULL;
    UmiFileIndex *index = NULL;
    UmiWorkspaceGraphSnapshot snapshot;
    UmiWorkspaceEntryResult created;
    UmiFileIndexEntry rows[2];
    UmiFileIndexPage page;
    char cwd[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY];
    int failed = 1;
    const int selfTest = argc == 2 && strcmp(argv[1], "--self-test") == 0;
    if (argc != 1 && !selfTest) {
        (void)fputs("Usage: umicom-notes-project-files [--self-test]\n", stderr);
        return 2;
    }
    if (Report(umi_fs_current_directory(cwd, sizeof(cwd)), "Read current folder") ||
        Report(umi_path_join(cwd, "umicom-notes-project-files", root, sizeof(root)), "Resolve practice folder")) return 1;
    if (umi_fs_exists(root)) {
        (void)fputs("The practice folder already exists. Run from a different empty directory.\n", stderr);
        return 1;
    }
    /* The example owns only this newly created practice folder. Trust here is
     * explicit because this program, not an imported project, creates it. */
    if (Report(umi_workspace_graph_create(&workspace), "Create workspace") ||
        Report(umi_workspace_graph_open(workspace, cwd, 1), "Select the current folder") ||
        Report(UmiWorkspaceCreateEntry(workspace, 0U, "umicom-notes-project-files",
            UMI_WORKSPACE_ENTRY_DIRECTORY, &created), "Create practice folder") ||
        Report(umi_workspace_graph_open(workspace, root, 1), "Open practice workspace") ||
        Report(umi_workspace_graph_snapshot(workspace, &snapshot), "Read workspace identity")) goto cleanup;
    if (Report(UmiWorkspaceCreateEntry(workspace, snapshot.revision, "include",
        UMI_WORKSPACE_ENTRY_DIRECTORY, &created), "Create include folder")) goto cleanup;
    if (Report(UmiWorkspaceCreateEntry(workspace, snapshot.revision, "include/notes.h",
        UMI_WORKSPACE_ENTRY_FILE, &created), "Create Notes header")) goto cleanup;
    (void)printf("Created an empty header: %s\n", created.path);
    /* Repeating creation is an error, never an instruction to empty the file. */
    if (UmiWorkspaceCreateEntry(workspace, snapshot.revision, "include/notes.h",
        UMI_WORKSPACE_ENTRY_FILE, &created) != UMI_STATUS_ALREADY_EXISTS || created.created) goto cleanup;
    (void)puts("The second creation was rejected; the existing header was preserved.");
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    config.maximum_files = 32U;
    if (Report(umi_file_index_create(&config, &index), "Create file index") ||
        Report(umi_file_index_rebuild(index), "Read project files") ||
        Report(UmiFileIndexReadPage(index, ".h", 0, 0U, 0U, rows, 2U, &page), "Read header page")) goto cleanup;
    for (size_t n = 0U; n < page.count; ++n)
        (void)printf("Indexed: %s (%llu bytes)\n", rows[n].relative_path,
            (unsigned long long)rows[n].size);
    if (page.count != 1U || strcmp(rows[0].relative_path, "include/notes.h") != 0) goto cleanup;
    failed = 0;
cleanup:
    umi_file_index_destroy(index);
    umi_workspace_graph_destroy(workspace);
    /* Normal use retains the practice project. CTest cleans only the dedicated
     * folder just created by this invocation, after all checks have succeeded. */
    if (!failed && selfTest) failed = Report(umi_fs_remove_tree(root), "Remove test fixture");
    else (void)printf("Practice files retained at: %s\n", root);
    return failed;
}
