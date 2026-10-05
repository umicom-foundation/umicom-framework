/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/test_runner.c
 * PURPOSE: Verify real process streaming, compatibility, cancellation and final diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
typedef struct Probe { const char *name; UmiBuildResult *result; UmiCancellationToken *token; char bytes[1024]; size_t length; int acknowledged; } Probe;
static void Observe(const char *bytes, size_t length, void *context)
{
    Probe *probe = context;
    CHECK(probe->result->state == UMI_BUILD_STATE_RUNNING);
    CHECK(length <= sizeof(probe->bytes) - probe->length);
    memcpy(probe->bytes + probe->length, bytes, length); probe->length += length;
    if (strcmp(probe->name, "cancel") == 0) umi_cancellation_token_request(probe->token);
    else if (strcmp(probe->name, "timeout") != 0 && !probe->acknowledged) {
        FILE *file = fopen("stream-ack", "wb"); CHECK(file != NULL);
        CHECK(fclose(file) == 0); probe->acknowledged = 1;
    }
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    (void)remove("stream-ack"); /* CTest assigns a dedicated fixture directory. */
    UmiBuildRunnerConfig config = {0}; UmiBuildRunner *runner = NULL; UmiBuildResult *result = NULL;
    Probe probe = {0}; probe.name = argv[2];
    CHECK(umi_build_result_create(&result) == UMI_STATUS_OK); probe.result = result;
    CHECK(umi_cancellation_token_create(&probe.token) == UMI_STATUS_OK);
    umi_build_profile_init(&config.profile);
    CHECK(strlen(argv[1]) < sizeof(config.profile.run_program)); strcpy(config.profile.run_program, argv[1]);
    (void)snprintf(config.profile.run_argument, sizeof(config.profile.run_argument), "%s", probe.name);
    config.profile.timeout_ms = strcmp(probe.name, "timeout") == 0 ? 250U : 4000U;
    config.cancellation = probe.token;
    CHECK(umi_build_runner_create(&config, &runner) == UMI_STATUS_OK);
    UmiStatus status;
    if (strcmp(probe.name, "invalid") == 0) {
        CHECK(UmiBuildRunnerRunObserved(NULL, UMI_BUILD_PHASE_RUN, Observe, &probe, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunnerRunObserved(runner, UMI_BUILD_PHASE_RUN, Observe, &probe, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        status = strcmp(probe.name, "legacy") == 0 ? umi_build_runner_run(runner, UMI_BUILD_PHASE_RUN, result) :
            UmiBuildRunnerRunObserved(runner, UMI_BUILD_PHASE_RUN, Observe, &probe, result);
        if (strcmp(probe.name, "cancel") == 0) CHECK(status == UMI_STATUS_CANCELLED);
        else if (strcmp(probe.name, "timeout") == 0) CHECK(status == UMI_STATUS_TIMEOUT);
        else if (strcmp(probe.name, "failure") == 0) CHECK(status != UMI_STATUS_OK && result->exit_code == 5);
        else CHECK(status == UMI_STATUS_OK);
        if (strcmp(probe.name, "legacy") == 0) CHECK(probe.length == 0U && strstr(result->output, "ready") != NULL);
        else CHECK(probe.length != 0U);
        if (strcmp(probe.name, "live") == 0 || strcmp(probe.name, "legacy") == 0) CHECK(result->diagnostics.count != 0U);
        if (strcmp(probe.name, "unicode") == 0) {
            const char expected[] = {'c', 'a', 'f', '\xc3', '\xa9', '\0', 'x'}; int found = 0;
            for (size_t i = 0U; i + sizeof(expected) <= probe.length; ++i)
                if (memcmp(probe.bytes + i, expected, sizeof(expected)) == 0) found = 1;
            CHECK(found);
        }
    }
    umi_build_runner_destroy(runner); umi_build_result_destroy(result); umi_cancellation_token_destroy(probe.token);
    (void)remove("stream-ack"); return 0;
}
