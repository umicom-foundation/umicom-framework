/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_console_owner.c
 *
 * PURPOSE:
 *   Verify model console cleanup obligations remain reachable after command failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "umicom/local_models/manager.h"

static max_align_t fake_owner_storage;
static UmiModelSession *const fake_owner = (UmiModelSession *)&fake_owner_storage;
static unsigned starts, reads, stops;
static UmiStatus start_result = UMI_STATUS_OK, stop_result = UMI_STATUS_OK;
static bool publish_start_owner = true, fail_late_output, consume_stop_failure;

static UmiStatus owner_start(const UmiModelRunPlan *plan, bool approved, UmiModelSession **slot)
{
    (void)plan; assert(approved && slot != NULL && *slot == NULL);
    ++starts; if (publish_start_owner) *slot = fake_owner;
    return start_result;
}
static UmiStatus owner_read(const UmiModelSession *session, UmiProcessJobSnapshot *out)
{
    assert(session == fake_owner && out != NULL); ++reads;
    memset(out, 0, sizeof(*out)); out->state = UMI_PROCESS_JOB_RUNNING;
    return UMI_STATUS_OK;
}
static UmiStatus owner_stop(UmiModelSession **slot)
{
    assert(slot != NULL);
    if (*slot == NULL) return UMI_STATUS_OK;
    assert(*slot == fake_owner); ++stops;
    if (stop_result == UMI_STATUS_OK || consume_stop_failure) *slot = NULL;
    return stop_result;
}
static char *owner_input(char *line, int capacity, FILE *stream)
{
    (void)line; (void)capacity; assert(stream == stdin);
    return NULL; /* Deterministic end-of-input requests ordinary session stop. */
}
static int owner_stream_error(FILE *stream)
{
    return stream == stdout && fail_late_output && starts != 0U ? 1 : ferror(stream);
}

/* Use the actual parser and console loop. Inject only the process, input and
 * late output boundaries so this test never starts a child or waits for input. */
#define UmiModelSessionStart owner_start
#define UmiModelSessionRead owner_read
#define UmiModelSessionStop owner_stop
#define fgets owner_input
#define ferror owner_stream_error
#include "../../src/local_models/cli.c"
#undef ferror
#undef fgets
#undef UmiModelSessionStop
#undef UmiModelSessionRead
#undef UmiModelSessionStart

#ifdef _WIN32
#define SERVER_PATH "C:/umicom-test/llama-server.exe"
#define MODEL_PATH "C:/umicom-test/local.gguf"
#else
#define SERVER_PATH "/umicom-test/llama-server"
#define MODEL_PATH "/umicom-test/local.gguf"
#endif

int main(int argc, char **argv)
{
    char *run[] = {"run", "qwen2.5-0.5b-q4", SERVER_PATH, MODEL_PATH, "18080", "2048", "4", "--apply"};
    char *catalogue[] = {"catalogue"};
    UmiModelSession *session = NULL;
    int result;
    assert(argc == 2);
    assert(!UmiModelsCommandHasRetainedSession());
    assert(UmiModelsCommandRelease() == UMI_STATUS_OK);
    if (strcmp(argv[1], "null") == 0) {
        assert(UmiModelsCommandWithSession(8, run, NULL) == 2);
        assert(starts == 0U && stops == 0U); return 0;
    }
    if (strcmp(argv[1], "occupied") == 0) {
        session = fake_owner;
        assert(UmiModelsCommandWithSession(-1, NULL, &session) == 1);
        assert(session == fake_owner && starts == 0U && stops == 0U);
        assert(owner_stop(&session) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(argv[1], "preview") == 0) {
        assert(UmiModelsCommandWithSession(7, run, &session) == 0);
        assert(session == NULL && starts == 0U && stops == 0U); return 0;
    }
    if (strcmp(argv[1], "legacy-retained") == 0 || strcmp(argv[1], "legacy-consumed") == 0) {
        stop_result = UMI_STATUS_IO_ERROR;
        assert(UmiModelsCommand(8, run) == 1);
        assert(UmiModelsCommandHasRetainedSession() && starts == 1U && stops == 1U);
        assert(UmiModelsCommand(1, catalogue) == 1);
        assert(starts == 1U && stops == 1U);
        assert(UmiModelsCommandRelease() == UMI_STATUS_IO_ERROR);
        assert(UmiModelsCommandHasRetainedSession() && stops == 2U);
        if (strcmp(argv[1], "legacy-consumed") == 0) {
            consume_stop_failure = true;
            assert(UmiModelsCommandRelease() == UMI_STATUS_IO_ERROR);
            assert(!UmiModelsCommandHasRetainedSession() && stops == 3U);
            assert(UmiModelsCommand(1, catalogue) == 0);
            return 0;
        }
        stop_result = UMI_STATUS_OK;
        assert(UmiModelsCommandRelease() == UMI_STATUS_OK);
        assert(!UmiModelsCommandHasRetainedSession() && stops == 3U);
        assert(UmiModelsCommandRelease() == UMI_STATUS_OK && stops == 3U);
        assert(UmiModelsCommand(1, catalogue) == 0); return 0;
    }
    if (strcmp(argv[1], "start-unowned") == 0) {
        start_result = UMI_STATUS_IO_ERROR; publish_start_owner = false;
    } else if (strcmp(argv[1], "start-clean") == 0) start_result = UMI_STATUS_IO_ERROR;
    else if (strcmp(argv[1], "start-retained") == 0) {
        start_result = UMI_STATUS_IO_ERROR; stop_result = UMI_STATUS_IO_ERROR;
    } else if (strcmp(argv[1], "start-consumed") == 0) {
        start_result = UMI_STATUS_IO_ERROR; stop_result = UMI_STATUS_IO_ERROR; consume_stop_failure = true;
    } else if (strcmp(argv[1], "stop-consumed") == 0) {
        stop_result = UMI_STATUS_IO_ERROR; consume_stop_failure = true;
    } else if (strcmp(argv[1], "stop-retained") == 0) stop_result = UMI_STATUS_IO_ERROR;
    else if (strcmp(argv[1], "output-clean") == 0) fail_late_output = true;
    else if (strcmp(argv[1], "output-retained") == 0) {
        fail_late_output = true; stop_result = UMI_STATUS_IO_ERROR;
    } else assert(strcmp(argv[1], "success") == 0);
    result = UmiModelsCommandWithSession(8, run, &session);
    assert(starts == 1U);
    assert(result == (strcmp(argv[1], "success") == 0 ? 0 : 1));
    assert(stops == (publish_start_owner ? 1U : 0U));
    if (stop_result != UMI_STATUS_OK && !consume_stop_failure) {
        assert(session == fake_owner);
        assert(UmiModelsCommandWithSession(1, catalogue, &session) == 1);
        assert(starts == 1U && stops == 1U);
        /* Compatibility cleanup owns only its own slot. It must not consume
         * a session returned to an explicit embedding caller. */
        assert(!UmiModelsCommandHasRetainedSession());
        assert(UmiModelsCommandRelease() == UMI_STATUS_OK && session == fake_owner);
        stop_result = UMI_STATUS_OK;
        assert(owner_stop(&session) == UMI_STATUS_OK && session == NULL);
    } else assert(session == NULL);
    return 0;
}
