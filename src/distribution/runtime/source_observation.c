/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation.c
 * Purpose: Bind observations to exact inventory context and bounded source-file digests.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "source_observation_internal.h"
#include "umicom/platform/input_file.h"
#include <stdlib.h>
#include <string.h>

/* Length prefixes make identities unambiguous even when fields contain tabs,
 * newlines or strings that resemble record separators. Encoding is host-neutral. */
static UmiStatus Number(UmiSha256 *hash, uint64_t value)
{
    unsigned char bytes[8];
    for (size_t i = 0U; i < sizeof(bytes); ++i) {
        bytes[sizeof(bytes) - i - 1U] = (unsigned char)(value & 255U);
        value >>= 8U;
    }
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}
static UmiStatus Field(UmiSha256 *hash, const char *value)
{
    size_t n = strlen(value);
    UmiStatus status = Number(hash, (uint64_t)n);
    return status == UMI_STATUS_OK ? UmiSha256Update(hash, value, n) : status;
}
static UmiStatus Finish(UmiSha256 *hash, char output[65])
{
    unsigned char digest[32];
    UmiStatus status = UmiSha256Final(hash, digest);
    if (status == UMI_STATUS_OK) UmiSha256Hex(digest, output);
    return status;
}
/* Inventory identity now belongs to the inventory service, allowing plans and
 * observations to prove they describe the same configured input. The previous
 * encoder is retained for compatibility review of the byte-level fingerprint. */
