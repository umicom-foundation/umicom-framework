/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_library_choices_files.c
 *
 * PURPOSE:
 *   Persist chosen library locators and re-admit manifests without cached execution trust.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_library_choices_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiLearningLibraryChoicesLoad(const char *root, const char *relative,
    UmiLearningLibraryChoices **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiRootedFileRead(root, relative, UMI_LEARNING_LIBRARY_CHOICES_BYTES,
        &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiLearningLibraryChoicesDecode(bytes, length, out);
    UmiRootedFileFree(bytes);
    return status;
}
UmiStatus UmiLearningLibraryChoicesSaveNew(const UmiLearningLibraryChoices *choices,
    const char *root, const char *relative, UmiRootedFileCreation *report)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *report = (UmiRootedFileCreation){0};
    char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = choices == NULL ? UMI_STATUS_INVALID_ARGUMENT :
        UmiRootedFileValidatePath(root, relative);
    if (status == UMI_STATUS_OK) {
        bytes = malloc(UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U);
        if (bytes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = UmiLearningLibraryChoicesEncode(choices, bytes,
        UMI_LEARNING_LIBRARY_CHOICES_BYTES + 1U, &length);
    if (status == UMI_STATUS_OK) status = UmiRootedFileCreate(root, relative, bytes, length, report);
    report->status = status;
    free(bytes);
    return status;
}
static int ChoiceLocaleEqual(const char *left, const char *right)
{
    for (size_t i = 0U;; ++i) {
        unsigned char a = (unsigned char)left[i], b = (unsigned char)right[i];
        if (a >= 'A' && a <= 'Z') a = (unsigned char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (unsigned char)(b - 'A' + 'a');
        if (a != b) return 0;
        if (a == 0U) return 1;
    }
}
UmiStatus UmiLearningLibraryChoicesReopen(const UmiLearningLibraryChoices *choices,
    size_t index, UmiVersion host, UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (choices == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= choices->count) return UMI_STATUS_NOT_FOUND;
    const UmiLearningLibraryChoice *entry = &choices->entries[index];
    UmiLearningPack *pack = NULL;
    UmiStatus status = UmiLearningPackLoad(entry->root, entry->manifest, host, &pack);
    UmiLearningPackInfo info;
    if (status == UMI_STATUS_OK) status = UmiLearningPackCopyInfo(pack, &info);
    /* A persisted path is a locator, never a cached admission decision. The
     * host must explicitly review different identities and revisions. */
    if (status == UMI_STATUS_OK && (strcmp(info.id, entry->pack_id) != 0 ||
        strcmp(info.product, entry->product) != 0 ||
        !ChoiceLocaleEqual(info.locale, entry->locale) || info.revision != entry->revision))
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) { UmiLearningPackDestroy(pack); return status; }
    *out = pack;
    return UMI_STATUS_OK;
}
