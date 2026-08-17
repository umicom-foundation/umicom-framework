/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/artifact_manifest.c
 *
 * PURPOSE:
 *   Implement build-artifact provenance and deployment-readiness evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/artifact_manifest.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildArtifactManifestRegistry {
    UmiBuildArtifactManifest manifests[UMI_BUILD_ARTIFACT_MANIFEST_CAPACITY];
    size_t count;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static void write_message(char *destination, size_t capacity,
                          const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_index(const UmiBuildArtifactManifestRegistry *registry,
                         const char *artifact_id)
{
    size_t index;
    if (registry == NULL || artifact_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->manifests[index].artifact_id, artifact_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

void umi_build_artifact_manifest_init(
    UmiBuildArtifactManifest *manifest,
    const char *artifact_id,
    const char *path,
    const char *kind)
{
    if (manifest == NULL) return;
    (void)memset(manifest, 0, sizeof(*manifest));
    manifest->structure_size = (uint32_t)sizeof(*manifest);
    manifest->api_version = UMI_BUILD_ARTIFACT_MANIFEST_API_VERSION;
    if (artifact_id != NULL)
        (void)copy_text(manifest->artifact_id,
                        sizeof(manifest->artifact_id), artifact_id);
    if (path != NULL)
        (void)copy_text(manifest->path, sizeof(manifest->path), path);
    if (kind != NULL)
        (void)copy_text(manifest->kind, sizeof(manifest->kind), kind);
    manifest->flags = UMI_BUILD_ARTIFACT_PRESENT;
    manifest->revision = 1U;
}

UmiStatus umi_build_artifact_manifest_import(
    const UmiBuildArtifactSnapshot *artifact,
    UmiBuildArtifactManifest *out_manifest)
{
    if (artifact == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_artifact_manifest_init(
        out_manifest, artifact->artifact_id, artifact->path, artifact->kind);
    out_manifest->operation_id = artifact->operation_id;
    (void)copy_text(out_manifest->node_id, sizeof(out_manifest->node_id),
                    artifact->node_id);
    out_manifest->size_bytes = artifact->size_bytes;
    out_manifest->build_revision = artifact->revision;
    out_manifest->revision = artifact->revision == 0U ? 1U
                                                       : artifact->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_validate(
    const UmiBuildArtifactManifest *manifest,
    char *out_message,
    size_t message_capacity)
{
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (manifest->artifact_id[0] == '\0' || manifest->path[0] == '\0' ||
        manifest->kind[0] == '\0') {
        write_message(out_message, message_capacity,
                      "Artifact id, path and kind are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((manifest->flags &
         ~(UmiBuildArtifactManifestFlags)
             UMI_BUILD_ARTIFACT_MANIFEST_ALL_FLAGS) != 0U) {
        write_message(out_message, message_capacity,
                      "Artifact flags are invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((manifest->flags & UMI_BUILD_ARTIFACT_VERIFIED) != 0U &&
        (manifest->checksum_algorithm[0] == '\0' ||
         manifest->checksum[0] == '\0')) {
        write_message(out_message, message_capacity,
                      "Verified artifacts require checksum evidence.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((manifest->flags & UMI_BUILD_ARTIFACT_SIGNED) != 0U &&
        (manifest->flags & UMI_BUILD_ARTIFACT_VERIFIED) == 0U) {
        write_message(out_message, message_capacity,
                      "Signed artifacts must also be verified.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((manifest->flags & UMI_BUILD_ARTIFACT_DEPLOYABLE) != 0U &&
        (manifest->flags & UMI_BUILD_ARTIFACT_PRESENT) == 0U) {
        write_message(out_message, message_capacity,
                      "Deployable artifacts must be present.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    write_message(out_message, message_capacity, "Artifact manifest is valid.");
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_create(
    UmiBuildArtifactManifestRegistry **out_registry)
{
    UmiBuildArtifactManifestRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiBuildArtifactManifestRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_build_artifact_manifest_registry_destroy(
    UmiBuildArtifactManifestRegistry *registry)
{
    free(registry);
}

void umi_build_artifact_manifest_registry_clear(
    UmiBuildArtifactManifestRegistry *registry)
{
    if (registry == NULL) return;
    (void)memset(registry->manifests, 0, sizeof(registry->manifests));
    registry->count = 0U;
    registry->revision += 1U;
}

UmiStatus umi_build_artifact_manifest_registry_upsert(
    UmiBuildArtifactManifestRegistry *registry,
    const UmiBuildArtifactManifest *manifest)
{
    UmiBuildArtifactManifest copy;
    size_t index;
    char message[256];
    UmiStatus status;
    if (registry == NULL || manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_build_artifact_manifest_validate(
        manifest, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    copy = *manifest;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_ARTIFACT_MANIFEST_API_VERSION;
    if (copy.revision == 0U) copy.revision = 1U;
    index = find_index(registry, manifest->artifact_id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_BUILD_ARTIFACT_MANIFEST_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        registry->manifests[registry->count++] = copy;
    } else {
        copy.revision = registry->manifests[index].revision + 1U;
        registry->manifests[index] = copy;
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_remove(
    UmiBuildArtifactManifestRegistry *registry,
    const char *artifact_id)
{
    size_t index;
    if (registry == NULL || artifact_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, artifact_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->manifests[index],
                      &registry->manifests[index + 1U],
                      (registry->count - index - 1U) *
                          sizeof(registry->manifests[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_find(
    const UmiBuildArtifactManifestRegistry *registry,
    const char *artifact_id,
    UmiBuildArtifactManifest *out_manifest)
{
    size_t index;
    if (registry == NULL || artifact_id == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, artifact_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_manifest = registry->manifests[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_at(
    const UmiBuildArtifactManifestRegistry *registry,
    size_t position,
    UmiBuildArtifactManifest *out_manifest)
{
    if (registry == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_manifest = registry->manifests[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_primary_deployable(
    const UmiBuildArtifactManifestRegistry *registry,
    UmiBuildArtifactManifest *out_manifest)
{
    size_t index;
    size_t first_deployable = SIZE_MAX;
    if (registry == NULL || out_manifest == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        const UmiBuildArtifactManifestFlags flags =
            registry->manifests[index].flags;
        if ((flags & (UMI_BUILD_ARTIFACT_PRESENT |
                      UMI_BUILD_ARTIFACT_DEPLOYABLE)) ==
            (UMI_BUILD_ARTIFACT_PRESENT | UMI_BUILD_ARTIFACT_DEPLOYABLE)) {
            if (first_deployable == SIZE_MAX) first_deployable = index;
            if ((flags & UMI_BUILD_ARTIFACT_PRIMARY) != 0U) {
                *out_manifest = registry->manifests[index];
                return UMI_STATUS_OK;
            }
        }
    }
    if (first_deployable == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_manifest = registry->manifests[first_deployable];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_artifact_manifest_registry_snapshot(
    const UmiBuildArtifactManifestRegistry *registry,
    UmiBuildArtifactManifestRegistrySnapshot *out_snapshot)
{
    size_t index;
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_ARTIFACT_MANIFEST_API_VERSION;
    out_snapshot->artifact_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        const UmiBuildArtifactManifest *manifest = &registry->manifests[index];
        if ((manifest->flags & UMI_BUILD_ARTIFACT_PRESENT) != 0U)
            out_snapshot->present_count += 1U;
        if ((manifest->flags & UMI_BUILD_ARTIFACT_PRIMARY) != 0U)
            out_snapshot->primary_count += 1U;
        if ((manifest->flags & UMI_BUILD_ARTIFACT_DEPLOYABLE) != 0U)
            out_snapshot->deployable_count += 1U;
        if ((manifest->flags & UMI_BUILD_ARTIFACT_REPRODUCIBLE) != 0U)
            out_snapshot->reproducible_count += 1U;
        if ((manifest->flags & UMI_BUILD_ARTIFACT_SIGNED) != 0U)
            out_snapshot->signed_count += 1U;
        if ((manifest->flags & UMI_BUILD_ARTIFACT_VERIFIED) != 0U)
            out_snapshot->verified_count += 1U;
        out_snapshot->total_size_bytes += manifest->size_bytes;
    }
    return UMI_STATUS_OK;
}

size_t umi_build_artifact_manifest_registry_count(
    const UmiBuildArtifactManifestRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_build_artifact_manifest_registry_revision(
    const UmiBuildArtifactManifestRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
