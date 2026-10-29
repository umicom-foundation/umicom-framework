/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_progress_archive.c
 *
 * PURPOSE:
 *   Export private checkpoint history and import it only into an empty matching journal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_progress_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiLearningProgressExport(const UmiLearningProgress *progress,
    void *out_bytes, size_t capacity, size_t *out_size)
{
    if (progress == NULL || out_size == NULL || (out_bytes == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    char header[160];
    int n = snprintf(header, sizeof(header), "ULP1|%s|%zu\n", progress->learner_id, progress->state->count);
    if (n < 0 || (size_t)n >= sizeof(header)) return UMI_STATUS_INTERNAL_ERROR;
    size_t required = (size_t)n;
    for (size_t i = 0U; i < progress->state->count; ++i)
        required += strlen(progress->state->wire[i]) + 1U;
    if (required > UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (capacity <= required) { *out_size = required; return UMI_STATUS_CAPACITY_EXCEEDED; }
    char *output = out_bytes;
    memcpy(output, header, (size_t)n);
    size_t at = (size_t)n;
    for (size_t i = 0U; i < progress->state->count; ++i) {
        size_t length = strlen(progress->state->wire[i]);
        memcpy(output + at, progress->state->wire[i], length);
        at += length;
        output[at++] = '\n';
    }
    output[at] = '\0';
    *out_size = at;
    return UMI_STATUS_OK;
}
static char *Line(char **cursor)
{
    char *line = *cursor, *end = strchr(line, '\n');
    if (end == NULL) return NULL;
    *end = '\0';
    if (end != line && end[-1] == '\r') end[-1] = '\0';
    *cursor = end + 1;
    return line;
}
static UmiStatus ArchiveCount(const char *text, size_t *out)
{
    if (strcmp(text, "0") == 0) { *out = 0U; return UMI_STATUS_OK; }
    if (text[0] < '1' || text[0] > '9') return UMI_STATUS_PARSE_ERROR;
    size_t count = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (text[i] < '0' || text[i] > '9') return UMI_STATUS_PARSE_ERROR;
        size_t digit = (size_t)(text[i] - '0');
        if (count > (UMI_LEARNING_PROGRESS_MAX_EVENTS - digit) / 10U)
            return UMI_STATUS_PARSE_ERROR;
        count = count * 10U + digit;
    }
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus ParseArchive(const UmiLearningProgress *progress, char *copy, LpState *out)
{
    char *cursor = copy, *head = Line(&cursor);
    if (head == NULL || strncmp(head, "ULP1|", 5U) != 0) return UMI_STATUS_PARSE_ERROR;
    char *learner = head + 5U, *separator = strchr(learner, '|');
    if (separator == NULL) return UMI_STATUS_PARSE_ERROR;
    *separator = '\0';
    if (LpToken(learner) != UMI_STATUS_OK || strcmp(learner, progress->learner_id) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ArchiveCount(separator + 1, &out->count);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < out->count; ++i) {
        char *line = Line(&cursor);
        if (line == NULL || strlen(line) >= LP_WIRE_CAPACITY) return UMI_STATUS_PARSE_ERROR;
        status = LpDecode(line, (uint64_t)i + 1U, &out->events[i]);
        if (status != UMI_STATUS_OK) return status;
        for (size_t earlier = 0U; earlier < i; ++earlier)
            if (strcmp(out->wire[earlier], line) == 0) return UMI_STATUS_PARSE_ERROR;
        memcpy(out->wire[i], line, strlen(line) + 1U);
    }
    return cursor[0] == '\0' ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiLearningProgressImportEmpty(UmiLearningProgress *progress,
    const void *archive, size_t size)
{
    if (progress == NULL || archive == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress->recovery_required) return UMI_STATUS_INVALID_STATE;
    if (progress->state->count != 0U) return UMI_STATUS_ALREADY_EXISTS;
    if (umi_data_server_is_readonly(progress->server)) return UMI_STATUS_PERMISSION_DENIED;
    if (size > UMI_LEARNING_PROGRESS_MAX_ARCHIVE_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (size == 0U || memchr(archive, '\0', size) != NULL) return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(size + 1U);
    LpState *incoming = calloc(1U, sizeof(*incoming)), *current = calloc(1U, sizeof(*current));
    if (copy == NULL || incoming == NULL || current == NULL) {
        free(copy); free(incoming); free(current);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, archive, size); copy[size] = '\0';
    UmiStatus status = ParseArchive(progress, copy, incoming);
    free(copy);
    if (status == UMI_STATUS_OK) {
        if (umi_data_server_in_transaction(progress->server)) status = UMI_STATUS_BUSY;
        else status = umi_data_server_begin(progress->server);
        if (status == UMI_STATUS_OK) {
            status = LpLoad(progress, current);
            if (status == UMI_STATUS_OK && current->count != 0U) status = UMI_STATUS_ALREADY_EXISTS;
            if (status == UMI_STATUS_OK) status = LpStore(progress, incoming, 0U);
            else status = LpRollback(progress, status);
            if (status == UMI_STATUS_OK) {
                free(progress->state);
                progress->state = incoming;
                incoming = NULL;
            }
        }
    }
    free(current);
    free(incoming);
    return status;
}
