/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/product_release_v2.h
 *
 * PURPOSE:
 *   Compose package, installer, evidence and update contracts for one release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PRODUCT_RELEASE_V2_H
#define UMICOM_DELIVERY_PRODUCT_RELEASE_V2_H

#include "umicom/delivery/installer_v2.h"
#include "umicom/delivery/package_manifest_v2.h"
#include "umicom/delivery/release_evidence_v2.h"
#include "umicom/delivery/update_channel_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_RELEASE_V2_MAX_INSTALLERS 4U

typedef struct UmiProductReleaseV2 {
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel channel;
    UmiPackageManifestV2 package;
    UmiInstallerPlanV2 installers[UMI_PRODUCT_RELEASE_V2_MAX_INSTALLERS];
    size_t installer_count;
    UmiReleaseEvidenceV2 evidence;
    UmiUpdateChannelV2 update_channel;
} UmiProductReleaseV2;

UmiStatus umi_product_release_v2_init(UmiProductReleaseV2 *release,
                                       const char *release_id,
                                       UmiReleaseChannel channel,
                                       const UmiPackageManifestV2 *package,
                                       const UmiUpdateChannelV2 *update_channel);
UmiStatus umi_product_release_v2_add_installer(UmiProductReleaseV2 *release,
                                                const UmiInstallerPlanV2 *installer);
size_t umi_product_release_v2_blocker_count(const UmiProductReleaseV2 *release);
int umi_product_release_v2_ready(const UmiProductReleaseV2 *release);

#ifdef __cplusplus
}
#endif
#endif
