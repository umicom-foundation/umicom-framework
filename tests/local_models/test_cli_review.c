/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_cli_review.c
 *
 * PURPOSE:
 *   Inject console delivery failures and verify model CLI previews never cross into side effects.
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
#include <stdio.h>
#include <string.h>
#include "umicom/local_models/runtime.h"
static unsigned side_effects;
static int fail_flush, fail_stream;
static int review_flush(FILE *stream) { return stream == stdout && fail_flush ? EOF : fflush(stream); }
static int review_error(FILE *stream) { return stream == stdout && fail_stream ? 1 : ferror(stream); }
static UmiStatus forbidden_download(const UmiModelDownloadPlan *plan, bool approved,
    const UmiCancellationToken *token, UmiModelInspection *out)
{ (void)plan; (void)approved; (void)token; (void)out; ++side_effects; return UMI_STATUS_IO_ERROR; }
static UmiStatus forbidden_start(const UmiModelRunPlan *plan, bool approved, UmiModelSession **out)
{ (void)plan; (void)approved; (void)out; ++side_effects; return UMI_STATUS_IO_ERROR; }
static UmiStatus forbidden_provider(uint16_t port, uint32_t timeout,
    const UmiAiWorkspaceCancellation *token, UmiAiProvider *out)
{ (void)port; (void)timeout; (void)token; (void)out; ++side_effects; return UMI_STATUS_IO_ERROR; }
static UmiStatus forbidden_inspect(const char *path, const char *digest,
    const UmiCancellationToken *token, UmiModelInspection *out)
{ (void)path; (void)digest; (void)token; (void)out; ++side_effects; return UMI_STATUS_IO_ERROR; }
/* Compile the real parser/control flow with failure boundaries only. No real
 * download, model read, provider creation or process submission is permitted. */
#define fflush review_flush
#define ferror review_error
#define UmiModelDownload forbidden_download
#define UmiModelSessionStart forbidden_start
#define UmiModelLocalProviderCreate forbidden_provider
#define UmiModelInspect forbidden_inspect
#include "../../src/local_models/cli.c"
#undef UmiModelInspect
#undef UmiModelLocalProviderCreate
#undef UmiModelSessionStart
#undef UmiModelDownload
#undef ferror
#undef fflush

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
    char *serve[] = {"serve", SERVER_PATH, MODEL_PATH,
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "fixture", "18080", "2048", "4", "--apply"};
    char *download[] = {"download", "qwen2.5-0.5b-q4", MODEL_PATH, "--apply"};
    char *chat[] = {"chat", "18080", "fixture", "Reviewed text", "--send-reviewed-prompt"};
    char *describe[] = {"describe", "qwen2.5-0.5b-q4", SERVER_PATH, MODEL_PATH, "18080", "2048", "4", "--apply"};
    char *custom[] = {"describe-file", SERVER_PATH, MODEL_PATH,
        "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "fixture", "18080", "2048", "4", "--apply"};
    assert(argc == 2);
    if (strcmp(argv[1], "describe") == 0) {
        assert(UmiModelsCommand(7, describe) == 0);
        assert(UmiModelsCommand(8, custom) == 0);
    } else if (strcmp(argv[1], "describe-apply") == 0) {
        assert(UmiModelsCommand(8, describe) == 2);
        assert(UmiModelsCommand(9, custom) == 2);
    } else if (strcmp(argv[1], "describe-invalid") == 0) {
        custom[6] = "0"; assert(UmiModelsCommand(8, custom) == 2);
        describe[1] = "missing"; assert(UmiModelsCommand(7, describe) == 1);
    } else {
        fail_flush = strcmp(argv[1], "flush") == 0;
        fail_stream = strcmp(argv[1], "stream") == 0;
        assert(fail_flush || fail_stream);
        assert(UmiModelsCommand(4, download) == 1);
        assert(UmiModelsCommand(8, run) == 1);
        assert(UmiModelsCommand(9, serve) == 1);
        assert(UmiModelsCommand(5, chat) == 1);
        assert(UmiModelsCommand(3, download) == 1);
        assert(UmiModelsCommand(7, run) == 1);
        assert(UmiModelsCommand(4, chat) == 1);
        assert(UmiModelsCommand(7, describe) == 1);
        assert(UmiModelsCommand(8, custom) == 1);
    }
    assert(side_effects == 0U);
    return 0;
}