#if 0
static UmiStatus InventoryDigest(const UmiReleaseInventory *inventory, char output[65])
{
    UmiSha256 hash;
    UmiSha256Init(&hash);
    const char *context[] = {"umicom-source-inventory-1",
        UmiReleaseInventoryProducer(inventory), UmiReleaseInventoryGeneration(inventory),
        UmiReleaseInventorySourceRoot(inventory), UmiReleaseInventoryBuildRoot(inventory),
        UmiReleaseInventoryConfiguration(inventory)};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(context) / sizeof(context[0]); ++i)
        status = Field(&hash, context[i]);
    size_t count = UmiReleaseInventoryCount(inventory);
    if (status == UMI_STATUS_OK) status = Number(&hash, (uint64_t)count);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        const UmiReleaseInventoryRecord *record = UmiReleaseInventoryAt(inventory, i);
        status = Number(&hash, (uint64_t)record->kind);
        const char *fields[] = {record->identity, record->owner, record->state, record->detail};
        for (size_t j = 0U; status == UMI_STATUS_OK && j < sizeof(fields) / sizeof(fields[0]); ++j)
            status = Field(&hash, fields[j]);
    }
    return status == UMI_STATUS_OK ? Finish(&hash, output) : status;
}
#endif
static UmiStatus InventoryDigest(const UmiReleaseInventory *inventory, char output[65])
{
    return UmiReleaseInventoryFingerprint(inventory, output);
}
static UmiStatus ReadFile(void *context, const char *path, size_t maximumBytes,
    size_t *outSize, char outSha256[65])
{
    (void)context;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, maximumBytes, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(bytes, size, outSha256);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK) *outSize = size;
    return status;
}
static bool ValidDigest(const char digest[65])
{
    for (size_t i = 0U; i < 64U; ++i)
        if (!((digest[i] >= '0' && digest[i] <= '9') || (digest[i] >= 'a' && digest[i] <= 'f')))
            return false;
    return digest[64] == '\0';
}
UmiStatus UmiReleaseSourceObserve(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceOptions *options, UmiReleaseSourceObservation **outObservation)
{
    if (inventory == NULL || outObservation == NULL || *outObservation != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(UmiReleaseInventoryProducer(inventory), "cmake") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseSourceOptions defaults = {
        .maximumFileBytes = UMI_RELEASE_SOURCE_FILE_LIMIT,
        .maximumTotalBytes = UMI_RELEASE_SOURCE_TOTAL_LIMIT
    };
    if (options == NULL) options = &defaults;
    if (options->maximumFileBytes == 0U || options->maximumFileBytes > UMI_RELEASE_SOURCE_FILE_LIMIT ||
        options->maximumTotalBytes == 0U || options->maximumTotalBytes > UMI_RELEASE_SOURCE_TOTAL_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (options->cancelled != NULL && options->cancelled(options->context)) return UMI_STATUS_CANCELLED;
    UmiReleaseSourceObservation *result = calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const char *problemIdentity = NULL;
    UmiStatus status = UmiSourceObservationCollect(inventory, result, &problemIdentity);
    if (status == UMI_STATUS_OK) status = InventoryDigest(inventory, result->inventoryDigest);
    UmiSha256 hash;
    UmiSha256Init(&hash);
    if (status == UMI_STATUS_OK) status = Field(&hash, "umicom-source-observation-1");
    if (status == UMI_STATUS_OK) status = Field(&hash, result->inventoryDigest);
    if (status == UMI_STATUS_OK) status = Number(&hash, (uint64_t)result->count);
    size_t total = 0U;
    UmiReleaseSourceReadFn read = options->read != NULL ? options->read : ReadFile;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < result->count; ++i) {
        if (options->cancelled != NULL && options->cancelled(options->context)) {
            status = UMI_STATUS_CANCELLED; break;
        }
        size_t limit = options->maximumTotalBytes - total;
        if (limit > options->maximumFileBytes) limit = options->maximumFileBytes;
        UmiReleaseSourceItem *item = &result->items[i];
        problemIdentity = item->path;
        status = read(options->context, item->path, limit, &item->bytes, item->sha256);
        if (status != UMI_STATUS_OK) break;
        if (item->bytes > limit) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        if (!ValidDigest(item->sha256)) { status = UMI_STATUS_PARSE_ERROR; break; }
        total += item->bytes;
        status = Field(&hash, item->path);
        if (status == UMI_STATUS_OK) status = Number(&hash, (uint64_t)item->bytes);
        if (status == UMI_STATUS_OK) status = Field(&hash, item->sha256);
        if (status == UMI_STATUS_OK) problemIdentity = NULL;
    }
    /* A cancellation noticed after the final read must not publish an apparently
     * complete observation to a host that has already abandoned the job. */
    if (status == UMI_STATUS_OK && options->cancelled != NULL && options->cancelled(options->context))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) status = Finish(&hash, result->fingerprint);
    if (status == UMI_STATUS_OK) *outObservation = result;
    else {
        if (options->diagnostic != NULL && problemIdentity != NULL)
            options->diagnostic(options->context, status, problemIdentity);
        UmiReleaseSourceObservationDestroy(result);
    }
    return status;
}
void UmiReleaseSourceObservationDestroy(UmiReleaseSourceObservation *observation)
{
    if (observation == NULL) return;
    for (size_t i = 0U; i < observation->count; ++i) free((void *)observation->items[i].path);
    free(observation->items);
    free(observation);
}
size_t UmiReleaseSourceObservationCount(const UmiReleaseSourceObservation *observation)
{
    return observation == NULL ? 0U : observation->count;
}
const UmiReleaseSourceItem *UmiReleaseSourceObservationAt(
    const UmiReleaseSourceObservation *observation, size_t index)
{
    return observation == NULL || index >= observation->count ? NULL : &observation->items[index];
}
const char *UmiReleaseSourceObservationFingerprint(const UmiReleaseSourceObservation *observation)
{
    return observation == NULL ? NULL : observation->fingerprint;
}
UmiStatus UmiReleaseSourceObservationCompare(const UmiReleaseSourceObservation *before,
    const UmiReleaseSourceObservation *after, UmiReleaseSourceComparison *outComparison)
{
    if (before == NULL || after == NULL || outComparison == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(before->inventoryDigest, after->inventoryDigest) != 0 || before->count != after->count)
        return UMI_STATUS_INVALID_STATE;
    UmiReleaseSourceComparison result = {before->count, 0U, true};
    for (size_t i = 0U; i < before->count; ++i) {
        const UmiReleaseSourceItem *a = &before->items[i], *b = &after->items[i];
        if (strcmp(a->path, b->path) != 0) return UMI_STATUS_INVALID_STATE;
        if (a->bytes != b->bytes || strcmp(a->sha256, b->sha256) != 0) ++result.changedFiles;
    }
    result.same = result.changedFiles == 0U;
    *outComparison = result;
    return UMI_STATUS_OK;
}

const char *UmiReleaseSourceObservationInventoryFingerprint(const UmiReleaseSourceObservation *observation)
{
    return observation == NULL ? NULL : observation->inventoryDigest;
}
