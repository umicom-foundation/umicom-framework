/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack_changes.c
 *
 * PURPOSE:
 *   Compare copied manifest metadata without reopening or trusting resource files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_pack_internal.h"
#include "umicom/teacher/learning_pack_changes.h"
#include <stdlib.h>
#include <string.h>

struct UmiLearningPackChanges {
    UmiLearningPackChangeSummary summary;
    UmiLearningResourceChange *entries;
};
static const UmiLearningPackResource *ChangeFind(const UmiLearningPack *pack, const char *id)
{
    for (size_t i = 0U; i < pack->info.resource_count; ++i)
        if (strcmp(pack->resources[i].id, id) == 0) return &pack->resources[i];
    return NULL;
}
static unsigned int ChangeFields(const UmiLearningPackResource *a, const UmiLearningPackResource *b)
{
    unsigned int fields = 0U;
    if (strcmp(a->title, b->title) != 0) fields |= UMI_LEARNING_CHANGE_TITLE;
    if (strcmp(a->topic, b->topic) != 0) fields |= UMI_LEARNING_CHANGE_TOPIC;
    if (a->kind != b->kind) fields |= UMI_LEARNING_CHANGE_KIND;
    if (strcmp(a->path, b->path) != 0) fields |= UMI_LEARNING_CHANGE_PATH;
    if (strcmp(a->sha256, b->sha256) != 0) fields |= UMI_LEARNING_CHANGE_DIGEST;
    if (strcmp(a->example_id, b->example_id) != 0) fields |= UMI_LEARNING_CHANGE_EXAMPLE;
    return fields;
}
UmiStatus UmiLearningPackCompare(const UmiLearningPack *previous,
    const UmiLearningPack *proposed, UmiLearningPackChanges **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (previous == NULL || proposed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackChanges *changes = calloc(1U, sizeof(*changes));
    if (changes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Count first to avoid reserving storage for every unchanged resource.
     * Inputs are immutable and remain owned by the caller during both passes. */
    for (size_t i = 0U; i < previous->info.resource_count; ++i) {
        const UmiLearningPackResource *a = &previous->resources[i];
        const UmiLearningPackResource *b = ChangeFind(proposed, a->id);
        if (b == NULL) ++changes->summary.removed;
        else {
            unsigned int fields = ChangeFields(a, b);
            if (fields == 0U) ++changes->summary.unchanged;
            else {
                ++changes->summary.modified;
                if ((fields & UMI_LEARNING_CHANGE_DIGEST) != 0U) ++changes->summary.changed_digests;
            }
        }
    }
    for (size_t i = 0U; i < proposed->info.resource_count; ++i)
        if (ChangeFind(previous, proposed->resources[i].id) == NULL) ++changes->summary.added;
    changes->summary.change_count = changes->summary.removed + changes->summary.modified + changes->summary.added;
    if (changes->summary.change_count != 0U) {
        changes->entries = calloc(changes->summary.change_count, sizeof(*changes->entries));
        if (changes->entries == NULL) { free(changes); return UMI_STATUS_OUT_OF_MEMORY; }
    }
    size_t count = 0U;
    for (size_t i = 0U; i < previous->info.resource_count; ++i) {
        const UmiLearningPackResource *a = &previous->resources[i];
        const UmiLearningPackResource *b = ChangeFind(proposed, a->id);
        unsigned int fields = b == NULL ? 0U : ChangeFields(a, b);
        if (b == NULL || fields != 0U) {
            UmiLearningResourceChange *entry = &changes->entries[count++];
            entry->kind = b == NULL ? UMI_LEARNING_RESOURCE_REMOVED : UMI_LEARNING_RESOURCE_MODIFIED;
            entry->fields = fields;
            entry->previous = *a;
            if (b != NULL) entry->proposed = *b;
        }
    }
    for (size_t i = 0U; i < proposed->info.resource_count; ++i) {
        const UmiLearningPackResource *b = &proposed->resources[i];
        if (ChangeFind(previous, b->id) == NULL) {
            UmiLearningResourceChange *entry = &changes->entries[count++];
            entry->kind = UMI_LEARNING_RESOURCE_ADDED;
            entry->proposed = *b;
        }
    }
    *out = changes;
    return UMI_STATUS_OK;
}
void UmiLearningPackChangesDestroy(UmiLearningPackChanges *changes)
{
    if (changes == NULL) return;
    free(changes->entries);
    free(changes);
}
UmiStatus UmiLearningPackChangesCopySummary(const UmiLearningPackChanges *changes,
    UmiLearningPackChangeSummary *out)
{
    if (changes == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = changes->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningPackChangeAt(const UmiLearningPackChanges *changes, size_t index,
    UmiLearningResourceChange *out)
{
    if (changes == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= changes->summary.change_count) return UMI_STATUS_NOT_FOUND;
    *out = changes->entries[index];
    return UMI_STATUS_OK;
}
