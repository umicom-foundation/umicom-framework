/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack_copy.c
 *
 * PURPOSE:
 *   Copy immutable pack metadata without sharing ownership or reading files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_pack_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiLearningPackClone(const UmiLearningPack *source, UmiLearningPack **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPack *copy = calloc(1U, sizeof(*copy));
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy->root, source->root, sizeof(copy->root));
    copy->info = source->info;
    if (source->info.resource_count != 0U) {
        copy->resources = calloc(source->info.resource_count, sizeof(*copy->resources));
        if (copy->resources == NULL) { free(copy); return UMI_STATUS_OUT_OF_MEMORY; }
        memcpy(copy->resources, source->resources,
            source->info.resource_count * sizeof(*copy->resources));
    }
    *out = copy;
    return UMI_STATUS_OK;
}
