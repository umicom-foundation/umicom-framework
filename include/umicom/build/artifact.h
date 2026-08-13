/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/artifact.h
 *
 * PURPOSE:
 *   Retain bounded artifact metadata produced by build graph nodes.  This is
 *   the stable input for future package, release and deployment gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_ARTIFACT_H
#define UMICOM_BUILD_ARTIFACT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_ARTIFACT_API_VERSION 1U

typedef struct UmiBuildArtifactSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    uint64_t operation_id;
    char node_id[UMI_BUILD_ID_CAPACITY];
    char artifact_id[UMI_BUILD_ID_CAPACITY];
    char path[UMI_BUILD_PATH_CAPACITY];
    char kind[UMI_BUILD_NAME_CAPACITY];
    uint64_t size_bytes;
    uint64_t revision;
} UmiBuildArtifactSnapshot;

typedef struct UmiBuildArtifactIndex UmiBuildArtifactIndex;

UmiStatus umi_build_artifact_index_create(UmiBuildArtifactIndex **out_index);
void umi_build_artifact_index_destroy(UmiBuildArtifactIndex *index);
UmiStatus umi_build_artifact_index_upsert(
    UmiBuildArtifactIndex *index,
    const UmiBuildArtifactSnapshot *artifact);
UmiStatus umi_build_artifact_index_find(const UmiBuildArtifactIndex *index,
                                        const char *artifact_id,
                                        UmiBuildArtifactSnapshot *out_artifact);
UmiStatus umi_build_artifact_index_at(const UmiBuildArtifactIndex *index,
                                      size_t position,
                                      UmiBuildArtifactSnapshot *out_artifact);
size_t umi_build_artifact_index_count(const UmiBuildArtifactIndex *index);
uint64_t umi_build_artifact_index_revision(const UmiBuildArtifactIndex *index);

#ifdef __cplusplus
}
#endif
#endif
