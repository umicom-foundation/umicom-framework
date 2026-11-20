/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_source_excerpt_command.c
 *
 * PURPOSE:
 *   Inspect and repeat reviewed native Media source selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_source_excerpt.h"
#include "umicom/base/ascii_number.h"
#include "umicom/security/secrets.h"
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif

/* The host passes original UTF-8 arguments. Accept only positive line
 * numbers that fit the platform's native file counters. */
static UmiStatus ParseLine(const char *argument, size_t *line)
{
    uint64_t candidate = 0U;
    UmiStatus status = umi_ascii_uint64_parse(argument, strlen(argument), &candidate);
    if (status != UMI_STATUS_OK) return status;
    if (candidate == 0U || candidate > SIZE_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    *line = (size_t)candidate;
    return UMI_STATUS_OK;
}

/* A repeat selection writes source bytes exactly, including embedded LF or
 * CRLF. Windows text mode would otherwise introduce an extra carriage return. */
static int WriteExcerpt(const UmiMediaSourceExcerpt *excerpt)
{
#if defined(_WIN32)
    if (fflush(stdout) != 0) return 1;
    const int descriptor = _fileno(stdout);
    const int previous_mode = _setmode(descriptor, _O_BINARY);
    if (previous_mode == -1) return 1;
#endif
    const size_t written = fwrite(excerpt->text, 1U, excerpt->byte_count, stdout);
    int result = fflush(stdout) != 0 || written != excerpt->byte_count || ferror(stdout);
#if defined(_WIN32)
    if (_setmode(descriptor, previous_mode) == -1) result = 1;
#endif
    return result;
}

int UmiMediaSourceExcerptCommand(int argc, char **argv)
{
    /* Keep the review receipt free of private paths and excerpt text.
     * The caller explicitly supplies the full approved digest to emit bytes. */
    if (argc == 1 && strcmp(argv[0], "--help") == 0) {
        fputs("source inspect ABSOLUTE_UTF8_FILE FIRST_LINE LAST_LINE\n"
              "source excerpt ABSOLUTE_UTF8_FILE FIRST_LINE LAST_LINE SOURCE_SHA256\n", stdout);
        return fflush(stdout) != 0 || ferror(stdout);
    }
    const bool inspect = argc == 4 && strcmp(argv[0], "inspect") == 0;
    const bool excerpt = argc == 5 && strcmp(argv[0], "excerpt") == 0;
    if (!inspect && !excerpt) {
        fputs("Choose source inspect or source excerpt. See source --help.\n", stderr);
        return 2;
    }
    size_t first_line = 0U;
    size_t last_line = 0U;
    UmiStatus status = ParseLine(argv[2], &first_line);
    if (status == UMI_STATUS_OK) status = ParseLine(argv[3], &last_line);
    if (status != UMI_STATUS_OK || last_line < first_line) {
        fputs("Select an ascending range of positive decimal line numbers.\n", stderr);
        return 2;
    }

    UmiMediaSourceExcerptObservation observation = {0};
    status = UmiMediaSourceExcerptInspectUtf8(argv[1], first_line, last_line,
        excerpt ? argv[4] : NULL, NULL, &observation);
    if (status != UMI_STATUS_OK) {
        /* The input service never publishes receipt data on a failed read. */
        fputs(status == UMI_STATUS_INVALID_STATE
            ? "Source bytes changed. Inspect the selection again before requesting its text.\n"
            : "Unable to read the selected UTF-8 source range.\n", stderr);
        if (status != UMI_STATUS_INVALID_STATE) fprintf(stderr, "%s\n", umi_status_text(status));
        return 1;
    }
    int result;
    if (inspect) {
        const int printed = printf(
            "{\"firstLine\":%zu,\"lastLine\":%zu,\"byteOffset\":%zu,\"byteCount\":%zu,"
            "\"fileBytes\":%zu,\"sourceSha256\":\"%s\"}\n",
            observation.excerpt.first_line, observation.excerpt.last_line,
            observation.excerpt.byte_offset, observation.excerpt.byte_count,
            observation.file_bytes, observation.source_sha256);
        result = fflush(stdout) != 0 || printed < 0 || ferror(stdout);
    } else {
        result = WriteExcerpt(&observation.excerpt);
    }
    umi_secret_clear(&observation, sizeof(observation));
    if (result != 0) fputs("Could not write the requested output.\n", stderr);
    return result;
}
