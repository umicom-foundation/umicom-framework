/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_progress_storage.c
 *
 * PURPOSE:
 *   Parse bounded checkpoint journals and reject incomplete or unknown history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_progress_internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int AlphaNumeric(unsigned char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}
UmiStatus LpToken(const char *text)
{
    if (text == NULL || !AlphaNumeric((unsigned char)text[0])) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t n = 1U; n < UMI_TEACHER_ID_CAPACITY; ++n) {
        unsigned char c = (unsigned char)text[n];
        if (c == 0U) return UMI_STATUS_OK;
        if (!AlphaNumeric(c) && c != '.' && c != '-' && c != '_') return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus Number(const char *text, uint64_t maximum, uint64_t *out)
{
    if (text[0] < '1' || text[0] > '9') return UMI_STATUS_PARSE_ERROR;
    uint64_t number = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (text[i] < '0' || text[i] > '9') return UMI_STATUS_PARSE_ERROR;
        uint64_t digit = (uint64_t)(text[i] - '0');
        if (digit > maximum || number > (maximum - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        number = number * 10U + digit;
    }
    *out = number;
    return UMI_STATUS_OK;
}
UmiStatus LpKey(const UmiLearningProgress *progress, const char *suffix, char out[LP_KEY_CAPACITY])
{
    int n = snprintf(out, LP_KEY_CAPACITY, "%s%s", progress->prefix, suffix);
    return n < 0 || (size_t)n >= LP_KEY_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
UmiStatus LpEncode(const UmiLearningProgressEvent *event, char out[LP_WIRE_CAPACITY])
{
    int n = snprintf(out, LP_WIRE_CAPACITY, "LP1|%s|%" PRIu64 "|%s|%s|%s|%s",
        event->pack_id, event->pack_revision, event->resource_id, event->sha256,
        event->checkpoint, event->evidence_reference);
    return n < 0 || (size_t)n >= LP_WIRE_CAPACITY ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/* Journal reload and private archive import share one strict event decoder.
 * Retain the former file-local declaration for review. */
#if 0
static UmiStatus Decode(const char *wire, uint64_t sequence, UmiLearningProgressEvent *out)
#endif
UmiStatus LpDecode(const char *wire, uint64_t sequence, UmiLearningProgressEvent *out)
{
    char copy[LP_WIRE_CAPACITY], *parts[7];
    size_t length = strlen(wire);
    if (length >= sizeof(copy)) return UMI_STATUS_PARSE_ERROR;
    memcpy(copy, wire, length + 1U);
    parts[0] = copy;
    for (size_t i = 1U; i < 7U; ++i) {
        char *separator = strchr(parts[i - 1U], '|');
        if (separator == NULL) return UMI_STATUS_PARSE_ERROR;
        *separator = '\0';
        parts[i] = separator + 1;
    }
    if (strchr(parts[6], '|') != NULL || strcmp(parts[0], "LP1") != 0) return UMI_STATUS_PARSE_ERROR;
    if (LpToken(parts[1]) != UMI_STATUS_OK || LpToken(parts[3]) != UMI_STATUS_OK ||
        LpToken(parts[5]) != UMI_STATUS_OK || LpToken(parts[6]) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
    UmiLearningProgressEvent event = {0};
    if (Number(parts[2], UINT64_MAX, &event.pack_revision) != UMI_STATUS_OK ||
        strlen(parts[4]) != UMI_SHA256_HEX_CAPACITY - 1U) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < UMI_SHA256_HEX_CAPACITY - 1U; ++i)
        if (!((parts[4][i] >= '0' && parts[4][i] <= '9') ||
              (parts[4][i] >= 'a' && parts[4][i] <= 'f'))) return UMI_STATUS_PARSE_ERROR;
    event.sequence = sequence;
    memcpy(event.pack_id, parts[1], strlen(parts[1]) + 1U);
    memcpy(event.resource_id, parts[3], strlen(parts[3]) + 1U);
    memcpy(event.sha256, parts[4], UMI_SHA256_HEX_CAPACITY);
    memcpy(event.checkpoint, parts[5], strlen(parts[5]) + 1U);
    memcpy(event.evidence_reference, parts[6], strlen(parts[6]) + 1U);
    *out = event;
    return UMI_STATUS_OK;
}
typedef struct LpVisit {
    const UmiLearningProgress *owner;
    const LpState *candidate;
    size_t matched;
    const char *head;
} LpVisit;
static UmiStatus InspectRecord(const char *key, const char *value, void *data)
{
    LpVisit *visit = data;
    size_t prefix_length = strlen(visit->owner->prefix);
    if (strncmp(key, visit->owner->prefix, prefix_length) != 0) return UMI_STATUS_OK;
    const char *suffix = key + prefix_length;
    if (strcmp(suffix, "head") == 0) {
        if (visit->candidate->count == 0U || strcmp(value, visit->head) != 0)
            return UMI_STATUS_PARSE_ERROR;
    } else {
        if (strncmp(suffix, "event/", 6U) != 0) return UMI_STATUS_PARSE_ERROR;
        uint64_t number = 0U;
        if (Number(suffix + 6U, visit->candidate->count, &number) != UMI_STATUS_OK)
            return UMI_STATUS_PARSE_ERROR;
        if (strcmp(value, visit->candidate->wire[(size_t)number - 1U]) != 0)
            return UMI_STATUS_PARSE_ERROR;
    }
    ++visit->matched;
    return UMI_STATUS_OK;
}
UmiStatus LpLoad(UmiLearningProgress *progress, LpState *out)
{
    char key[LP_KEY_CAPACITY], head[32];
    UmiStatus status = LpKey(progress, "head", key);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_get(progress->server, key, head, sizeof(head));
    if (status == UMI_STATUS_NOT_FOUND) {
        /* Orphaned or unknown records must never look like a fresh learner. */
        LpVisit visit = {progress, out, 0U, ""};
        return umi_data_server_visit(progress->server, InspectRecord, &visit);
    }
    if (status != UMI_STATUS_OK) return status;
    uint64_t count = 0U;
    if (strncmp(head, "LP1|", 4U) != 0 ||
        Number(head + 4U, UMI_LEARNING_PROGRESS_MAX_EVENTS, &count) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
    out->count = (size_t)count;
    for (size_t i = 0U; i < out->count; ++i) {
        char suffix[32];
        int n = snprintf(suffix, sizeof(suffix), "event/%zu", i + 1U);
        if (n < 0 || (size_t)n >= sizeof(suffix)) return UMI_STATUS_INTERNAL_ERROR;
        status = LpKey(progress, suffix, key);
        if (status == UMI_STATUS_OK)
            status = umi_data_server_get(progress->server, key, out->wire[i], sizeof(out->wire[i]));
        if (status != UMI_STATUS_OK) return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
        status = LpDecode(out->wire[i], (uint64_t)i + 1U, &out->events[i]);
        if (status != UMI_STATUS_OK) return status;
        /* The append API never emits an identical event twice. Apply the same
         * invariant to stored history and imported archives. */
        for (size_t earlier = 0U; earlier < i; ++earlier)
            if (strcmp(out->wire[earlier], out->wire[i]) == 0) return UMI_STATUS_PARSE_ERROR;
    }
    LpVisit visit = {progress, out, 0U, head};
    status = umi_data_server_visit(progress->server, InspectRecord, &visit);
    if (status == UMI_STATUS_OK && visit.matched != out->count + 1U) status = UMI_STATUS_PARSE_ERROR;
    return status;
}
UmiStatus LpRollback(UmiLearningProgress *progress, UmiStatus prior)
{
    UmiStatus rollback = umi_data_server_rollback(progress->server);
    if (rollback != UMI_STATUS_OK) {
        progress->recovery_required = 1;
        return rollback;
    }
    return prior;
}

/* One storage boundary serves individual checkpoints and empty-journal import.
 * Each caller already holds the Data Server transaction. Do not publish the
 * candidate projection until both its events and header have committed. */
UmiStatus LpStore(UmiLearningProgress *progress, const LpState *candidate, size_t first)
{
    UmiStatus status = UMI_STATUS_OK;
    if (first > candidate->count || candidate->count > UMI_LEARNING_PROGRESS_MAX_EVENTS)
        return LpRollback(progress, UMI_STATUS_INVALID_ARGUMENT);
    if (candidate->count == 0U) return LpRollback(progress, UMI_STATUS_OK);
    char key[LP_KEY_CAPACITY];
    for (size_t i = first; status == UMI_STATUS_OK && i < candidate->count; ++i) {
        char suffix[32];
        int n = snprintf(suffix, sizeof(suffix), "event/%zu", i + 1U);
        if (n < 0 || (size_t)n >= sizeof(suffix)) status = UMI_STATUS_INTERNAL_ERROR;
        if (status == UMI_STATUS_OK) status = LpKey(progress, suffix, key);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(progress->server, key, candidate->wire[i]);
    }
    char head[32];
    int n = snprintf(head, sizeof(head), "LP1|%zu", candidate->count);
    if (n < 0 || (size_t)n >= sizeof(head)) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) status = LpKey(progress, "head", key);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(progress->server, key, head);
    if (status != UMI_STATUS_OK) return LpRollback(progress, status);
    status = umi_data_server_commit(progress->server);
    if (status != UMI_STATUS_OK) {
        progress->recovery_required = 1;
        if (umi_data_server_in_transaction(progress->server)) (void)LpRollback(progress, status);
    }
    return status;
}
