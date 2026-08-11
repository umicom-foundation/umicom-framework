/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/manifest.c
 *
 * PURPOSE:
 *   Describe an application release independently from the package technology used to distribute it.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The manifest is the stable identity card for a release and links source revision, version, channel and generation.
 */

#include "umicom/delivery/manifest.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_delivery_manifest_init(UmiDeliveryManifest *manifest,
                                     const char *application_id,
                                     const char *release_id,
                                     const char *version,
                                     UmiReleaseChannel channel)
{
    UmiStatus status;
    if (manifest == NULL || application_id == NULL || release_id == NULL ||
        version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->application_id,
                                    sizeof(manifest->application_id),
                                    application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->release_id,
                                    sizeof(manifest->release_id), release_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    if (status != UMI_STATUS_OK) return status;
    manifest->channel = channel;
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_manifest_set_generation(UmiDeliveryManifest *manifest,
                                               const char *generation_id)
{
    if (manifest == NULL || generation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->generation_id,
                                  sizeof(manifest->generation_id), generation_id);
}

UmiStatus umi_delivery_manifest_set_source_revision(UmiDeliveryManifest *manifest,
                                                    const char *revision)
{
    if (manifest == NULL || revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->source_revision,
                                  sizeof(manifest->source_revision), revision);
}

UmiStatus umi_delivery_manifest_validate(const UmiDeliveryManifest *manifest)
{
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (manifest->application_id[0] == '\0' || manifest->release_id[0] == '\0' ||
        manifest->version[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
