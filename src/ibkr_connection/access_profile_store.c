/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/access_profile_store.c
 *
 * PURPOSE:
 *   Protect local settings with a cooperative lease and complete-file replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_profiles.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/file_lease.h"
#include <stdio.h>

UmiStatus UmiBrokerAccessProfilesLoad(const char *path,
    UmiBrokerAccessProfiles *outProfiles)
{
    if (outProfiles == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path,
        UMI_BROKER_PROFILES_TEXT_CAPACITY - 1U, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiBrokerAccessProfilesDecode(bytes, size, outProfiles);
    UmiInputFileFree(bytes);
    return status;
}

UmiStatus UmiBrokerAccessProfilesStore(const char *path,
    const UmiBrokerAccessPlan *plan)
{
    if (UmiBrokerAccessPlanValidate(plan) != UMI_STATUS_OK ||
        plan->mode == UMI_BROKER_ACCESS_GUEST)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    char guard[UMI_PATH_CAPACITY];
    int count = snprintf(guard, sizeof(guard), "%s.umi-guard", path);
    if (count < 0 || (size_t)count >= sizeof(guard)) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiFileLease *lease = NULL;
    status = UmiFileLeaseTryAcquire(guard, &lease);
    if (status != UMI_STATUS_OK) return status;
    UmiBrokerAccessProfiles profiles;
    status = UmiBrokerAccessProfilesLoad(path, &profiles);
    if (status == UMI_STATUS_NOT_FOUND)
        status = UmiBrokerAccessProfilesDefault(&profiles);
    /* The lease spans read, validation and replacement. Two cooperating
     * applications therefore cannot erase each other's other-mode settings. */
    if (status == UMI_STATUS_OK) {
        if (plan->mode == UMI_BROKER_ACCESS_PAPER) profiles.paper = *plan;
        else profiles.live = *plan;
        char bytes[UMI_BROKER_PROFILES_TEXT_CAPACITY];
        size_t size = 0U;
        status = UmiBrokerAccessProfilesEncode(&profiles, bytes, sizeof(bytes), &size);
        if (status == UMI_STATUS_OK) status = UmiLocalFileReplace(path, bytes, size);
    }
    UmiStatus released = UmiFileLeaseRelease(&lease);
    return status == UMI_STATUS_OK ? released : status;
}
