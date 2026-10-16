/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository_publication.c
 *
 * PURPOSE:
 *   Expose workspace publication and local change monitoring through the native Umicom command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/repository/publication.h"
#include "umicom/repository/publication_report.h"
#include "umicom/platform/threading.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A signal handler only updates a signal-safe scalar. The current bounded
 * Git probe finishes before the read-only watch loop exits. */
static volatile sig_atomic_t publication_stop = 0;

static void stop_watching(int signal_number)
{
    (void)signal_number;
    publication_stop = 1;
}

static void show_report(const UmiRepositoryPublicationReport *report)
{
    (void)printf("\n%zu checkouts; %zu publishing repositories; %zu blocked.\n",
        report->count, report->repository_count, report->blocked_count);
    for (size_t i = 0U; i < report->count; ++i) {
        const UmiRepositoryPublicationEntry *entry = &report->entries[i];
        (void)printf("%s | %zu changed paths | %zu ahead / %zu behind | %s\n",
            entry->blocked ? "BLOCKED" : entry->vendor ? "Vendor pin" :
            entry->primary == i ? "Primary" : "Shared copy",
            entry->changed_paths, entry->ahead, entry->behind, entry->path);
        (void)printf("  Branch: %s | HEAD: %.12s | origin/main: %.12s\n",
            entry->branch[0] ? entry->branch : "detached", entry->head, entry->remote_head);
        if (entry->detail[0]) (void)printf("  %s\n", entry->detail);
        /* Completed flags survive failed validation and graph rediscovery. */
        if (entry->committed || entry->pushed || entry->fast_forwarded || entry->locked)
            (void)printf("  Completed:%s%s%s%s\n",
                entry->committed ? " commit" : "",
                entry->pushed ? " push" : "",
                entry->fast_forwarded ? " fast-forward" : "",
                entry->locked ? " parent lock" : "");
    }
    (void)printf("%s\n", report->detail);
    (void)printf("%zu commits; %zu pushes; %zu checkout updates completed.\n",
        report->committed_count, report->pushed_count, report->updated_count);
}

static void show_action(const char *path, const char *action, void *context)
{
    (void)context;
    (void)printf("%s: %s\n", action, path);
    (void)fflush(stdout);
}

