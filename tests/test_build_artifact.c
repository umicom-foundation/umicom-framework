/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_artifact.c
 * PURPOSE: Verify build artifact insertion, update and lookup.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/artifact.h"

int main(void)
{
    UmiBuildArtifactIndex *index = NULL;
    UmiBuildArtifactSnapshot artifact = {0};
    UmiBuildArtifactSnapshot copy;
    assert(umi_build_artifact_index_create(&index) == UMI_STATUS_OK);
    strcpy(artifact.artifact_id, "studio.exe");
    strcpy(artifact.node_id, "build");
    strcpy(artifact.path, "build/bin/umicom-studio-ide.exe");
    strcpy(artifact.kind, "executable");
    artifact.operation_id = 7U;
    artifact.size_bytes = 100U;
    assert(umi_build_artifact_index_upsert(index, &artifact) == UMI_STATUS_OK);
    artifact.size_bytes = 200U;
    assert(umi_build_artifact_index_upsert(index, &artifact) == UMI_STATUS_OK);
    assert(umi_build_artifact_index_count(index) == 1U);
    assert(umi_build_artifact_index_find(index, "studio.exe", &copy) ==
           UMI_STATUS_OK);
    assert(copy.size_bytes == 200U);
    umi_build_artifact_index_destroy(index);
    return 0;
}
