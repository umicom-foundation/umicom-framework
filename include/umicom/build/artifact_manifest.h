/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/artifact_manifest.h
 *
 * PURPOSE:
 *   Enrich build artifacts with deployable provenance, checksum, target,
 *   platform, architecture, reproducibility and verification evidence.
 *
 * ARCHITECTURE:
 *   UmiBuildArtifactIndex remains the compact build-output index. This module
 *   adds release/deployment evidence without changing that established ABI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_ARTIFACT_MANIFEST_H
#define UMICOM_BUILD_ARTIFACT_MANIFEST_H

#include "umicom/build/artifact.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_ARTIFACT_MANIFEST_API_VERSION 1U
#define UMI_BUILD_ARTIFACT_MANIFEST_CAPACITY 64U
#define UMI_BUILD_CHECKSUM_CAPACITY 192U
#define UMI_BUILD_CONTENT_TYPE_CAPACITY 128U
#define UMI_BUILD_PLATFORM_CAPACITY 96U

typedef uint32_t UmiBuildArtifactManifestFlags;
enum {
    UMI_BUILD_ARTIFACT_PRESENT = 1U << 0,
    UMI_BUILD_ARTIFACT_PRIMARY = 1U << 1,
    UMI_BUILD_ARTIFACT_DEPLOYABLE = 1U << 2,
    UMI_BUILD_ARTIFACT_REPRODUCIBLE = 1U << 3,
    UMI_BUILD_ARTIFACT_SIGNED = 1U << 4,
    UMI_BUILD_ARTIFACT_VERIFIED = 1U << 5,
    UMI_BUILD_ARTIFACT_DEBUG = 1U << 6,
    UMI_BUILD_ARTIFACT_GENERATED = 1U << 7,
    UMI_BUILD_ARTIFACT_MANIFEST_ALL_FLAGS = (1U << 8) - 1U
};

typedef struct UmiBuildArtifactManifest {
    uint32_t structure_size;
    uint32_t api_version;
    uint64_t operation_id;
    char artifact_id[UMI_BUILD_ID_CAPACITY];
    char node_id[UMI_BUILD_ID_CAPACITY];
    char target_id[UMI_BUILD_ID_CAPACITY];
    char profile_id[UMI_BUILD_ID_CAPACITY];
    char configuration[UMI_BUILD_NAME_CAPACITY];
    char kind[UMI_BUILD_NAME_CAPACITY];
    char format[UMI_BUILD_NAME_CAPACITY];
    char path[UMI_BUILD_PATH_CAPACITY];
    char content_type[UMI_BUILD_CONTENT_TYPE_CAPACITY];
    char platform[UMI_BUILD_PLATFORM_CAPACITY];
    char architecture[UMI_BUILD_PLATFORM_CAPACITY];
    char checksum_algorithm[UMI_BUILD_NAME_CAPACITY];
    char checksum[UMI_BUILD_CHECKSUM_CAPACITY];
    char source_revision[UMI_BUILD_NAME_CAPACITY];
    uint64_t size_bytes;
    uint64_t created_at;
    uint64_t build_revision;
    uint64_t revision;
    UmiBuildArtifactManifestFlags flags;
} UmiBuildArtifactManifest;

typedef struct UmiBuildArtifactManifestRegistrySnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t artifact_count;
    size_t present_count;
    size_t primary_count;
    size_t deployable_count;
    size_t reproducible_count;
    size_t signed_count;
    size_t verified_count;
    uint64_t total_size_bytes;
    uint64_t revision;
} UmiBuildArtifactManifestRegistrySnapshot;

typedef struct UmiBuildArtifactManifestRegistry
    UmiBuildArtifactManifestRegistry;

void umi_build_artifact_manifest_init(
    UmiBuildArtifactManifest *manifest,
    const char *artifact_id,
    const char *path,
    const char *kind);
UmiStatus umi_build_artifact_manifest_import(
    const UmiBuildArtifactSnapshot *artifact,
    UmiBuildArtifactManifest *out_manifest);
UmiStatus umi_build_artifact_manifest_validate(
    const UmiBuildArtifactManifest *manifest,
    char *out_message,
    size_t message_capacity);
UmiStatus umi_build_artifact_manifest_registry_create(
    UmiBuildArtifactManifestRegistry **out_registry);
void umi_build_artifact_manifest_registry_destroy(
    UmiBuildArtifactManifestRegistry *registry);
void umi_build_artifact_manifest_registry_clear(
    UmiBuildArtifactManifestRegistry *registry);
UmiStatus umi_build_artifact_manifest_registry_upsert(
    UmiBuildArtifactManifestRegistry *registry,
    const UmiBuildArtifactManifest *manifest);
UmiStatus umi_build_artifact_manifest_registry_remove(
    UmiBuildArtifactManifestRegistry *registry,
    const char *artifact_id);
UmiStatus umi_build_artifact_manifest_registry_find(
    const UmiBuildArtifactManifestRegistry *registry,
    const char *artifact_id,
    UmiBuildArtifactManifest *out_manifest);
UmiStatus umi_build_artifact_manifest_registry_at(
    const UmiBuildArtifactManifestRegistry *registry,
    size_t position,
    UmiBuildArtifactManifest *out_manifest);
UmiStatus umi_build_artifact_manifest_registry_primary_deployable(
    const UmiBuildArtifactManifestRegistry *registry,
    UmiBuildArtifactManifest *out_manifest);
UmiStatus umi_build_artifact_manifest_registry_snapshot(
    const UmiBuildArtifactManifestRegistry *registry,
    UmiBuildArtifactManifestRegistrySnapshot *out_snapshot);
size_t umi_build_artifact_manifest_registry_count(
    const UmiBuildArtifactManifestRegistry *registry);
uint64_t umi_build_artifact_manifest_registry_revision(
    const UmiBuildArtifactManifestRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_ARTIFACT_MANIFEST_H */
