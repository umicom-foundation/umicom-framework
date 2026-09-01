/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/manifest.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_MANIFEST_H
#define INCLUDE_UMICOM_DELIVERY_MANIFEST_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryManifest {
    char application_id[UMI_DELIVERY_ID_CAPACITY];
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char generation_id[UMI_DELIVERY_ID_CAPACITY];
    char source_revision[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel channel;
    uint64_t created_epoch_ms;
    size_t artifact_count;
} UmiDeliveryManifest;

UmiStatus umi_delivery_manifest_init(UmiDeliveryManifest *manifest,
                                     const char *application_id,
                                     const char *release_id,
                                     const char *version,
                                     UmiReleaseChannel channel);
UmiStatus umi_delivery_manifest_set_generation(UmiDeliveryManifest *manifest,
                                               const char *generation_id);
UmiStatus umi_delivery_manifest_set_source_revision(UmiDeliveryManifest *manifest,
                                                    const char *revision);
UmiStatus umi_delivery_manifest_validate(const UmiDeliveryManifest *manifest);

#ifdef __cplusplus
}
#endif

#endif
