/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_artifact_manifest.c
 *
 * PURPOSE:
 *   Verify provenance validation, import and primary deployable selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/artifact_manifest.h"

int main(void)
{
    UmiBuildArtifactSnapshot artifact = {0};
    UmiBuildArtifactManifest first;
    UmiBuildArtifactManifest second;
    UmiBuildArtifactManifest selected;
    UmiBuildArtifactManifestRegistry *registry = NULL;
    UmiBuildArtifactManifestRegistrySnapshot snapshot;
    char message[256];

    artifact.structure_size = (uint32_t)sizeof(artifact);
    artifact.api_version = UMI_BUILD_ARTIFACT_API_VERSION;
    artifact.operation_id = 55U;
    strcpy(artifact.node_id, "package");
    strcpy(artifact.artifact_id, "studio.zip");
    strcpy(artifact.path, "dist/studio.zip");
    strcpy(artifact.kind, "archive");
    artifact.size_bytes = 4096U;
    artifact.revision = 9U;
    assert(umi_build_artifact_manifest_import(&artifact, &first) ==
           UMI_STATUS_OK);
    strcpy(first.checksum_algorithm, "SHA-256");
    strcpy(first.checksum, "abc123");
    first.flags |= UMI_BUILD_ARTIFACT_PRIMARY |
                   UMI_BUILD_ARTIFACT_DEPLOYABLE |
                   UMI_BUILD_ARTIFACT_REPRODUCIBLE |
                   UMI_BUILD_ARTIFACT_VERIFIED;
    assert(umi_build_artifact_manifest_validate(
               &first, message, sizeof(message)) == UMI_STATUS_OK);

    umi_build_artifact_manifest_init(
        &second, "symbols.zip", "dist/symbols.zip", "debug-symbols");
    second.flags |= UMI_BUILD_ARTIFACT_DEPLOYABLE;
    assert(umi_build_artifact_manifest_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_build_artifact_manifest_registry_upsert(registry, &second) ==
           UMI_STATUS_OK);
    assert(umi_build_artifact_manifest_registry_upsert(registry, &first) ==
           UMI_STATUS_OK);
    assert(umi_build_artifact_manifest_registry_primary_deployable(
               registry, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.artifact_id, "studio.zip") == 0);
    assert(umi_build_artifact_manifest_registry_snapshot(
               registry, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.artifact_count == 2U);
    assert(snapshot.deployable_count == 2U);
    assert(snapshot.verified_count == 1U);

    first.flags |= UMI_BUILD_ARTIFACT_SIGNED;
    first.flags &=
        ~(UmiBuildArtifactManifestFlags)UMI_BUILD_ARTIFACT_VERIFIED;
    assert(umi_build_artifact_manifest_validate(
               &first, message, sizeof(message)) ==
           UMI_STATUS_INVALID_ARGUMENT);

    umi_build_artifact_manifest_registry_destroy(registry);
    return 0;
}
