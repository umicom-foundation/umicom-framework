/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_observe_files.c
 *
 * PURPOSE:
 *   Expose selected-file byte observations through the shared Framework evidence service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/toolchain/file_observation.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ParseLimit(const char *text, size_t *out)
{
    size_t value = 0U;
    if (text == NULL || *text == '\0') return 0;
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
        if (*p < '0' || *p > '9') return 0;
        unsigned digit = (unsigned)(*p - '0');
        if (value > (UMI_FILE_OBSERVATION_MAX_BYTES - digit) / 10U) return 0;
        value = value * 10U + digit;
    }
    if (value == 0U) return 0;
    *out = value;
    return 1;
}
int umi_cli_command_observe_files(int argc, char **argv)
{
    if (argc < 0 || (argc != 0 && argv == NULL)) return 2;
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL || argv[i][0] == '\0') return 2;
    if (argc == 1 && (!strcmp(argv[0], "--help") || !strcmp(argv[0], "-h"))) {
        (void)puts("Usage: umicom observe-files --file ABSOLUTE_PATH [--file ABSOLUTE_PATH ...]\n"
            "       [--max-bytes N] [--report ABSOLUTE_NEW_FILE]\n"
            "Read up to 64 explicit files; default limit 64 MiB each, total 256 MiB.\n"
            "Emit JSON to stdout or save exclusively. No tool execution or loaded-module claim.");
        return 0;
    }
    const char *paths[UMI_FILE_OBSERVATION_MAX_FILES], *report = NULL;
    size_t count = 0U, maximum = UMI_FILE_OBSERVATION_MAX_BYTES;
    int limitSeen = 0;
    /* Admit the complete vector before any capture or file creation. The shared
     * service validates all selected paths before reading even its first file. */
    for (int i = 0; i < argc; ++i) {
        const char *option = argv[i];
        if (i + 1 >= argc) return 2;
        const char *value = argv[++i];
        if (!strcmp(option, "--file")) {
            if (count == UMI_FILE_OBSERVATION_MAX_FILES) return 2;
            paths[count++] = value;
        } else if (!strcmp(option, "--report")) {
            if (report != NULL || UmiOutputFileValidatePath(value) != UMI_STATUS_OK) return 2;
            report = value;
        } else if (!strcmp(option, "--max-bytes")) {
            if (limitSeen || !ParseLimit(value, &maximum)) return 2;
            limitSeen = 1;
        } else return 2;
    }
    if (count == 0U) return 2;
    UmiFileObservation *observation = NULL;
    size_t failedIndex;
    UmiStatus status = UmiFileObservationCapture(paths, count, maximum, &observation, &failedIndex);
    if (status != UMI_STATUS_OK) {
        if (failedIndex != SIZE_MAX)
            (void)fprintf(stderr, "File observation %zu failed: %s\n", failedIndex + 1U, umi_status_text(status));
        else (void)fprintf(stderr, "File observation failed: %s\n", umi_status_text(status));
        return status == UMI_STATUS_INVALID_ARGUMENT ? 2 : 1;
    }
    if (report != NULL) {
        status = UmiFileObservationSave(observation, report);
        if (status == UMI_STATUS_OK) (void)puts("Selected-file observation saved; qualification remains separate.");
    } else {
        char *json = malloc(UMI_FILE_OBSERVATION_JSON_CAPACITY);
        if (json == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else {
            size_t length;
            status = UmiFileObservationFormat(observation, json, UMI_FILE_OBSERVATION_JSON_CAPACITY, &length);
            if (status == UMI_STATUS_OK &&
                fwrite(json, 1U, length, stdout) != length) status = UMI_STATUS_IO_ERROR;
            free(json);
        }
    }
    UmiFileObservationDestroy(observation);
    if (status != UMI_STATUS_OK) (void)fprintf(stderr, "File observation output failed: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? 0 : 1;
}
