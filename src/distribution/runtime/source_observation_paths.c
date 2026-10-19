/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation_paths.c
 * Purpose: Collect exact file identities before any source observation performs I/O.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "source_observation_internal.h"
#include "impact_internal.h"
#include <stdlib.h>
#include <string.h>

/* Preserve path identity across hosts without resolving aliases or consulting
 * the current directory. The native reader applies its own platform rules too. */
static UmiStatus Normalise(const char *input, char **out)
{
    if (input == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0U;
    while (n <= UMI_RELEASE_IMPACT_PATH_LIMIT && input[n] != '\0') ++n;
    if (n == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (n > UMI_RELEASE_IMPACT_PATH_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *copy = malloc(n + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < n; ++i) {
        unsigned char c = (unsigned char)input[i];
        if (c < 32U || c == 127U) { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
        copy[i] = c == '\\' ? '/' : (char)c;
    }
    copy[n] = '\0';
    bool drive = n >= 3U && ((copy[0] >= 'A' && copy[0] <= 'Z') ||
        (copy[0] >= 'a' && copy[0] <= 'z')) && copy[1] == ':' && copy[2] == '/';
    if (!drive && copy[0] != '/') { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
    size_t start = drive ? 3U : 1U;
    for (size_t i = start; i <= n; ++i) {
        if (copy[i] == ':') { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
        if (copy[i] != '/' && copy[i] != '\0') continue;
        size_t length = i - start;
        if (length == 0U || (length == 1U && copy[start] == '.') ||
            (length == 2U && copy[start] == '.' && copy[start + 1U] == '.')) {
            free(copy); return UMI_STATUS_INVALID_ARGUMENT;
        }
        start = i + 1U;
    }
    *out = copy;
    return UMI_STATUS_OK;
}
static UmiStatus HeaderPath(void *context, const char *key, const char *value)
{
    char **path = context;
    if (strcmp(key, "path") != 0 || *path != NULL) return UMI_STATUS_PARSE_ERROR;
    return Normalise(value, path);
}
static int ComparePaths(const void *left, const void *right)
{
    const UmiReleaseSourceItem *a = left, *b = right;
    return strcmp(a->path, b->path);
}
UmiStatus UmiSourceObservationCollect(const UmiReleaseInventory *inventory,
    UmiReleaseSourceObservation *observation, const char **problemIdentity)
{
    size_t records = UmiReleaseInventoryCount(inventory), pathBytes = 0U;
    observation->items = calloc(records == 0U ? 1U : records, sizeof(*observation->items));
    if (observation->items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < records; ++i) {
        const UmiReleaseInventoryRecord *record = UmiReleaseInventoryAt(inventory, i);
        char *path = NULL;
        UmiStatus status = UMI_STATUS_OK;
        if (record->kind != UMI_RELEASE_INVENTORY_SOURCE && record->kind != UMI_RELEASE_INVENTORY_HEADER) continue;
        *problemIdentity = record->identity;
        if (record->kind == UMI_RELEASE_INVENTORY_SOURCE) {
            if (strcmp(record->state, "present") != 0) return UMI_STATUS_UNAVAILABLE;
            status = Normalise(record->identity, &path);
        } else if (record->kind == UMI_RELEASE_INVENTORY_HEADER) {
            bool present = false;
            status = UmiImpactMetadataRead(record->detail, HeaderPath, &path, &present);
            if (status == UMI_STATUS_OK && (!present || path == NULL)) status = UMI_STATUS_UNAVAILABLE;
        } else continue;
        if (status != UMI_STATUS_OK) { free(path); return status; }
        size_t bytes = strlen(path) + 1U;
        if (bytes > UMI_RELEASE_SOURCE_PATH_BYTES_LIMIT - pathBytes) {
            free(path); return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        pathBytes += bytes;
        observation->items[observation->count++].path = path;
        *problemIdentity = NULL;
    }
    if (observation->count == 0U) return UMI_STATUS_UNAVAILABLE;
    qsort(observation->items, observation->count, sizeof(*observation->items), ComparePaths);
    size_t kept = 0U;
    for (size_t i = 0U; i < observation->count; ++i) {
        if (kept != 0U && strcmp(observation->items[kept - 1U].path, observation->items[i].path) == 0)
            free((void *)observation->items[i].path);
        else observation->items[kept++] = observation->items[i];
    }
    observation->count = kept;
    return UMI_STATUS_OK;
}
