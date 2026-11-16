/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/bundle_selection.c
 *
 * PURPOSE:
 *   Select pinned GGUF companions through the shared bundle validation and staged publication boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/bundle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool shard_decimal(const char *digits, size_t *value)
{
    size_t number = 0U;
    for (size_t i = 0U; i < 5U; ++i) {
        if (digits[i] < '0' || digits[i] > '9') return false;
        number = number * 10U + (size_t)(digits[i] - '0');
    }
    *value = number;
    return true;
}
/* The canonical catalogue validator has already bounded and terminated the
 * filename. Recognize only the documented fixed-width suffix; do not guess a
 * model's architecture or treat an unrelated trailing number as a shard. */
static UmiStatus shard_suffix(const char *filename, size_t *prefix_length,
    size_t *total)
{
    size_t length = strlen(filename);
    *prefix_length = 0U;
    *total = 1U;
    if (length < 20U || filename[length - 20U] != '-' ||
        memcmp(filename + length - 14U, "-of-", 4U) != 0 ||
        strcmp(filename + length - 5U, ".gguf") != 0)
        return UMI_STATUS_OK;
    size_t prefix = length - 20U;
    size_t part, parts;
    if (prefix == 0U || filename[prefix - 1U] == '/' ||
        !shard_decimal(filename + length - 19U, &part) ||
        !shard_decimal(filename + length - 10U, &parts) ||
        part == 0U || parts == 0U || part > parts)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (parts > UMI_MODEL_BUNDLE_MAX_FILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    *prefix_length = prefix;
    *total = parts;
    return UMI_STATUS_OK;
}
UmiStatus UmiModelBundleSelectGguf(const UmiModelBundlePlan *catalogue,
    size_t selected_index, UmiModelBundlePlan *out_plan)
{
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelBundlePlanValidate(catalogue);
    if (status != UMI_STATUS_OK) return status;
    if (selected_index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    const UmiModelBundleFile *selected = &catalogue->files[selected_index];
    if (selected->format != UMI_MODEL_ARTIFACT_GGUF) return UMI_STATUS_NOT_IMPLEMENTED;
    size_t prefix, count;
    status = shard_suffix(selected->filename, &prefix, &count);
    if (status != UMI_STATUS_OK) return status;
    if (prefix == 0U)
        return UmiModelBundlePlanCreate(catalogue->repository, catalogue->revision,
            selected, 1U, out_plan);

    /* Keep owned staging separate from the caller's catalogue so replacing a
     * catalogue in place cannot erase later companions. Canonical construction
     * owns final validation and atomic publication; no second validator lives
     * here. The 128-record bound also limits the search and allocation sizes. */
    UmiModelBundleFile *files = calloc(count, sizeof(*files));
    if (files == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t part = 1U; part <= count; ++part) {
        char expected[sizeof(selected->filename)];
        int written = snprintf(expected, sizeof(expected), "%.*s-%05zu-of-%05zu.gguf",
            (int)prefix, selected->filename, part, count);
        if (written < 0 || (size_t)written >= sizeof(expected)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        size_t found = 0U;
        while (found < catalogue->count &&
            strcmp(catalogue->files[found].filename, expected) != 0) ++found;
        if (found == catalogue->count) {
            status = UMI_STATUS_NOT_FOUND;
            break;
        }
        files[part - 1U] = catalogue->files[found];
    }
    if (status == UMI_STATUS_OK)
        status = UmiModelBundlePlanCreate(catalogue->repository, catalogue->revision,
            files, count, out_plan);
    free(files);
    return status;
}
