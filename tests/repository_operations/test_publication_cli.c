/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_publication_cli.c
 *
 * PURPOSE:
 *   Verify native publication option admission and shared-service forwarding without Git.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../tools/umicom/src/cli.h"
#include "umicom/repository/publication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned preparations, previews, applies;
static int forwarding_valid = 1;
static const char *scenario;

UmiStatus umi_cli_context_prepare_operation(UmiCliContext *context, const char *root,
    UmiToolchainOperationKind operation)
{
    (void)context;
    ++preparations;
    if (strcmp(root, "/work") != 0 ||
        (operation != UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ &&
         operation != UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE)) forwarding_valid = 0;
    return strcmp(scenario, "prepare-failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static UmiStatus FakeRun(const UmiToolchainProfile *, UmiEnvironmentPlan *,
    const UmiRepositoryPublicationRequest *, const UmiRepositoryPublicationOptions *,
    size_t, UmiRepositoryPublicationReport *, size_t);

/* Substitute the service boundary only. The real parser, preview-before-apply
 * sequence and presentation remain in use, with no tool process or remote. */
#define umi_repository_publication_run_options_sized FakeRun
#include "../../tools/umicom/src/command_repository_publication.c"
#undef umi_repository_publication_run_options_sized

static UmiStatus FakeRun(const UmiToolchainProfile *profile, UmiEnvironmentPlan *environment,
    const UmiRepositoryPublicationRequest *request, const UmiRepositoryPublicationOptions *options,
    size_t options_size, UmiRepositoryPublicationReport *report, size_t report_size)
{
    (void)profile; (void)environment;
    if (options_size != sizeof(*options) || report_size != sizeof(*report) ||
        strcmp(request->workspace_root, "/work") != 0 || !options->primary_directory ||
        strcmp(options->primary_directory, "Applications") != 0 || options->attach_published_main)
        forwarding_valid = 0;
    int wants_review = strcmp(scenario, "apply") == 0 || strcmp(scenario, "blocked-preview") == 0;
    if ((options->review_staged != NULL) != wants_review) forwarding_valid = 0;
    if (request->apply) {
        ++applies;
        if (previews != 1U || !request->refresh || !request->observer ||
            !request->commit_message || strcmp(request->commit_message, "fix: reviewed change") != 0)
            forwarding_valid = 0;
    } else ++previews;
    if (strcmp(scenario, "watch") == 0) {
        if (request->apply || request->refresh) forwarding_valid = 0;
        publication_stop = 1; /* End the real foreground loop after one snapshot. */
    }
    memset(report, 0, sizeof(*report));
    report->status = strcmp(scenario, "blocked-preview") == 0
        ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    (void)snprintf(report->detail, sizeof(report->detail), "Synthetic publication service result.");
    return report->status;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    scenario = argv[1];
    UmiCliContext *context = calloc(1U, sizeof(*context));
    if (!context) return 1;
    char *args[12] = {"publish-all", "/work", "--primary-directory", "Applications"};
    char **vector = args;
    int count = 4, expected = 2;
    unsigned expected_preparations = 0U, expected_previews = 0U, expected_applies = 0U;
    if (strcmp(scenario, "preview") == 0 || strcmp(scenario, "watch") == 0) {
        if (strcmp(scenario, "watch") == 0) args[0] = "watch";
        expected = 0; expected_preparations = expected_previews = 1U;
    } else if (strcmp(scenario, "apply") == 0 || strcmp(scenario, "blocked-preview") == 0) {
        args[4] = "--review-staged"; args[5] = "--apply";
        args[6] = "--message"; args[7] = "fix: reviewed change"; count = 8;
        expected_preparations = expected_previews = 1U;
        expected_applies = strcmp(scenario, "apply") == 0 ? 1U : 0U;
        expected = expected_applies ? 0 : 1;
    } else if (strcmp(scenario, "prepare-failure") == 0) {
        expected_preparations = 1U; expected = 1;
    } else if (strcmp(scenario, "null-vector") == 0) vector = NULL;
    else if (strcmp(scenario, "null-value") == 0) args[2] = NULL;
    else if (strcmp(scenario, "empty-value") == 0) args[3] = "";
    else if (strcmp(scenario, "negative-count") == 0) count = -1;
    else if (strcmp(scenario, "null-context") == 0) { free(context); context = NULL; }
    else if (strcmp(scenario, "unknown-command") == 0) args[0] = "unexpected";
    else if (strcmp(scenario, "empty-workspace") == 0) args[1] = "";
    else if (strcmp(scenario, "missing-primary") == 0) count = 3;
    else if (strcmp(scenario, "duplicate-primary") == 0) {
        args[4] = "--primary-directory"; args[5] = "Other"; count = 6;
    } else if (strcmp(scenario, "primary-followed-by-option") == 0) args[3] = "--apply";
    else if (strcmp(scenario, "missing-message") == 0) {
        args[4] = "--message"; args[5] = "--apply"; count = 6;
    } else if (strcmp(scenario, "missing-report") == 0) {
        args[4] = "--report"; args[5] = "--apply"; count = 6;
    } else if (strcmp(scenario, "duplicate-review") == 0) {
        args[4] = args[5] = "--review-staged"; count = 6;
    } else if (strcmp(scenario, "watch-apply") == 0) {
        args[0] = "watch"; args[4] = "--apply"; count = 5;
    } else if (strcmp(scenario, "watch-review") == 0) {
        args[0] = "watch"; args[4] = "--review-staged"; count = 5;
    } else if (strcmp(scenario, "duplicate-interval") == 0) {
        args[0] = "watch"; args[4] = args[6] = "--interval";
        args[5] = "1"; args[7] = "2"; count = 8;
    } else { free(context); return 2; }
    int actual = umi_cli_command_repository_publication(context, count, vector);
    int failed = actual != expected || preparations != expected_preparations ||
        previews != expected_previews || applies != expected_applies || !forwarding_valid;
    if (failed) (void)fprintf(stderr, "%s: result=%d prepares=%u previews=%u applies=%u forwarding=%d\n",
        scenario, actual, preparations, previews, applies, forwarding_valid);
    free(context);
    return failed;
}
