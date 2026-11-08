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


/* Only presentation belongs in the CLI. Framework captures the tree, invokes
 * this callback and refuses changes made while the owner reviews it. Escape
 * bytes that could move the terminal cursor or impersonate the next prompt. */
static void review_text(const char *bytes, size_t size, int multiline)
{
    for (size_t i = 0U; i < size; ++i) {
        unsigned char value = (unsigned char)bytes[i];
        if (multiline && value == '\n') (void)putchar('\n');
        else if (value < 32U || value >= 127U || value == '\\')
            (void)printf("\\x%02x", (unsigned)value);
        else (void)putchar((int)value);
    }
}

static UmiStatus review_staged(const UmiRepositoryPublicationStagedReview *review,
    void *context)
{
    char answer[32];
    (void)context;
    (void)fputs("\nStaged review: ", stdout);
    review_text(review->repository_path, strlen(review->repository_path), 0);
    (void)printf("\nHEAD: %s\nStaged tree: %s\n", review->head, review->tree);
    (void)fputs("\nSummary\n", stdout);
    review_text(review->summary, review->summary_size, 1);
    (void)fputs("\nPatch (control and non-ASCII bytes use hexadecimal notation)\n", stdout);
    review_text(review->patch, review->patch_size, 1);
    (void)fputs("\nType commit and press Enter to approve this new commit. Anything else stops publication: ", stdout);
    if (fflush(stdout) != 0 || ferror(stdout)) return UMI_STATUS_IO_ERROR;
    if (!fgets(answer, sizeof(answer), stdin))
        return ferror(stdin) ? UMI_STATUS_IO_ERROR : UMI_STATUS_CANCELLED;
    return strcmp(answer, "commit\n") == 0 || strcmp(answer, "commit\r\n") == 0
        ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
}

/* Preserve default API dispatch for existing clients. Explicit exclusions are
 * the same borrowed scope in preview, monitoring and fresh apply preflight. */
static UmiStatus run_publication(UmiCliContext *context,
    const UmiRepositoryPublicationRequest *request,
    const UmiRepositoryPublicationOptions *options,
    const char *const *excluded, size_t count, UmiRepositoryPublicationReport *report)
{
    return count == 0U
        ? umi_repository_publication_run_options(&context->discovery.profile,
            &context->environment, request, options, report)
        : umi_repository_publication_run_scoped(&context->discovery.profile,
            &context->environment, request, options, excluded, count, report);
}

