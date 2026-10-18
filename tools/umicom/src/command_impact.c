/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_impact.c
 * Purpose: Present affected build and test plans through existing Framework services.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/distribution/runtime/impact.h"
#include "umicom/platform/input_file.h"
#include "umicom/vcs/working_tree.h"
#include "umicom/vcs/revision_changes.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Print data, not executable command text. Repository paths and test names may
 * contain terminal controls, so escape them before showing a proposed plan. */
static void Text(const char *text)
{
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < 32U || *p == 127U || *p == '\\') (void)printf("\\x%02x", (unsigned)*p);
        else (void)putchar((int)*p);
    }
}
static void Reasons(uint32_t value)
{
    if (value & UMI_RELEASE_IMPACT_CHANGED_SOURCE) (void)fputs(" changed-source", stdout);
    if (value & UMI_RELEASE_IMPACT_DEPENDENT) (void)fputs(" dependent", stdout);
    if (value & UMI_RELEASE_IMPACT_TEST_OWNER) (void)fputs(" test-owner", stdout);
    if (value & UMI_RELEASE_IMPACT_TEST_PREREQUISITE) (void)fputs(" test-prerequisite", stdout);
    if (value & UMI_RELEASE_IMPACT_ALWAYS) (void)fputs(" always", stdout);
    if (value & UMI_RELEASE_IMPACT_FULL) (void)fputs(" full-selection", stdout);
}
static void Fallback(uint32_t value)
{
    if (value & UMI_RELEASE_IMPACT_REQUESTED_FULL) (void)puts("Full selection requested.");
    if (value & UMI_RELEASE_IMPACT_UNMAPPED_PATH) (void)puts("Full selection: changed path has no configured owner.");
    if (value & UMI_RELEASE_IMPACT_BUILD_CONFIGURATION) (void)puts("Full selection: build configuration changed; reconfigure first.");
    if (value & UMI_RELEASE_IMPACT_INCOMPLETE_GRAPH) (void)puts("Full selection: dependency graph is incomplete or predates impact metadata.");
    if (value & UMI_RELEASE_IMPACT_UNOWNED_TEST) (void)puts("Full selection: at least one test lacks complete build prerequisites.");
    if (value & UMI_RELEASE_IMPACT_UNRESOLVED_SOURCE) (void)puts("Full selection: at least one source/header identity is unresolved.");
}
int umi_cli_command_impact(int argc, char **argv)
{
    if (argc == 1 && argv != NULL && argv[0] != NULL && strcmp(argv[0], "--help") == 0) {
        (void)puts("Usage: umicom impact --inventory ABSOLUTE.tsv [--working-tree | --changed PATH ...] [--since COMMIT] [--full] [--git PROGRAM] [--expression]\n"
            "Preview only. No builds, tests, staging, commits, pulls, pushes or branch changes.\n"
            "Configure with UMICOM_RELEASE_INVENTORY_CAPTURE=ON to produce inventory-CONFIG.tsv.\n"
            "--changed paths are relative to the inventory source root; include both names of a rename.\n"
            "--working-tree observes staged, unstaged and untracked Git changes in that source root.\n"
            "--since COMMIT includes committed differences plus working changes; use a complete recorded commit ID.\n"
            "Working-tree mode alone excludes committed changes; supply --since, explicit paths or --full.\n"
            "Unknown ownership or dependency metadata widens the selection. No changes is not tests passed.\n"
            "Inventory age/source bytes are not verified. Reconfigure after build-system changes.\n"
            "Exit 0: preview complete; 1: selected work has no registered test coverage; 2: invalid input or discovery failure.");
        return fflush(stdout) == 0 && ferror(stdout) == 0 ? 0 : 2;
    }
    if (argc < 0 || (argc > 0 && argv == NULL)) return 2;
    const char *inventoryPath = NULL, *git = "git", *since = NULL;
    char observedCommit[UMI_VCS_ID_CAPACITY] = {0};
    bool workingTree = false, full = false, gitSeen = false, expression = false;
    const char **paths = calloc(UMI_RELEASE_IMPACT_CHANGE_LIMIT, sizeof(*paths));
    if (paths == NULL) return 2;
    size_t count = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (int i = 0; i < argc && status == UMI_STATUS_OK; ++i) {
        const char *key = argv[i];
        if (key == NULL) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
        if (strcmp(key, "--working-tree") == 0 && !workingTree) { workingTree = true; continue; }
        if (strcmp(key, "--full") == 0 && !full) { full = true; continue; }
        if (strcmp(key, "--expression") == 0 && !expression) { expression = true; continue; }
        if (i + 1 >= argc || argv[i + 1] == NULL || argv[i + 1][0] == '\0') {
            status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
        const char *value = argv[++i];
        if (strcmp(key, "--inventory") == 0 && inventoryPath == NULL) inventoryPath = value;
        else if (strcmp(key, "--git") == 0 && !gitSeen) { git = value; gitSeen = true; }
        else if (strcmp(key, "--since") == 0 && since == NULL) since = value;
        else if (strcmp(key, "--changed") == 0 && count < UMI_RELEASE_IMPACT_CHANGE_LIMIT) paths[count++] = value;
        else status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (since != NULL) workingTree = true;
    if (inventoryPath == NULL || (workingTree && count != 0U) || (gitSeen && !workingTree) ||
        (!workingTree && count == 0U && !full)) status = UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiReleaseInventory *inventory = NULL;
    UmiVcsWorkingTree *tree = NULL;
    UmiVcsRevisionChanges *committed = NULL;
    UmiReleaseImpactPlan *plan = NULL;
    if (status == UMI_STATUS_OK) status = UmiInputFileRead(inventoryPath, UMI_RELEASE_INVENTORY_TEXT_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiReleaseInventoryParse((const char *)bytes, size, &inventory);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && workingTree) {
        UmiVcsWorkingTreeRequest request = {0};
        request.repository_root = UmiReleaseInventorySourceRoot(inventory);
        request.git_program = git;
        request.timeout_ms = 30000U;
        status = UmiVcsWorkingTreeRead(&request, &tree);
        UmiVcsWorkingTreeSummary summary = {0};
        if (status == UMI_STATUS_OK) status = UmiVcsWorkingTreeDescribe(tree, &summary);
        if (status == UMI_STATUS_OK && summary.conflicts != 0U) status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK && summary.commit_known)
            (void)snprintf(observedCommit, sizeof(observedCommit), "%s", summary.commit_id);
        if (status == UMI_STATUS_OK && since != NULL && !summary.commit_known) status = UMI_STATUS_INVALID_STATE;
        for (size_t i = 0U; i < summary.entries && status == UMI_STATUS_OK; ++i) {
            const UmiVcsWorkingTreeEntry *entry = UmiVcsWorkingTreeEntryAt(tree, i);
            if (entry == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
            if (count > UMI_RELEASE_IMPACT_CHANGE_LIMIT - 2U) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
            paths[count++] = entry->change.path;
            if (entry->change.original_path[0] != '\0') paths[count++] = entry->change.original_path;
            if (entry->submodule || entry->staged_gitlink) full = true;
        }
    }
    if (status == UMI_STATUS_OK && since != NULL) {
        UmiVcsRevisionChangesRequest request = {0};
        request.repository_root = UmiReleaseInventorySourceRoot(inventory);
        request.git_program = git;
        request.base_commit = since;
        request.head_commit = observedCommit;
        request.timeout_ms = 30000U;
        status = UmiVcsRevisionChangesRead(&request, &committed);
        for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiVcsRevisionChangesCount(committed); ++i) {
            const UmiVcsRevisionChange *entry = UmiVcsRevisionChangesAt(committed, i);
            if (count == UMI_RELEASE_IMPACT_CHANGE_LIMIT) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
            paths[count++] = entry->path;
            if (entry->gitlink) full = true;
        }
    }
    if (status == UMI_STATUS_OK) status = UmiReleaseImpactCreate(inventory, paths, count, full, &plan);
    int result = 2;
    if (status == UMI_STATUS_OK) {
        UmiReleaseImpactSummary summary = {0};
        status = UmiReleaseImpactDescribe(plan, &summary);
        if (status == UMI_STATUS_OK) {
            (void)fputs("Inventory generation: ", stdout); Text(UmiReleaseInventoryGeneration(inventory));
            (void)fputs("\nConfiguration: ", stdout); Text(UmiReleaseInventoryConfiguration(inventory));
            (void)printf("\nChanged paths: %zu; selected targets: %zu; selected tests: %zu.\n",
                summary.changedPaths, summary.targets, summary.tests);
            if (since != NULL) {
                (void)fputs("Commit baseline: ", stdout); Text(since);
                (void)fputs("; observed HEAD: ", stdout); Text(observedCommit); (void)putchar('\n');
            }
            Fallback(summary.fallbackReasons);
            for (size_t i = 0U; i < UmiReleaseImpactCount(plan); ++i) {
                const UmiReleaseImpactItem *item = UmiReleaseImpactAt(plan, i);
                (void)fputs(item->kind == UMI_RELEASE_INVENTORY_TARGET ? "target\t" : "test\t", stdout);
                Text(item->name); Reasons(item->reasons); (void)putchar('\n');
            }
            if (expression) {
                size_t required = 0U;
                UmiStatus encoded = UmiReleaseImpactTestExpression(plan, NULL, 0U, &required);
                if (encoded == UMI_STATUS_OK) {
                    char *filter = malloc(required);
                    if (filter == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
                    else {
                        encoded = UmiReleaseImpactTestExpression(plan, filter, required, &required);
                        if (encoded == UMI_STATUS_OK) {
                            (void)fputs("Exact CTest expression (data, not a shell command):\n", stdout);
                            (void)puts(filter);
                        } else status = encoded;
                        free(filter);
                    }
                } else if (encoded == UMI_STATUS_UNAVAILABLE) {
                    (void)puts("No narrow expression emitted: full/empty selection or expression limits. Do not substitute an empty filter.");
                } else status = encoded;
            }
            if (summary.noWork) (void)puts("No changed paths selected. This does not certify committed code or a previous build.");
            if (summary.missingTestCoverage) (void)puts("Selected work has no registered test coverage; qualification is incomplete.");
            (void)puts("PREVIEW ONLY. No build or test executed. Review metadata and qualify a stable source snapshot.");
            result = status != UMI_STATUS_OK ? 2 : summary.missingTestCoverage ? 1 : 0;
        }
    }
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "Impact preview refused: %s. Use impact --help.\n", umi_status_text(status));
    if (fflush(stdout) != 0 || ferror(stdout) != 0) result = 2;
    UmiReleaseImpactDestroy(plan); UmiVcsWorkingTreeDestroy(tree);
    UmiVcsRevisionChangesDestroy(committed);
    UmiReleaseInventoryDestroy(inventory); free(paths);
    return result;
}
