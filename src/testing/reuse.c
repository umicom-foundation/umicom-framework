/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/reuse.c
 *
 * PURPOSE:
 *   Reuse only matching passing receipts and keep unsuccessful tests on the retry path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/reuse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Receipt {
    char name[UMI_TEST_NAME_CAPACITY];
    char digest[65];
    UmiTestState outcome;
} Receipt;
struct UmiTestReuseState {
    char context[65];
    Receipt *items;
    size_t count;
};
static const char prefix[] = "UMICOM-TEST-REUSE 1 ";

static bool Digest(const char *text, size_t size)
{
    if (size != 64U) return false;
    for (size_t i = 0U; i < size; ++i)
        if (!((text[i] >= '0' && text[i] <= '9') ||
              (text[i] >= 'a' && text[i] <= 'f'))) return false;
    return true;
}
static int Compare(const void *a, const void *b)
{
    return strcmp(((const Receipt *)a)->name, ((const Receipt *)b)->name);
}
static Receipt *Find(const UmiTestReuseState *state, const char *name)
{
    size_t first = 0U, last = state->count;
    while (first < last) {
        size_t middle = first + (last - first) / 2U;
        int order = strcmp(name, state->items[middle].name);
        if (order == 0) return &state->items[middle];
        if (order < 0) last = middle;
        else first = middle + 1U;
    }
    return NULL;
}
void UmiTestReuseDestroy(UmiTestReuseState *state)
{
    if (state != NULL) { free(state->items); free(state); }
}
/* Keep the canonical LF parser as the sole structural validator. The public
 * boundary below accepts native Windows text without weakening field checks. */