int umi_cli_command_repository_publication(
    UmiCliContext *context, int argc, char **argv)
{
    UmiRepositoryPublicationRequest request;
    UmiRepositoryPublicationReport *report = NULL, *previous = NULL;
    unsigned interval = 30U;
    const char *report_path = NULL;
    UmiOutputFile *report_file = NULL;
    UmiStatus report_status = UMI_STATUS_OK;
    int apply = 0;
    int attach_published_main = 0;
    int watch;
    UmiStatus status;
    void (*old_signal)(int) = SIG_DFL;
    if (!context || argc < 2 || argv[1][0] == '-') {
        (void)fputs("Usage: umicom repo publish-all WORKSPACE [--apply] [--attach-published-main] [--message TEXT]\n"
            "       Add --report ABSOLUTE_PATH to save a new private JSON result.\n"
            "       umicom repo watch WORKSPACE [--interval SECONDS]\n", stderr);
        return 2;
    }
    watch = strcmp(argv[0], "watch") == 0;
    umi_repository_publication_request_init(&request, argv[1]);
    for (int i = 2; i < argc; ++i) {
        if (!watch && strcmp(argv[i], "--apply") == 0) {
            if (apply) return 2;
            apply = 1;
        } else if (!watch && strcmp(argv[i], "--attach-published-main") == 0) {
            if (attach_published_main) return 2;
            attach_published_main = 1;
        } else if (!watch && strcmp(argv[i], "--report") == 0 &&
            i + 1 < argc && report_path == NULL) {
            report_path = argv[++i];
        } else if (!watch && strcmp(argv[i], "--message") == 0 &&
            i + 1 < argc && request.commit_message == NULL) {
            request.commit_message = argv[++i];
        } else if (watch && strcmp(argv[i], "--interval") == 0 && i + 1 < argc) {
            char *end = NULL;
            errno = 0;
            unsigned long parsed = strtoul(argv[++i], &end, 10);
            if (errno || !end || *end || parsed < 1UL || parsed > 3600UL) {
                (void)fputs("Watch interval must be between 1 and 3600 seconds.\n", stderr);
                return 2;
            }
            interval = (unsigned)parsed;
        } else {
            (void)fprintf(stderr, "Unknown or incomplete publication option: %s\n", argv[i]);
            return 2;
        }
    }
    status = umi_cli_context_prepare_operation(context, request.workspace_root,
        apply ? UMI_TOOLCHAIN_OPERATION_REPOSITORY_WRITE : UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot prepare Git: %s\n", umi_status_text(status));
        return 1;
    }
    report = calloc(1U, sizeof(*report));
    if (watch) previous = calloc(1U, sizeof(*previous));
    if (!report || (watch && !previous)) {
        free(report); free(previous);
        return 1;
    }
    if (watch) {
        request.refresh = 0;
        publication_stop = 0;
        old_signal = signal(SIGINT, stop_watching);
        (void)puts("Watching local changes using cached remote refs. No add, commit, push or fetch.");
    }
    int first = 1;
    do {
/* Explicit recovery extends the existing publication request without changing its defaults. The former implementation is retained for review. */
#if 0
        status = umi_repository_publication_run(&context->discovery.profile,
            &context->environment, &request, report);
#endif
        status = umi_repository_publication_run_recovery(&context->discovery.profile,
            &context->environment, &request, report, attach_published_main);
        if (!watch || first || memcmp(previous, report, sizeof(*report)) != 0) {
            show_report(report);
            (void)fflush(stdout);
            if (previous) memcpy(previous, report, sizeof(*report));
        }
        first = 0;
        if (!watch) break;
        for (unsigned elapsed = 0U; elapsed < interval * 10U && !publication_stop; ++elapsed)
            umi_thread_sleep_ms(100U);
    } while (!publication_stop);
    if (watch && old_signal != SIG_ERR) (void)signal(SIGINT, old_signal);
    /* Reserve the explicitly chosen new evidence file after preview and before
     * any publication. Placement or open failures must not silently run apply.
     * A blocked preview can still be saved for review without changing Git. */
    if (report_path) {
        report_status = umi_repository_publication_report_check_path(report, report_path);
        if (report_status == UMI_STATUS_OK)
            report_status = UmiOutputFileCreate(report_path, &report_file);
        if (report_status != UMI_STATUS_OK)
            (void)fprintf(stderr, "Cannot create publication report: %s. Choose a new absolute file outside Git repositories in an existing trusted directory. No apply was started.\n",
                umi_status_text(report_status));
    }
    if (report_status == UMI_STATUS_OK) {
        if (!watch && apply && status == UMI_STATUS_OK) {
            /* --apply is the owner's explicit publication instruction. Display a
             * preview and then take a fresh preflight; no interactive loop or shell
             * script is needed to publish a whole workspace. */
            (void)puts("Applying with a fresh preflight. Keep repository editors idle until completion.");
            request.apply = 1;
            request.observer = show_action;
    /* Explicit recovery extends the existing publication request without changing its defaults. The former implementation is retained for review. */
#if 0
            status = umi_repository_publication_run(&context->discovery.profile,
                &context->environment, &request, report);
#endif
            status = umi_repository_publication_run_recovery(&context->discovery.profile,
                &context->environment, &request, report, attach_published_main);
            show_report(report);
        } else if (!watch && status == UMI_STATUS_OK) {
            (void)puts("Preview only. Add --apply to stage, commit, push and lock parents.");
        }
    }
    if (report_file) {
        /* Publication failure and evidence I/O failure remain separate. A late
         * write error cannot undo commits or pushes that already succeeded. */
        report_status = umi_repository_publication_report_write_json(report,
            request.apply ? UMI_REPOSITORY_PUBLICATION_REPORT_APPLY :
                UMI_REPOSITORY_PUBLICATION_REPORT_PREVIEW, report_file);
        UmiStatus close_status = UmiOutputFileClose(report_file);
        if (report_status == UMI_STATUS_OK) report_status = close_status;
        UmiOutputFileDestroy(report_file);
        if (report_status == UMI_STATUS_OK)
            (void)printf("Publication report: %s\n", report_path);
        else
            (void)fprintf(stderr, "Publication report is incomplete: %s (%s). Completed Git actions are retained; inspect terminal output before retrying.\n",
                report_path, umi_status_text(report_status));
    }
    free(previous);
    free(report);
    /* A report failure now contributes to command failure without changing the
     * Git result. Retain the former return rule for compatibility review. */
#if 0
    return watch || status == UMI_STATUS_OK ? 0 : 1;
#endif
    return (watch || status == UMI_STATUS_OK) && report_status == UMI_STATUS_OK ? 0 : 1;
}
