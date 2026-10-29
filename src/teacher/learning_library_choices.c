/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_library_choices.c
 *
 * PURPOSE:
 *   Own bounded library choices independently of shelves, widgets and filesystem handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_library_choices_internal.h"
#include "learning_pack_internal.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>

static int ChoiceText(const char *text, size_t capacity)
{
    size_t length = 0U;
    while (length < capacity && text[length] != '\0') ++length;
    if (length == 0U || length == capacity ||
        !umi_document_utf8_validate((const unsigned char *)text, length, NULL)) return 0;
    for (size_t i = 0U; i < length; ++i)
        if ((unsigned char)text[i] < 0x20U || (unsigned char)text[i] == 0x7fU) return 0;
    return 1;
}
static int ChoiceAlpha(unsigned char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9');
}
static int ChoiceToken(const char *text, size_t capacity, int locale)
{
    if (!ChoiceText(text, capacity) || !ChoiceAlpha((unsigned char)text[0])) return 0;
    int segment = 1;
    for (size_t i = 1U; text[i] != '\0'; ++i) {
        if (ChoiceAlpha((unsigned char)text[i])) segment = 1;
        else if (text[i] == '-' && (!locale || segment)) segment = 0;
        else if (!locale && (text[i] == '.' || text[i] == '_')) segment = 1;
        else return 0;
    }
    return !locale || segment;
}
UmiStatus LearningLibraryChoiceAdd(UmiLearningLibraryChoices *choices,
    const UmiLearningLibraryChoice *entry)
{
    if (choices == NULL || entry == NULL ||
        !ChoiceText(entry->root, sizeof(entry->root)) ||
        !ChoiceText(entry->manifest, sizeof(entry->manifest)) ||
        !ChoiceToken(entry->pack_id, sizeof(entry->pack_id), 0) ||
        !ChoiceToken(entry->product, sizeof(entry->product), 0) ||
        !ChoiceToken(entry->locale, sizeof(entry->locale), 1) ||
        entry->revision == 0U || entry->revision > INT64_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiRootedFileValidatePath(entry->root, entry->manifest);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < choices->count; ++i)
        if (strcmp(choices->entries[i].pack_id, entry->pack_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (choices->count == UMI_LEARNING_LIBRARY_CHOICES_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningLibraryChoice copy = *entry;
    status = umi_path_normalise(entry->root, copy.root, sizeof(copy.root));
    if (status != UMI_STATUS_OK) return status;
    choices->entries[choices->count++] = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningLibraryChoicesCreate(UmiLearningLibraryChoices **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = calloc(1U, sizeof(**out));
    return *out == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
}
void UmiLearningLibraryChoicesDestroy(UmiLearningLibraryChoices *choices) { free(choices); }
UmiStatus UmiLearningLibraryChoicesClone(const UmiLearningLibraryChoices *choices,
    UmiLearningLibraryChoices **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (choices == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningLibraryChoices *copy = malloc(sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *copy = *choices;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningLibraryChoicesRemember(UmiLearningLibraryChoices *choices,
    const UmiLearningPack *pack, const char *manifest)
{
    if (choices == NULL || pack == NULL || manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_TEACHER_PATH_CAPACITY && manifest[length] != '\0') ++length;
    if (length == UMI_TEACHER_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLearningLibraryChoice entry = {0};
    memcpy(entry.root, pack->root, sizeof(entry.root));
    memcpy(entry.manifest, manifest, length + 1U);
    memcpy(entry.pack_id, pack->info.id, sizeof(entry.pack_id));
    memcpy(entry.product, pack->info.product, sizeof(entry.product));
    memcpy(entry.locale, pack->info.locale, sizeof(entry.locale));
    entry.revision = pack->info.revision;
    return LearningLibraryChoiceAdd(choices, &entry);
}
UmiStatus UmiLearningLibraryChoicesForget(UmiLearningLibraryChoices *choices, const char *id)
{
    if (choices == NULL || id == NULL || !ChoiceToken(id, UMI_TEACHER_ID_CAPACITY, 0))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < choices->count; ++i) {
        if (strcmp(choices->entries[i].pack_id, id) != 0) continue;
        /* Forgetting changes only this owner. It does not delete a folder,
         * remove an admitted shelf pack or invalidate an independent reader. */
        memmove(&choices->entries[i], &choices->entries[i + 1U],
            (choices->count - i - 1U) * sizeof(choices->entries[0]));
        --choices->count;
        memset(&choices->entries[choices->count], 0, sizeof(choices->entries[0]));
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
size_t UmiLearningLibraryChoicesCount(const UmiLearningLibraryChoices *choices)
{ return choices == NULL ? 0U : choices->count; }
UmiStatus UmiLearningLibraryChoicesAt(const UmiLearningLibraryChoices *choices, size_t index,
    UmiLearningLibraryChoice *out)
{
    if (choices == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= choices->count) return UMI_STATUS_NOT_FOUND;
    *out = choices->entries[index];
    return UMI_STATUS_OK;
}
