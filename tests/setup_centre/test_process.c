/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/setup_centre/test_process.c
 * PURPOSE:
 *   Exercise the actual canonical process runner through the Setup adapter. The child is an
 *   inert C executable, not QEMU, GRUB or a guest boot.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  * Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
  * Exercise the actual canonical process runner through the Setup adapter.
  * The child is an inert C executable, not QEMU, GRUB or a guest boot.
  *---------------------------------------------------------------------------*/
#include "umicom/setup_centre/execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *test = argv[1], *child = argv[2];
    UmiProcessResult *result = calloc(1U, sizeof *result);
    CHECK(result != NULL);
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    UmiSetupProcessContext context = {token, 2000U, result};
    int exitCode = -1;
    UmiStatus status;
    if (strcmp(test, "arguments") == 0) {
/* Cover non-ANSI text, empty arguments and a trailing backslash through the real process boundary. The previous implementation remains for engineering review. */
#if 0
        const char *args[] = {"with spaces", "$(not-a-command)", "quote\"here", "caf\xc3\xa9"};
#endif
        const char *args[] = {"with spaces", "$(not-a-command)", "quote\"here", "caf\xc3\xa9", "\xe6\x9d\xb1\xe4\xba\xac", "", "trailing\\"};
/* Pass the complete regression vector without a stale hard-coded count. The previous implementation remains for engineering review. */
#if 0
        status = UmiSetupExecuteProcess(child, args, 4U, NULL, &context, &exitCode);
#endif
        status = UmiSetupExecuteProcess(child, args, sizeof(args)/sizeof(args[0]), NULL, &context, &exitCode);
        CHECK(status == UMI_STATUS_OK && exitCode == 0 && result->launched);
        CHECK(strstr(result->output, "[with spaces]") != NULL);
        CHECK(strstr(result->output, "[$(not-a-command)]") != NULL);
        CHECK(strstr(result->output, "[quote\"here]") != NULL);
        CHECK(strstr(result->output, "[caf\xc3\xa9]") != NULL);
        CHECK(strstr(result->output, "[\xe6\x9d\xb1\xe4\xba\xac]") != NULL);
        CHECK(strstr(result->output, "[]") != NULL);
        CHECK(strstr(result->output, "[trailing\\]") != NULL);
    } else if (strcmp(test, "timeout") == 0) {
        const char *args[] = {"sleep"};
        context.timeoutMs = 100U;
        status = UmiSetupExecuteProcess(child, args, 1U, NULL, &context, &exitCode);
        CHECK(status == UMI_STATUS_TIMEOUT && result->timed_out && result->launched);
        CHECK(result->termination_requested);
    } else if (strcmp(test, "pre_cancelled") == 0) {
        umi_cancellation_token_request(token);
        status = UmiSetupExecuteProcess(child, NULL, 0U, NULL, &context, &exitCode);
        CHECK(status == UMI_STATUS_CANCELLED && result->cancelled && !result->launched);
    } else if (strcmp(test, "exit_failure") == 0) {
        const char *args[] = {"fail"};
        status = UmiSetupExecuteProcess(child, args, 1U, NULL, &context, &exitCode);
        CHECK(status != UMI_STATUS_OK && exitCode == 13 && result->launched);
        CHECK(strstr(result->output, "deliberate failure") != NULL);
    } else if (strcmp(test, "missing_program") == 0) {
        char path[4096];
        int n = snprintf(path, sizeof path, "%s-not-existing", child);
        CHECK(n > 0 && (size_t)n < sizeof path);
        status = UmiSetupExecuteProcess(path, NULL, 0U, NULL, &context, &exitCode);
        CHECK(status != UMI_STATUS_OK && !result->launched);
    } else if (strcmp(test, "bad_directory") == 0) {
        status = UmiSetupExecuteProcess(child, NULL, 0U, child, &context, &exitCode);
        CHECK(status != UMI_STATUS_OK && !result->launched);
    } else {
        free(result);
        umi_cancellation_token_destroy(token);
        return 2;
    }
    umi_cancellation_token_destroy(token);
    free(result);
    printf("PASS native process adapter: %s\n", test);
    return 0;
}
