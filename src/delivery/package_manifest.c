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
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise package manifest from caller-provided values so later operations receive a
 * known state.
 */
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

/*
 * Find package manifest while leaving the underlying catalogue or model owned by this
 * module.
 */
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

/* Add package manifest only after its inputs and available capacity have been checked. */
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

/* Check that package manifest satisfies its contract before another service relies on it. */
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
