/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/profile_inspect.c
 *
 * PURPOSE:
 *   Project the shared bounded source-profile parser through immutable public inspection ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/os_image/profile.h"

struct UmiOsImageProfile {
    OiPlan plan;
    char hash[65];
};
UmiStatus UmiOsImageProfileOpen(const void *data, size_t size, UmiOsImageProfile **out)
{
    if (!out || !data || !size) return UMI_STATUS_INVALID_ARGUMENT;
    UmiOsImageProfile *profile = calloc(1, sizeof *profile);
    if (!profile) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = OiProfileDecode(data, size, &profile->plan);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Buffer(data, size, profile->hash);
    if (status != UMI_STATUS_OK) {
        free(profile);
        return status;
    }
    *out = profile;
    return UMI_STATUS_OK;
}
UmiStatus UmiOsImageProfileRead(const char *path, UmiOsImageProfile **out)
{
    if (!out || OiAbsolute(path, 0) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0;
    UmiStatus status = OiRead(path, OI_META_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiOsImageProfileOpen(bytes, size, out);
    free(bytes);
    return status;
}
UmiStatus UmiOsImageProfileInfoRead(const UmiOsImageProfile *profile, UmiOsImageProfileInfo *out)
{
    if (!profile || !out) return UMI_STATUS_INVALID_ARGUMENT;
    UmiOsImageProfileInfo info = {0};
    memcpy(info.buildrootCommit, profile->plan.commit, sizeof info.buildrootCommit);
    memcpy(info.linuxVersion, profile->plan.linuxVersion, sizeof info.linuxVersion);
    memcpy(info.linuxSha256, profile->plan.linuxHash, sizeof info.linuxSha256);
    memcpy(info.profileSha256, profile->hash, sizeof info.profileSha256);
    info.fileCount = profile->plan.count;
    *out = info;
    return UMI_STATUS_OK;
}
const char *UmiOsImageProfileFileAt(const UmiOsImageProfile *profile, size_t index)
{
    if (!profile || index >= profile->plan.count) return NULL;
    return profile->plan.inputs[index].name;
}
void UmiOsImageProfileDestroy(UmiOsImageProfile *profile)
{
    free(profile);
}