static UmiStatus ImportLf(const char *text, size_t length, UmiTestReuseState **out)
{
    const size_t head = sizeof(prefix) - 1U + 64U;
    if (text == NULL || out == NULL || *out != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_TEST_REUSE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < head + 1U || memcmp(text, prefix, sizeof(prefix) - 1U) != 0 ||
        text[head] != '\n' || !Digest(text + sizeof(prefix) - 1U, 64U) ||
        text[length - 1U] != '\n' || memchr(text, 0, length) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = 0U;
    for (size_t i = head + 1U; i < length; ++i) if (text[i] == '\n') ++count;
    if (count > UMI_CTEST_COLLECTION_MAX_TESTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiTestReuseState *state = calloc(1U, sizeof(*state));
    if (state == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    state->items = count == 0U ? NULL : calloc(count, sizeof(*state->items));
    if (count != 0U && state->items == NULL) {
        UmiTestReuseDestroy(state); return UMI_STATUS_OUT_OF_MEMORY;
    }
    state->count = count;
    memcpy(state->context, text + sizeof(prefix) - 1U, 64U);
    const char *cursor = text + head + 1U, *end = text + length;
    for (size_t i = 0U; i < count; ++i) {
        const char *line = memchr(cursor, '\n', (size_t)(end - cursor));
        const char *tab = line == NULL ? NULL : memchr(cursor, '\t', (size_t)(line - cursor));
        if (tab == NULL || tab == cursor || (size_t)(tab - cursor) >= UMI_TEST_NAME_CAPACITY ||
            line - tab != 67 || tab[65] != '\t' || tab[66] < '0' || tab[66] > '6' ||
            !Digest(tab + 1, 64U)) {
            UmiTestReuseDestroy(state); return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (const char *p = cursor; p < tab; ++p)
            if ((unsigned char)*p < 32U || (unsigned char)*p > 126U) {
                UmiTestReuseDestroy(state); return UMI_STATUS_INVALID_ARGUMENT;
            }
        Receipt *item = &state->items[i];
        memcpy(item->name, cursor, (size_t)(tab - cursor));
        memcpy(item->digest, tab + 1, 64U);
        item->outcome = (UmiTestState)(tab[66] - '0');
        cursor = line + 1;
    }
    if (count > 1U) qsort(state->items, count, sizeof(*state->items), Compare);
    for (size_t i = 1U; i < count; ++i)
        if (strcmp(state->items[i - 1U].name, state->items[i].name) == 0) {
            UmiTestReuseDestroy(state); return UMI_STATUS_INVALID_ARGUMENT;
        }
    *out = state;
    return UMI_STATUS_OK;
}
/* CMake and text editors may emit CRLF on Windows. Remove only CR immediately
 * followed by LF; a bare CR, including one inside a digest or name, is invalid.
 * Allocate a bounded private copy so failures never alter caller input/output.
 * Exports stay canonical LF regardless of the imported file's line endings. */
UmiStatus UmiTestReuseImport(const char *text, size_t length, UmiTestReuseState **out)
{
    if (text == NULL || out == NULL || *out != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_TEST_REUSE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\r', length) == NULL) return ImportLf(text, length, out);
    char *normalized = malloc(length);
    if (normalized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t used = 0U;
    for (size_t i = 0U; i < length; ++i) {
        if (text[i] == '\r') {
            if (i + 1U == length || text[i + 1U] != '\n') {
                free(normalized);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
        } else {
            normalized[used++] = text[i];
        }
    }
    UmiStatus status = ImportLf(normalized, used, out);
    free(normalized);
    return status;
}
size_t UmiTestReuseCount(const UmiTestReuseState *state) { return state == NULL ? 0U : state->count; }
const char *UmiTestReuseName(const UmiTestReuseState *state, size_t index)
{
    return state == NULL || index >= state->count ? NULL : state->items[index].name;
}
UmiTestState UmiTestReuseOutcome(const UmiTestReuseState *state, size_t index)
{
    return state == NULL || index >= state->count ? UMI_TEST_STATE_NOT_RUN : state->items[index].outcome;
}
UmiStatus UmiTestReusePrepare(const UmiTestReuseState *current,
    const UmiTestReuseState *previous, UmiTestReuseState **out)
{
    if (current == NULL || out == NULL || *out != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestReuseState *next = calloc(1U, sizeof(*next));
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(next->context, current->context, sizeof(next->context));
    next->count = current->count;
    if (next->count != 0U) {
        next->items = malloc(next->count * sizeof(*next->items));
        if (next->items == NULL) { free(next); return UMI_STATUS_OUT_OF_MEMORY; }
        memcpy(next->items, current->items, next->count * sizeof(*next->items));
    }
    bool same = previous != NULL && strcmp(current->context, previous->context) == 0;
    for (size_t i = 0U; i < next->count; ++i) {
        Receipt *item = &next->items[i];
        const Receipt *old = same ? Find(previous, item->name) : NULL;
        item->outcome = current->items[i].outcome == UMI_TEST_STATE_NOT_RUN && old != NULL && old->outcome == UMI_TEST_STATE_PASSED &&
            strcmp(old->digest, item->digest) == 0 ? UMI_TEST_STATE_PASSED : UMI_TEST_STATE_NOT_RUN;
    }
    *out = next;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestReuseRecord(UmiTestReuseState *state, const char *xml, size_t length)
{
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestReport *report = NULL;
    UmiStatus status = UmiCtestReportImport(xml, length, &report);
    if (status != UMI_STATUS_OK) return status;
    UmiCtestReportSummary summary;
    status = UmiCtestReportReadSummary(report, &summary);
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < summary.tests; ++i) {
            const UmiCtestReportEntry *entry = UmiCtestReportEntryAt(report, i);
            Receipt *item = Find(state, entry->name);
            if (item != NULL) item->outcome = entry->disabled ? UMI_TEST_STATE_SKIPPED : entry->result.state;
        }
    }
    UmiCtestReportDestroy(report);
    return status;
}
UmiStatus UmiTestReuseExport(const UmiTestReuseState *state, char *text,
    size_t capacity, size_t *required)
{
    if (state == NULL || required == NULL || (text == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t needed = sizeof(prefix) - 1U + 64U + 1U + 1U;
    for (size_t i = 0U; i < state->count; ++i) needed += strlen(state->items[i].name) + 68U;
    if (text == NULL) { *required = needed; return UMI_STATUS_OK; }
    if (capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t used = (size_t)snprintf(text, capacity, "%s%s\n", prefix, state->context);
    for (size_t i = 0U; i < state->count; ++i) {
        const Receipt *item = &state->items[i];
        used += (size_t)snprintf(text + used, capacity - used, "%s\t%s\t%d\n",
            item->name, item->digest, (int)item->outcome);
    }
    *required = needed;
    return UMI_STATUS_OK;
}
