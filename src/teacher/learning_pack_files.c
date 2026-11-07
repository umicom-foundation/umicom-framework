/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack_files.c
 *
 * PURPOSE:
 *   Read admitted learning resources through rooted storage and verify content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_pack_internal.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/base/text.h"
#include <string.h>

/* The former loader discarded its manifest route. Retained for review; the
 * replacement carries that route through shelf and reader ownership. */
#if 0
UmiStatus UmiLearningPackLoad(const char *root, const char *manifest_path,
    UmiVersion host, UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiRootedFileRead(root, manifest_path,
        UMI_LEARNING_PACK_MANIFEST_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackCreate(bytes, size, root, host, out);
    UmiRootedFileFree(bytes);
    return status;
}
#endif
UmiStatus UmiLearningPackLoad(const char *root, const char *manifest_path,
    UmiVersion host, UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    char captured[UMI_TEACHER_PATH_CAPACITY];
    UmiStatus status = manifest_path == NULL ? UMI_STATUS_INVALID_ARGUMENT :
        umi_text_copy(captured, sizeof(captured), manifest_path);
    if (status != UMI_STATUS_OK) return status;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiRootedFileRead(root, captured,
        UMI_LEARNING_PACK_MANIFEST_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackCreate(bytes, size, root, host, out);
    if (status == UMI_STATUS_OK) {
        /* Keep the admitted route with the immutable pack. Readers and shelves
         * clone it so remembering an older document cannot select a newer route. */
        memcpy((*out)->manifest_path, captured, strlen(captured) + 1U);
    }
    UmiRootedFileFree(bytes);
    return status;
}
UmiStatus UmiLearningPackCopyManifestPath(const UmiLearningPack *pack,
    char *destination, size_t capacity)
{
    if (pack == NULL || destination == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (pack->manifest_path[0] == '\0') return UMI_STATUS_NOT_FOUND;
    const size_t required = strlen(pack->manifest_path) + 1U;
    if (required > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, pack->manifest_path, required);
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningPackReadResource(const UmiLearningPack *pack, const char *id,
    size_t maximum_bytes, unsigned char **out_bytes, size_t *out_size)
{
    if (out_bytes != NULL) *out_bytes = NULL;
    if (out_size != NULL) *out_size = 0U;
    if (pack == NULL || id == NULL || out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackResource resource;
    UmiStatus status = UmiLearningPackFindResource(pack, id, &resource);
    if (status != UMI_STATUS_OK) return status;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiRootedFileRead(pack->root, resource.path, maximum_bytes, &bytes, &size);
    if (status == UMI_STATUS_OK) {
        char actual[UMI_SHA256_HEX_CAPACITY];
        status = UmiSha256Buffer(bytes, size, actual);
        if (status == UMI_STATUS_OK && strcmp(actual, resource.sha256) != 0)
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK) {
        UmiRootedFileFree(bytes);
        return status;
    }
    /* Publish the checked buffer itself. Returning just a filename would make
     * the frontend reopen different bytes after an intervening edit. */
    *out_bytes = bytes;
    *out_size = size;
    return UMI_STATUS_OK;
}
void UmiLearningPackFreeBytes(void *bytes)
{
    UmiRootedFileFree(bytes);
}
