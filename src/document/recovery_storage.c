/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/recovery_storage.c
 * PURPOSE: Persist exclusive recovery records and publish bounded catalogues only after complete enumeration.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_storage.h"
#include "umicom/platform/directory_scan.h"
#include "umicom/platform/resource_location.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
struct UmiDocumentRecoveryCatalogue
{
    size_t count;
    UmiDocumentRecoveryEntry entries[UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT];
};
static UmiStatus RecoveryLeaf(const char *key, char *leaf, size_t capacity)
{
    if (key == NULL || strlen(key) != 32U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < 32U; ++i)
        if (!((key[i] >= '0' && key[i] <= '9') || (key[i] >= 'a' && key[i] <= 'f')))
            return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < 39U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(leaf, key, 32U);
    memcpy(leaf + 32U, ".draft", 7U);
    return UMI_STATUS_OK;
}
static UmiStatus RecoveryDirectorySyntax(const char *directory)
{
    if (directory == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate the supplied spelling before any join can normalize dot segments. */
    size_t length = strlen(directory);
    if (length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_PATH_CAPACITY - 32U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *separator = (directory[length - 1U] == '/'
#ifdef _WIN32
                             || directory[length - 1U] == '\\'
#endif
                             )
                                ? ""
                                : "/";
    char probe[UMI_PATH_CAPACITY];
    UmiStatus status = UMI_STATUS_OK;
    (void)snprintf(probe, sizeof(probe), "%s%srecovery-check", directory, separator);
    return status == UMI_STATUS_OK ? UmiOutputFileValidatePath(probe) : status;
}
UmiStatus UmiDocumentRecoveryDirectory(const char *application, const char *base, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(application);
    config.baseOverride = base;
    UmiApplicationPaths paths;
    UmiStatus status = UmiApplicationPathsResolve(&config, &paths);
    if (status == UMI_STATUS_OK)
        status = umi_path_copy(out, capacity, paths.recovery);
    return status;
}
UmiStatus UmiDocumentRecoverySave(const char *directory, const UmiDocumentRecoveryDraft *draft,
                                  const UmiCancellationToken *cancel)
{
    UmiDocumentRecoveryInfo info;
    UmiStatus status = UmiDocumentRecoveryDraftInspect(draft, &info);
    if (status == UMI_STATUS_OK)
        status = RecoveryDirectorySyntax(directory);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiFileInfo parent;
    status = umi_directory_stat(directory, &parent);
    if (status == UMI_STATUS_OK && parent.kind != UMI_FILE_KIND_DIRECTORY)
        status = UMI_STATUS_PERMISSION_DENIED;
    char leaf[39], path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = RecoveryLeaf(info.key, leaf, sizeof(leaf));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, leaf, path, sizeof(path));
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryDraftEncode(draft, &bytes, &size);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileCreate(path, &file);
    /* Once creation starts, finish/flush this bounded snapshot even when the
     * window closes. Cancellation before creation writes nothing; afterwards
     * report the actual write result instead of hiding a completed record. */
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileWrite(file, bytes, size);
    if (file != NULL)
    {
        UmiStatus close_status = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK)
            status = close_status;
    }
    UmiOutputFileDestroy(file);
    UmiDocumentRecoveryBytesFree(bytes);
    return status;
}
UmiStatus UmiDocumentRecoveryLoad(const char *directory, const char *key, const UmiCancellationToken *cancel,
                                  UmiDocumentRecoveryDraft **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    char leaf[39];
    UmiStatus status = RecoveryLeaf(key, leaf, sizeof(leaf));
    if (status == UMI_STATUS_OK)
        status = RecoveryDirectorySyntax(directory);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiDocumentRecoveryDraft *draft = NULL;
    status = UmiRootedFileRead(directory, leaf, UMI_DOCUMENT_RECOVERY_RECORD_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryDraftDecode(bytes, size, cancel, &draft);
    if (status == UMI_STATUS_OK)
    {
        UmiDocumentRecoveryInfo info;
        status = UmiDocumentRecoveryDraftInspect(draft, &info);
        if (status == UMI_STATUS_OK && strcmp(info.key, key) != 0)
            status = UMI_STATUS_INVALID_STATE;
    }
    UmiRootedFileFree(bytes);
    if (status == UMI_STATUS_OK)
        *out = draft;
    else
        UmiDocumentRecoveryDraftDestroy(draft);
    return status;
}
static UmiStatus RecoveryCollect(const UmiFileInfo *file, void *data)
{
    UmiDocumentRecoveryCatalogue *catalogue = data;
    if (strlen(file->name) != 38U || strcmp(file->name + 32U, ".draft") != 0)
        return UMI_STATUS_OK;
    char key[33], leaf[39];
    memcpy(key, file->name, 32U);
    key[32] = '\0';
    if (RecoveryLeaf(key, leaf, sizeof(leaf)) != UMI_STATUS_OK)
        return UMI_STATUS_OK;
    if (catalogue->count == UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentRecoveryEntry *entry = &catalogue->entries[catalogue->count++];
    memcpy(entry->key, key, sizeof(key));
    entry->file_bytes = file->size;
    entry->modified_nanoseconds = file->modified_nanoseconds;
    entry->availability = file->kind != UMI_FILE_KIND_REGULAR               ? UMI_STATUS_PERMISSION_DENIED
                          : file->size > UMI_DOCUMENT_RECOVERY_RECORD_LIMIT ? UMI_STATUS_CAPACITY_EXCEEDED
                                                                            : UMI_STATUS_OK;
    return UMI_STATUS_OK;
}
static int RecoveryCompare(const void *left, const void *right)
{
    const UmiDocumentRecoveryEntry *a = left, *b = right;
    if (a->modified_nanoseconds != b->modified_nanoseconds)
        return a->modified_nanoseconds > b->modified_nanoseconds ? -1 : 1;
    return strcmp(a->key, b->key);
}
UmiStatus UmiDocumentRecoveryList(const char *directory, const UmiCancellationToken *cancel,
                                  UmiDocumentRecoveryCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = RecoveryDirectorySyntax(directory);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiDocumentRecoveryCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiDirectoryScanShallow(directory, 4096U, RecoveryCollect, catalogue, cancel);
    if (status == UMI_STATUS_NOT_FOUND)
        status = UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
    {
        qsort(catalogue->entries, catalogue->count, sizeof(catalogue->entries[0]), RecoveryCompare);
        *out = catalogue;
    }
    else
        free(catalogue);
    return status;
}
size_t UmiDocumentRecoveryCatalogueCount(const UmiDocumentRecoveryCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiDocumentRecoveryCatalogueAt(const UmiDocumentRecoveryCatalogue *catalogue, size_t index,
                                         UmiDocumentRecoveryEntry *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->entries[index];
    return UMI_STATUS_OK;
}
void UmiDocumentRecoveryCatalogueDestroy(UmiDocumentRecoveryCatalogue *catalogue) { free(catalogue); }

UmiStatus UmiDocumentRecoverySaveWithinBudget(const char *directory, const UmiDocumentRecoveryDraft *draft,
                                              size_t maximum_records, uint64_t maximum_file_bytes,
                                              const UmiCancellationToken *cancel)
{
    if (maximum_records == 0U || maximum_file_bytes == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (maximum_records > UMI_DOCUMENT_RECOVERY_CATALOGUE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentRecoveryInfo info;
    UmiStatus status = UmiDocumentRecoveryDraftInspect(draft, &info);
    UmiDocumentRecoveryCatalogue *catalogue = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryList(directory, cancel, &catalogue);
    uint64_t remaining = maximum_file_bytes;
    if (status == UMI_STATUS_OK && catalogue->count >= maximum_records)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        const UmiDocumentRecoveryEntry *entry = &catalogue->entries[i];
        if (strcmp(entry->key, info.key) == 0)
            status = UMI_STATUS_ALREADY_EXISTS;
        else if (entry->file_bytes > remaining)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            remaining -= entry->file_bytes;
    }
    unsigned char *encoded = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoveryDraftEncode(draft, &encoded, &bytes);
    if (status == UMI_STATUS_OK && (uint64_t)bytes > remaining)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentRecoveryBytesFree(encoded);
    UmiDocumentRecoveryCatalogueDestroy(catalogue);
    /* Reuse exclusive creation and its flush/close contract after admission.
     * A failed check or write retains every earlier recovery record. */
    if (status == UMI_STATUS_OK)
        status = UmiDocumentRecoverySave(directory, draft, cancel);
    return status;
}
