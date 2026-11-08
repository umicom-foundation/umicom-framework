/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web/test_preview_cli.c
 *
 * PURPOSE:
 *   Test the real preview parser and loop against controlled session and signal boundaries without starting a server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../tools/umicom/src/cli.h"
#include "umicom/web/preview.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static const char *mode;
static unsigned creates, starts, destroys, polls, signal_calls;
static int valid_create = 1, valid_wait = 1;
static void PreviewStopSignal(int number);
#ifdef _WIN32
static const char root_value[] = "C:/site with spaces/caf\xc3\xa9";
#else
static const char root_value[] = "/site with spaces/caf\xc3\xa9";
#endif
static int Is(const char *text) { return strcmp(mode, text) == 0; }
static UmiStatus FakeCreate(const char *root, uint32_t port, UmiWebPreview **out)
{
    ++creates; valid_create = !strcmp(root, root_value) && port == 8080U;
    *out = NULL;
    if (Is("create-failure")) return UMI_STATUS_OUT_OF_MEMORY;
    *out = (UmiWebPreview *)(void *)&creates; return UMI_STATUS_OK;
}
static UmiStatus FakeStart(UmiWebPreview *preview)
{
    ++starts; if (preview != (UmiWebPreview *)(void *)&creates) valid_create = 0;
    return Is("start-failure") ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static void FakeDestroy(UmiWebPreview *preview)
{
    ++destroys; if (preview != (UmiWebPreview *)(void *)&creates) valid_create = 0;
}
static UmiStatus FakeServe(UmiWebPreview *preview, uint32_t wait_ms, uint32_t exchange_ms,
    const UmiCancellationToken *cancel, UmiWebExchangeResult *out)
{
    ++polls; memset(out, 0, sizeof(*out));
    valid_wait = preview == (UmiWebPreview *)(void *)&creates && wait_ms == 100U &&
        exchange_ms == 3000U && cancel == NULL;
    if (Is("listener-failure")) return UMI_STATUS_IO_ERROR;
    if (polls == 1U && Is("client-timeout")) return UMI_STATUS_TIMEOUT;
    if (polls == 1U && Is("client-close")) return UMI_STATUS_UNAVAILABLE;
    if (polls == 1U && Is("bad-request")) {
        out->bytes_received = 12U; out->response_complete = true;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    PreviewStopSignal(SIGINT); return UMI_STATUS_OK;
}
typedef void (*SignalHandler)(int);
static SignalHandler FakeSignal(int number, SignalHandler handler)
{
    ++signal_calls;
    if (signal_calls == 1U && number != SIGINT) valid_wait = 0;
    if (signal_calls == 2U && number != SIGTERM) valid_wait = 0;
    if ((signal_calls == 1U && Is("signal-int-failure")) ||
        (signal_calls == 2U && Is("signal-term-failure")) ||
        (signal_calls == 3U && Is("restore-failure"))) return SIG_ERR;
    if (signal_calls <= 2U && handler != PreviewStopSignal) valid_wait = 0;
    if (signal_calls > 2U && handler != SIG_DFL) valid_wait = 0;
    return SIG_DFL;
}
#define UmiWebPreviewCreate FakeCreate
#define UmiWebPreviewStart FakeStart
#define UmiWebPreviewDestroy FakeDestroy
#define UmiWebPreviewServeNext FakeServe
#define signal FakeSignal
#include "../../tools/umicom/src/command_preview.c"
#undef signal
#undef UmiWebPreviewServeNext
#undef UmiWebPreviewDestroy
#undef UmiWebPreviewStart
#undef UmiWebPreviewCreate
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    const char *cases[] = {"help","missing-root","missing-port","root-value","port-value","duplicate-root","duplicate-port","duplicate-dry-run","null-vector","null-item","empty-item","negative-argc","unknown","late-unknown","relative-root","overflow-port","low-port","high-port","zero-port","empty-argv","dry-run","order","create-failure","start-failure","signal-int-failure","signal-term-failure","normal-stop","client-timeout","client-close","bad-request","listener-failure","restore-failure"};

    size_t matched = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) matched += (size_t)Is(cases[i]);
    if (matched != 1U) return 2;
    char *options[12] = {"--root", (char *)root_value, "--port", "8080"};
    char **arguments = options;
    int count = 4, expected = 2;
    unsigned expected_creates = 0U, expected_starts = 0U, expected_destroys = 0U, expected_polls = 0U, expected_signals = 0U;
    if (Is("help")) { options[0] = "--help"; count = 1; expected = 0; }
    else if (Is("missing-root")) { arguments = options + 2; count = 2; }
    else if (Is("missing-port")) count = 2;
    else if (Is("root-value")) count = 1;
    else if (Is("port-value")) count = 3;
    else if (Is("duplicate-root")) { options[4] = "--root"; options[5] = (char *)root_value; count = 6; }
    else if (Is("duplicate-port")) { options[4] = "--port"; options[5] = "8080"; count = 6; }
    else if (Is("duplicate-dry-run")) { options[4] = "--dry-run"; options[5] = "--dry-run"; count = 6; }
    else if (Is("null-vector")) arguments = NULL;
    else if (Is("null-item")) options[3] = NULL;
    else if (Is("empty-item")) options[3] = "";
    else if (Is("negative-argc")) count = -1;
    else if (Is("unknown")) options[0] = "--unknown";
    else if (Is("late-unknown")) { options[4] = "--unknown"; count = 5; }
    else if (Is("relative-root")) options[1] = ".";
    else if (Is("overflow-port")) options[3] = "18446744073709551616";
    else if (Is("low-port")) options[3] = "1023";
    else if (Is("high-port")) options[3] = "65536";
    else if (Is("zero-port")) options[3] = "0";
    else if (Is("empty-argv")) { arguments = NULL; count = 0; }
    else if (Is("dry-run")) { options[4] = "--dry-run"; count = 5; expected = 0; }
    else if (Is("order")) {
        options[0] = "--dry-run"; options[1] = "--port"; options[2] = "8080";
        options[3] = "--root"; options[4] = (char *)root_value; count = 5; expected = 0;
    } else {
        expected_creates = 1U; expected_destroys = 1U; expected_starts = 1U;
        expected_signals = 4U; expected_polls = 1U; expected = 0;
        if (Is("create-failure")) {
            expected = 1; expected_destroys = expected_starts = expected_signals = expected_polls = 0U;
        } else if (Is("signal-int-failure")) {
            expected = 1; expected_signals = 1U; expected_starts = expected_polls = 0U;
        } else if (Is("signal-term-failure")) {
            expected = 1; expected_signals = 3U; expected_starts = expected_polls = 0U;
        } else if (Is("start-failure")) { expected = 1; expected_polls = 0U; }
        else if (Is("listener-failure") || Is("restore-failure")) expected = 1;
        else if (Is("client-timeout") || Is("client-close") || Is("bad-request")) expected_polls = 2U;
    }
    CHECK(umi_cli_command_preview(count, arguments) == expected);
    CHECK(creates == expected_creates && starts == expected_starts && destroys == expected_destroys);
    CHECK(polls == expected_polls && signal_calls == expected_signals);
    CHECK(valid_create && valid_wait);
    return 0;
}
