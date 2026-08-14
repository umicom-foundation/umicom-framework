/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/product_release.c
 *
 * PURPOSE:
 *   Compose package, installer, evidence and update contracts for one release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/product_release.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_product_release_init(UmiProductRelease *release,
                                       const char *release_id,
                                       UmiReleaseChannel channel,
                                       const UmiPackageManifest *package,
                                       const UmiUpdateChannel *update_channel)
{
    UmiStatus status;
    if (release == NULL || release_id == NULL || package == NULL ||
        update_channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(release, 0, sizeof(*release));
    status = umi_delivery_copy_text(release->release_id,
                                    sizeof(release->release_id), release_id);
    if (status != UMI_STATUS_OK) return status;
    release->channel = channel;
    release->package = *package;
    release->update_channel = *update_channel;
    umi_release_evidence_init(&release->evidence);
    return UMI_STATUS_OK;
}

UmiStatus umi_product_release_add_installer(UmiProductRelease *release,
                                                const UmiInstallerPlan *installer)
{
    if (release == NULL || installer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (release->installer_count >= UMI_PRODUCT_RELEASE_MAX_INSTALLERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_installer_plan_validate(installer) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    release->installers[release->installer_count] = *installer;
    ++release->installer_count;
    return UMI_STATUS_OK;
}

size_t umi_product_release_blocker_count(const UmiProductRelease *release)
{
    size_t blockers = 0U;
    size_t index;
    if (release == NULL) return 4U;
    if (release->release_id[0] == '\0') ++blockers;
    if (umi_package_manifest_validate(&release->package) != UMI_STATUS_OK) {
        ++blockers;
    }
    if (umi_update_channel_validate(&release->update_channel) != UMI_STATUS_OK ||
        release->update_channel.channel != release->channel) ++blockers;
    if (release->installer_count == 0U) ++blockers;
    for (index = 0U; index < release->installer_count; ++index) {
        if (umi_installer_plan_validate(&release->installers[index]) !=
            UMI_STATUS_OK) ++blockers;
    }
    blockers += umi_release_evidence_blocker_count(&release->evidence,
                                                       release->channel);
    return blockers;
}

int umi_product_release_ready(const UmiProductRelease *release)
{
    return umi_product_release_blocker_count(release) == 0U;
}
