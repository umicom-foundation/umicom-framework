/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/manifest.c
 *
 * PURPOSE:
 *   Describe an application release independently from the package technology used to distribute it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The manifest is the stable identity card for a release and links source revision, version, channel and generation.
 */

#include "umicom/delivery/manifest.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery manifest from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_manifest_init(UmiDeliveryManifest *manifest,
                                     const char *application_id,
                                     const char *release_id,
                                     const char *version,
                                     UmiReleaseChannel channel)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || application_id == NULL || release_id == NULL ||
        version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->application_id,
                                    sizeof(manifest->application_id),
                                    application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->release_id,
                                    sizeof(manifest->release_id), release_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manifest->channel = channel;
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery manifest set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_generation(UmiDeliveryManifest *manifest,
                                               const char *generation_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || generation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->generation_id,
                                  sizeof(manifest->generation_id), generation_id);
}

/*
 * Provide the delivery manifest set source revision operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_source_revision(UmiDeliveryManifest *manifest,
                                                    const char *revision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->source_revision,
                                  sizeof(manifest->source_revision), revision);
}

/* Check that delivery manifest satisfies its contract before another service relies on it. */
UmiStatus umi_delivery_manifest_validate(const UmiDeliveryManifest *manifest)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (manifest->application_id[0] == '\0' || manifest->release_id[0] == '\0' ||
        manifest->version[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
