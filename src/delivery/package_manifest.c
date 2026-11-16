/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/package_manifest.c
 *
 * PURPOSE:
 *   Define deployable product files independently from an archive or installer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/package_manifest.h"
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>
#include "umicom/base/snapshot_validation.h"

/*
 * Initialise package manifest from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_package_manifest_init(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || product_id == NULL || version == NULL ||
        entrypoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->product_id,
                                    sizeof(manifest->product_id), product_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->entrypoint,
                                    sizeof(manifest->entrypoint), entrypoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manifest->format = format;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_package_manifest_init_build_value(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || product_id == NULL || version == NULL ||
        entrypoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->product_id,
                                    sizeof(manifest->product_id), product_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->entrypoint,
                                    sizeof(manifest->entrypoint), entrypoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manifest->format = format;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_package_manifest_init, UmiPackageManifest, manifest,
    (UmiPackageManifest *manifest, const char *product_id, const char *version, const char *entrypoint, UmiPackageFormat format),
    umi_package_manifest_init_build_value(candidate, product_id, version, entrypoint, format))


/*
 * Find package manifest while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Manifest operations now validate bounded storage before indexing and publish complete append records. This protects borrowed inputs and rejects malformed restored state. The former implementation is retained for owner review. */
#if 0
const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest,
    const char *destination)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || destination == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(manifest->files[index].destination, destination) == 0) {
            return &manifest->files[index];
        }
    }
    return NULL;
}

#endif

static int PackageFileRoleValid(UmiPackageFileRole role)
{
    return role >= UMI_PACKAGE_FILE_EXECUTABLE && role <= UMI_PACKAGE_FILE_DOCUMENTATION;
}

UmiStatus umi_package_manifest_validate_storage(const UmiPackageManifest *manifest)
{
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (manifest->count > UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_INVALID_STATE;
    static const UmiSnapshotTextField header_fields[] = {
        {"product_id", offsetof(UmiPackageManifest, product_id), sizeof manifest->product_id, 0},
        {"version", offsetof(UmiPackageManifest, version), sizeof manifest->version, 0},
        {"entrypoint", offsetof(UmiPackageManifest, entrypoint), sizeof manifest->entrypoint, 0}
    };
    static const UmiSnapshotTextField file_fields[] = {
        {"source", offsetof(UmiPackageFile, source), sizeof(((UmiPackageFile *)0)->source), 1},
        {"destination", offsetof(UmiPackageFile, destination), sizeof(((UmiPackageFile *)0)->destination), 1}
    };
    if (UmiSnapshotValidateTextFields(manifest, sizeof *manifest, header_fields,
            sizeof header_fields / sizeof header_fields[0], NULL) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    /* Complete every field check before comparing names, including entries
     * after a potential match. Restored counts never authorize an array read. */
    for (size_t i = 0U; i < manifest->count; ++i) {
        const UmiPackageFile *file = &manifest->files[i];
        if (UmiSnapshotValidateTextFields(file, sizeof *file, file_fields,
                sizeof file_fields / sizeof file_fields[0], NULL) != UMI_STATUS_OK ||
            !PackageFileRoleValid(file->role)) return UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0U; i < manifest->count; ++i)
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(manifest->files[i].destination, manifest->files[j].destination) == 0)
                return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest, const char *destination)
{
    if (destination == NULL || umi_package_manifest_validate_storage(manifest) != UMI_STATUS_OK)
        return NULL;
    for (size_t i = 0U; i < manifest->count; ++i)
        if (strcmp(manifest->files[i].destination, destination) == 0) return &manifest->files[i];
    return NULL;
}
/* Add package manifest only after its inputs and available capacity have been checked. */
/* Manifest operations now validate bounded storage before indexing and publish complete append records. This protects borrowed inputs and rejects malformed restored state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRole role,
                                       int required)
{
    UmiPackageFile *file;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || source == NULL || source[0] == '\0' ||
        destination == NULL || destination[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (manifest->count >= UMI_DELIVERY_MAX_ARTIFACTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_package_manifest_find(manifest, destination) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    file = &manifest->files[manifest->count];
    (void)memset(file, 0, sizeof(*file));
    status = umi_delivery_copy_text(file->source, sizeof(file->source), source);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(file->destination,
                                    sizeof(file->destination), destination);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    file->role = role;
    file->required = required != 0;
    ++manifest->count;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
    const char *source, const char *destination, UmiPackageFileRole role, int required)
{
    if (manifest == NULL || source == NULL || source[0] == '\0' ||
        destination == NULL || destination[0] == '\0' || !PackageFileRoleValid(role))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_package_manifest_validate_storage(manifest) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    if (manifest->count == UMI_DELIVERY_MAX_ARTIFACTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_package_manifest_find(manifest, destination) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Stage a single record so a late path refusal cannot damage a prepared
     * unused slot. Inputs may borrow that slot or another live manifest field. */
    UmiPackageFile candidate = {0};
    UmiStatus status = umi_delivery_copy_text(candidate.source, sizeof candidate.source, source);
    if (status == UMI_STATUS_OK)
        status = umi_delivery_copy_text(candidate.destination, sizeof candidate.destination, destination);
    if (status != UMI_STATUS_OK) return status;
    candidate.role = role;
    candidate.required = required != 0;
    manifest->files[manifest->count] = candidate;
    ++manifest->count;
    return UMI_STATUS_OK;
}
/* Check that package manifest satisfies its contract before another service relies on it. */
/* Manifest operations now validate bounded storage before indexing and publish complete append records. This protects borrowed inputs and rejects malformed restored state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest)
{
    const UmiPackageFile *entrypoint;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (manifest->product_id[0] == '\0' || manifest->version[0] == '\0' ||
        manifest->entrypoint[0] == '\0' || manifest->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    entrypoint = umi_package_manifest_find(manifest, manifest->entrypoint);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entrypoint == NULL ||
        entrypoint->role != UMI_PACKAGE_FILE_EXECUTABLE ||
        !entrypoint->required) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest)
{
    UmiStatus status = umi_package_manifest_validate_storage(manifest);
    if (status != UMI_STATUS_OK) return status;
    if (manifest->product_id[0] == '\0' || manifest->version[0] == '\0' ||
        manifest->entrypoint[0] == '\0' || manifest->count == 0U ||
        manifest->format < UMI_PACKAGE_DIRECTORY || manifest->format > UMI_PACKAGE_FLATPAK)
        return UMI_STATUS_INVALID_STATE;
    const UmiPackageFile *entrypoint = umi_package_manifest_find(manifest, manifest->entrypoint);
    if (entrypoint == NULL || entrypoint->role != UMI_PACKAGE_FILE_EXECUTABLE ||
        !entrypoint->required) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}