int umi_cli_command_repository_publication(
    UmiCliContext *context, int argc, char **argv)
{
    UmiRepositoryPublicationRequest request;
    UmiRepositoryPublicationOptions options;
    UmiRepositoryPublicationReport *report = NULL, *previous = NULL;
    unsigned interval = 30U;
    const char *report_path = NULL;
    UmiOutputFile *report_file = NULL;
    UmiStatus report_status = UMI_STATUS_OK;
    int apply = 0;
    int staged_review = 0;
    int attach_published_main = 0;
    int watch;
    UmiStatus status;
    void (*old_signal)(int) = SIG_DFL;
    int interval_seen = 0;
    const char *excluded[UMI_REPOSITORY_PUBLICATION_EXCLUSION_CAPACITY];
    size_t excluded_count = 0U;
    /* Reject malformed vectors before dereferencing or discovering any tool. */
    if (!argv || argc < 2) return 2;
    for (int i = 0; i < argc; ++i) if (!argv[i] || !argv[i][0]) return 2;
    if (strcmp(argv[0], "publish-all") != 0 && strcmp(argv[0], "watch") != 0) return 2;
    if (!context || argc < 2 || argv[1][0] == '-') {
        (void)fputs("Usage: umicom repo publish-all WORKSPACE [--apply] [--attach-published-main] [--message TEXT]\n"
            "       Add --primary-directory PATH to select the main editing subtree.\n"
            "       Add --review-staged to inspect each new commit before approving it.\n"
            "       Add --exclude NAME to leave an unrelated immediate checkout outside scope.\n"
            "       Add --report ABSOLUTE_PATH to save a new private JSON result.\n"
            "       umicom repo watch WORKSPACE [--interval SECONDS]\n", stderr);
        return 2;
    }
    watch = strcmp(argv[0], "watch") == 0;
    umi_repository_publication_request_init(&request, argv[1]);
    umi_repository_publication_options_init(&options);
    for (int i = 2; i < argc; ++i) {
        /* Explicit checkout scope is admitted before the existing publication
         * switches. Keep the former first-branch condition for review; --apply
         * still uses the same duplicate guard and execution path below. */
#if 0
        if (!watch && strcmp(argv[i], "--apply") == 0) {
#endif
        if (strcmp(argv[i], "--exclude") == 0) {
            if (i + 1 >= argc || excluded_count == UMI_REPOSITORY_PUBLICATION_EXCLUSION_CAPACITY) return 2;
            excluded[excluded_count++] = argv[++i];
        } else if (!watch && strcmp(argv[i], "--apply") == 0) {
            if (apply) return 2;
            apply = 1;
        } else if (!watch && strcmp(argv[i], "--review-staged") == 0) {
            if (staged_review) return 2;
            staged_review = 1;
        } else if (!watch && strcmp(argv[i], "--attach-published-main") == 0) {
            if (attach_published_main) return 2;
            attach_published_main = 1;
        } else if (strcmp(argv[i], "--primary-directory") == 0 &&
            i + 1 < argc && options.primary_directory == NULL &&
            argv[i + 1][0] && argv[i + 1][0] != '-') {
            options.primary_directory = argv[++i];
        /* A following option is not a report filename. Preserve the earlier
         * two-line branch condition while validating before tool discovery. */
#if 0
        } else if (!watch && strcmp(argv[i], "--report") == 0 &&
            i + 1 < argc && report_path == NULL) {
#endif
        } else if (!watch && strcmp(argv[i], "--report") == 0 &&
            i + 1 < argc && report_path == NULL && argv[i + 1][0] != '-') {
            report_path = argv[++i];
        /* Do not reinterpret a missing message as the next option. The former
         * condition is retained; valid message forwarding is unchanged. */
#if 0
        } else if (!watch && strcmp(argv[i], "--message") == 0 &&
            i + 1 < argc && request.commit_message == NULL) {
#endif
        } else if (!watch && strcmp(argv[i], "--message") == 0 &&
            i + 1 < argc && request.commit_message == NULL && argv[i + 1][0] != '-') {
            request.commit_message = argv[++i];
        /* Repeated intervals are ambiguous. Keep the prior condition for
         * review while requiring a single explicit value. */
#if 0
        } else if (watch && strcmp(argv[i], "--interval") == 0 && i + 1 < argc) {
#endif
        } else if (watch && strcmp(argv[i], "--interval") == 0 && i + 1 < argc && !interval_seen) {
            interval_seen = 1;
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
    if (umi_repository_publication_exclusions_validate(excluded, excluded_count) != UMI_STATUS_OK) return 2;
    for (size_t i = 0U; i < excluded_count; ++i)
        (void)printf("Explicitly excluded top-level checkout: %s\n", excluded[i]);
    options.attach_published_main = attach_published_main;
    options.review_staged = staged_review ? review_staged : NULL;
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
        /* Preserve recovery-only dispatch; the options service adds the same
         * explicit checkout policy to preview and apply. */
#if 0
        status = umi_repository_publication_run_recovery(&context->discovery.profile,
            &context->environment, &request, report, attach_published_main);
#endif
        /* The default-only call is retained; explicit scope now reaches both preview and apply. */
#if 0
        status = umi_repository_publication_run_options(&context->discovery.profile,
            &context->environment, &request, &options, report);
#endif
        status = run_publication(context, &request, &options, excluded, excluded_count, report);
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
            /* The same policy must govern the fresh apply preflight. Retain
             * the earlier recovery-only call for compatibility review. */
#if 0
            status = umi_repository_publication_run_recovery(&context->discovery.profile,
                &context->environment, &request, report, attach_published_main);
#endif
            /* Retain the default-only apply call for review. */
#if 0
            status = umi_repository_publication_run_options(&context->discovery.profile,
                &context->environment, &request, &options, report);
#endif
            status = run_publication(context, &request, &options, excluded, excluded_count, report);
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
