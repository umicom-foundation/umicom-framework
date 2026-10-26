/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/os_image/profile.h
 *
 * PURPOSE:
 *   Expose validated OS source profiles for inspection without building or executing their contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OS_IMAGE_PROFILE_H
#define UMICOM_OS_IMAGE_PROFILE_H
#include "umicom/os_image/image.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiOsImageProfile UmiOsImageProfile;
typedef struct UmiOsImageProfileInfo {
    char buildrootCommit[41];
    char linuxVersion[32];
    char linuxSha256[65];
    char profileSha256[65];
    size_t fileCount;
} UmiOsImageProfileInfo;

/* Open copies validated data into an immutable owner. Failure preserves
 * *outProfile. FileAt returns borrowed text valid until Destroy; Info copies
 * its value. A profile hash identifies recipe text, not captured source bytes,
 * a successful build, licence approval or publisher authenticity. */
UmiStatus UmiOsImageProfileOpen(const void *data, size_t size, UmiOsImageProfile **outProfile);
UmiStatus UmiOsImageProfileRead(const char *absolutePath, UmiOsImageProfile **outProfile);
UmiStatus UmiOsImageProfileInfoRead(const UmiOsImageProfile *profile, UmiOsImageProfileInfo *outInfo);
const char *UmiOsImageProfileFileAt(const UmiOsImageProfile *profile, size_t index);
void UmiOsImageProfileDestroy(UmiOsImageProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
