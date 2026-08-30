/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/profile.c
 *
 * PURPOSE:
 *   Describe an SDK build profile such as static, shared, debug or release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/profile.h"
#include <stddef.h>
UmiStatus umi_sdk_profile_validate(const UmiSdkProfile *p){if(p==NULL||p->profile_id==NULL||p->profile_id[0]=='\0'||(p->linkage!=UMI_SDK_LINKAGE_STATIC&&p->linkage!=UMI_SDK_LINKAGE_SHARED))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
