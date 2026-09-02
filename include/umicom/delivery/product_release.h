/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/product_release.h
 *
 * PURPOSE:
 *   Compose package, installer, evidence and update contracts for one release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PRODUCT_RELEASE_H
#define UMICOM_DELIVERY_PRODUCT_RELEASE_H

#include "umicom/delivery/installer.h"
#include "umicom/delivery/package_manifest.h"
#include "umicom/delivery/release_evidence.h"
#include "umicom/delivery/update_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_RELEASE_MAX_INSTALLERS 4U

/**
 * Represent the product release data shared with callers of this public contract.
 */
typedef struct UmiProductRelease {
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel channel;
    UmiPackageManifest package;
    UmiInstallerPlan installers[UMI_PRODUCT_RELEASE_MAX_INSTALLERS];
    size_t installer_count;
    UmiReleaseEvidence evidence;
    UmiUpdateChannel update_channel;
} UmiProductRelease;

/**
 * Initialise product release from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_release_init(UmiProductRelease *release,
                                       const char *release_id,
                                       UmiReleaseChannel channel,
                                       const UmiPackageManifest *package,
                                       const UmiUpdateChannel *update_channel);
/**
 * Provide the product release add installer operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_release_add_installer(UmiProductRelease *release,
                                                const UmiInstallerPlan *installer);
/**
 * Return the number of records represented by product release blocker without changing
 * their state.
 */
size_t umi_product_release_blocker_count(const UmiProductRelease *release);
/**
 * Provide the product release ready operation used by this module and its client
 * applications.
 */
int umi_product_release_ready(const UmiProductRelease *release);

#ifdef __cplusplus
}
#endif
#endif
