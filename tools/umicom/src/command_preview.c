/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_preview.c
 *
 * PURPOSE:
 *   Expose the Framework-owned local static preview with explicit root, port and bounded shutdown polling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/web/preview.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
static volatile sig_atomic_t preview_stop_requested;
static void PreviewStopSignal(int number)
{
    (void)number;
    preview_stop_requested = 1;
}
int umi_cli_command_preview(int argc, char **argv)
{
    if (argc < 0 || (argc > 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (!strcmp(argv[0], "--help") || !strcmp(argv[0], "-h"))) {
        (void)puts("Usage: umicom preview --root ABSOLUTE_DIRECTORY --port 1024..65535 [--dry-run]\n"
            "Serve selected static files on 127.0.0.1. Stop with Ctrl+C.\n"
            "GET/HEAD only; complete files must be smaller than 16 KiB.\n"
            "Dry-run validates syntax only: no file access, socket or browser launch.\n"
            "Local development only; no TLS, authentication or remote deployment.");
        return 0;
    }
    const char *root = NULL;
    uint32_t port = 0U;
    int port_seen = 0, dry_run = 0;
    /* Admit the whole vector before installing handlers or opening a socket. */
    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "--dry-run")) {
            if (dry_run) return 2;
            dry_run = 1;
        } else if (!strcmp(argv[i], "--root")) {
            if (root != NULL || i + 1 >= argc) return 2;
            root = argv[++i];
        } else if (!strcmp(argv[i], "--port")) {
            if (port_seen || i + 1 >= argc) return 2;
            if (UmiWebPreviewPortParse(argv[++i], &port) != UMI_STATUS_OK) return 2;
            port_seen = 1;
        } else return 2;
    }
    if (root == NULL || !port_seen || UmiWebPreviewValidate(root, port) != UMI_STATUS_OK)
        return 2;
    if (dry_run) {
        (void)printf("Local preview plan: http://127.0.0.1:%u/\nRoot: %s\n"
            "Syntax admitted. Root contents and port availability are not verified.\n", (unsigned)port, root);
        return ferror(stdout) ? 1 : 0;
    }
    UmiWebPreview *preview = NULL;
    UmiStatus status = UmiWebPreviewCreate(root, port, &preview);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Preview creation failed: %s\n", umi_status_text(status));
        return 1;
    }
    preview_stop_requested = 0;
    void (*previous_int)(int) = signal(SIGINT, PreviewStopSignal);
    if (previous_int == SIG_ERR) { UmiWebPreviewDestroy(preview); return 1; }
    void (*previous_term)(int) = signal(SIGTERM, PreviewStopSignal);
    if (previous_term == SIG_ERR) {
        (void)signal(SIGINT, previous_int);
        UmiWebPreviewDestroy(preview);
        return 1;
    }
    status = UmiWebPreviewStart(preview);
    int failed = status != UMI_STATUS_OK;
    if (!failed) {
        (void)printf("Local preview: http://127.0.0.1:%u/\nRoot: %s\nStop with Ctrl+C.\n",
            (unsigned)port, root);
        if (fflush(stdout) != 0) { failed = 1; status = UMI_STATUS_IO_ERROR; }
    }
    while (!failed && !preview_stop_requested) {
        UmiWebExchangeResult result;
        status = UmiWebPreviewServeNext(preview, 100U, 3000U, NULL, &result);
        if (status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT || status == UMI_STATUS_UNAVAILABLE)
            continue;
        (void)fprintf(stderr, "Preview exchange: %s; response %s.\n",
            umi_status_text(status), result.response_complete ? "sent" : "incomplete");
        /* A rejected/failed client is not a dead listener. Conversely, failure
         * before any request progress must not become an endless error loop. */
        if (!result.dispatched && result.bytes_received == 0U) failed = 1;
    }
    if (failed) (void)fprintf(stderr, "Preview stopped with failure: %s\n", umi_status_text(status));
    UmiWebPreviewDestroy(preview);
    if (signal(SIGTERM, previous_term) == SIG_ERR) failed = 1;
    if (signal(SIGINT, previous_int) == SIG_ERR) failed = 1;
    return failed ? 1 : 0;
}
