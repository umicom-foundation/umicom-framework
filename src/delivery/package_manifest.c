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

UmiStatus umi_package_manifest_init(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format)
{
    UmiStatus status;
    if (manifest == NULL || product_id == NULL || version == NULL ||
        entrypoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->product_id,
                                    sizeof(manifest->product_id), product_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->entrypoint,
                                    sizeof(manifest->entrypoint), entrypoint);
    if (status != UMI_STATUS_OK) return status;
    manifest->format = format;
    return UMI_STATUS_OK;
}

const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest,
    const char *destination)
{
    size_t index;
    if (manifest == NULL || destination == NULL) return NULL;
    for (index = 0U; index < manifest->count; ++index) {
        if (strcmp(manifest->files[index].destination, destination) == 0) {
            return &manifest->files[index];
        }
    }
    return NULL;
}

UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRole role,
                                       int required)
{
    UmiPackageFile *file;
    UmiStatus status;
    if (manifest == NULL || source == NULL || source[0] == '\0' ||
        destination == NULL || destination[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (manifest->count >= UMI_DELIVERY_MAX_ARTIFACTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_package_manifest_find(manifest, destination) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    file = &manifest->files[manifest->count];
    (void)memset(file, 0, sizeof(*file));
    status = umi_delivery_copy_text(file->source, sizeof(file->source), source);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(file->destination,
                                    sizeof(file->destination), destination);
    if (status != UMI_STATUS_OK) return status;
    file->role = role;
    file->required = required != 0;
    ++manifest->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest)
{
    const UmiPackageFile *entrypoint;
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (manifest->product_id[0] == '\0' || manifest->version[0] == '\0' ||
        manifest->entrypoint[0] == '\0' || manifest->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    entrypoint = umi_package_manifest_find(manifest, manifest->entrypoint);
    if (entrypoint == NULL ||
        entrypoint->role != UMI_PACKAGE_FILE_EXECUTABLE ||
        !entrypoint->required) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
