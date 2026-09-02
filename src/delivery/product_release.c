/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/product_release.c
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
#include "umicom/delivery/product_release.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise product release from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_release_init(UmiProductRelease *release,
                                       const char *release_id,
                                       UmiReleaseChannel channel,
                                       const UmiPackageManifest *package,
                                       const UmiUpdateChannel *update_channel)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (release == NULL || release_id == NULL || package == NULL ||
        update_channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(release, 0, sizeof(*release));
    status = umi_delivery_copy_text(release->release_id,
                                    sizeof(release->release_id), release_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    release->channel = channel;
    release->package = *package;
    release->update_channel = *update_channel;
    umi_release_evidence_init(&release->evidence);
    return UMI_STATUS_OK;
}

/*
 * Provide the product release add installer operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_release_add_installer(UmiProductRelease *release,
                                                const UmiInstallerPlan *installer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (release == NULL || installer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (release->installer_count >= UMI_PRODUCT_RELEASE_MAX_INSTALLERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_installer_plan_validate(installer) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    release->installers[release->installer_count] = *installer;
    ++release->installer_count;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by product release blocker without changing
 * their state.
 */
size_t umi_product_release_blocker_count(const UmiProductRelease *release)
{
    size_t blockers = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (release == NULL) return 4U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (release->release_id[0] == '\0') ++blockers;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_package_manifest_validate(&release->package) != UMI_STATUS_OK) {
        ++blockers;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_update_channel_validate(&release->update_channel) != UMI_STATUS_OK ||
        release->update_channel.channel != release->channel) ++blockers;
    /* Apply this branch only when its contract condition is satisfied. */
    if (release->installer_count == 0U) ++blockers;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < release->installer_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_installer_plan_validate(&release->installers[index]) !=
            UMI_STATUS_OK) ++blockers;
    }
    blockers += umi_release_evidence_blocker_count(&release->evidence,
                                                       release->channel);
    return blockers;
}

/*
 * Provide the product release ready operation used by this module and its client
 * applications.
 */
int umi_product_release_ready(const UmiProductRelease *release)
{
    return umi_product_release_blocker_count(release) == 0U;
}
