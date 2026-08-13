/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/artifact.c
 *
 * PURPOSE:
 *   Implement deterministic artifact/result metadata retention.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/artifact.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildArtifactIndex {
    UmiBuildArtifactSnapshot items[UMI_BUILD_ARTIFACT_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiBuildArtifactIndex *index,
                         const char *artifact_id)
{
    size_t position;
    if (index == NULL || artifact_id == NULL) return SIZE_MAX;
    for (position = 0U; position < index->count; ++position) {
        if (strcmp(index->items[position].artifact_id, artifact_id) == 0)
            return position;
    }
    return SIZE_MAX;
}

UmiStatus umi_build_artifact_index_create(UmiBuildArtifactIndex **out_index)
{
    UmiBuildArtifactIndex *index;
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    index = (UmiBuildArtifactIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_build_artifact_index_destroy(UmiBuildArtifactIndex *index)
{
    free(index);
}

UmiStatus umi_build_artifact_index_upsert(
    UmiBuildArtifactIndex *index,
    const UmiBuildArtifactSnapshot *artifact)
{
    size_t position;
    if (index == NULL || artifact == NULL || artifact->artifact_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    position = find_index(index, artifact->artifact_id);
    if (position == SIZE_MAX) {
        if (index->count >= UMI_BUILD_ARTIFACT_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        position = index->count++;
    }
    index->items[position] = *artifact;
    index->items[position].structure_size =
        (uint32_t)sizeof(index->items[position]);
    index->items[position].api_version = UMI_BUILD_ARTIFACT_API_VERSION;
    index->items[position].node_id[UMI_BUILD_ID_CAPACITY - 1U] = '\0';
    index->items[position].artifact_id[UMI_BUILD_ID_CAPACITY - 1U] = '\0';
    index->items[position].path[UMI_BUILD_PATH_CAPACITY - 1U] = '\0';
    index->items[position].kind[UMI_BUILD_NAME_CAPACITY - 1U] = '\0';
    index->revision += 1U;
    index->items[position].revision = index->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_index_find(const UmiBuildArtifactIndex *index,
                                        const char *artifact_id,
                                        UmiBuildArtifactSnapshot *out_artifact)
{
    size_t position;
    if (index == NULL || artifact_id == NULL || out_artifact == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    position = find_index(index, artifact_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_artifact = index->items[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_index_at(const UmiBuildArtifactIndex *index,
                                      size_t position,
                                      UmiBuildArtifactSnapshot *out_artifact)
{
    if (index == NULL || out_artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;
    *out_artifact = index->items[position];
    return UMI_STATUS_OK;
}

size_t umi_build_artifact_index_count(const UmiBuildArtifactIndex *index)
{
    return index != NULL ? index->count : 0U;
}

uint64_t umi_build_artifact_index_revision(const UmiBuildArtifactIndex *index)
{
    return index != NULL ? index->revision : 0U;
}
