/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_repository_submodules.c
 *
 * PURPOSE:
 *   Present recursive child commit observations through the shared read-only repository probe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/repository/submodule_probe.h"
#include <stdio.h>
#include <string.h>

/* Human-readable Git output is untrusted display text. Keep line boundaries,
 * but make terminal controls and non-ASCII bytes explicit rather than executing
 * escape sequences or depending on the terminal's encoding. No text is reparsed
 * into a command or filesystem path. */
static int SubmoduleDisplay(FILE *stream, const unsigned char *bytes, size_t length)
{
    for (size_t i = 0U; i < length; ++i) {
        unsigned char byte = bytes[i];
        if (byte == '\n' || (byte >= 0x20U && byte <= 0x7eU)) {
            if (fputc((int)byte, stream) == EOF) return 0;
        } else if (fprintf(stream, "\\x%02x", (unsigned int)byte) < 0) return 0;
    }
    return 1;
}

int umi_cli_command_repository_submodules(int argc, char **argv)
{
    if (argc < 0 || (argc != 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (!strcmp(argv[0], "--help") || !strcmp(argv[0], "-h"))) {
        puts("Usage: umicom repo submodules [PATH]\n"
             "Observe recursive child commits and index mismatches with a 10-second timeout.\n"
             "No fetch, initialization, checkout, staging, lock or publication occurs.\n"
             "This is not a complete child working-file or remote freshness audit.");
        return 0;
    }
    if (argc > 1 || (argc == 1 && argv[0][0] == '-')) {
        fputs("Invalid submodule observation arguments. Use umicom repo submodules --help.\n", stderr);
        return 2;
    }
    UmiRepositoryInspectionContext request;
    umi_repository_inspection_context_init(&request, argc == 1 ? argv[0] : ".");
    UmiRepositoryGitCaptureResult captured = {0};
    UmiRepositorySubmoduleStatus summary;
    UmiStatus status = UmiRepositorySubmoduleCapture(&request, NULL, &captured);
    if (status == UMI_STATUS_OK)
        status = UmiRepositorySubmoduleStatusParse(captured.bytes, captured.length, &summary);
    int displayed = 1;
    if (status == UMI_STATUS_OK) {
        displayed = SubmoduleDisplay(stdout, captured.bytes, captured.length);
        if (displayed && printf("Reported: %zu; initialized: %zu; missing: %zu; index mismatch: %zu; merge-conflicted: %zu\n",
            summary.configured, summary.initialised, summary.missing,
            summary.head_mismatch, summary.dirty) < 0) displayed = 0;
        if (displayed && puts("Local observation only; child working files and remote freshness require separate review.") == EOF)
            displayed = 0;
    } else {
        fprintf(stderr, "Submodule observation failed: %s (exit %d, launched %d, timeout %d, cancelled %d, truncated %d).\n",
            umi_status_text(status), captured.exit_code, captured.launched,
            captured.timed_out, captured.cancelled, captured.output_truncated);
        if (captured.length != 0U) {
            fputs("Partial or rejected Git output follows; no summary was published:\n", stderr);
            displayed = SubmoduleDisplay(stderr, captured.bytes, captured.length);
        }
    }
    UmiRepositoryGitCaptureRelease(&captured);
    if (status != UMI_STATUS_OK || !displayed) return 1;
    return fflush(stdout) == 0 ? 0 : 1;
}
