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

/*
 * This implementation keeps SDK validation deterministic so generated Umicom applications can prove they are using a complete compatible Framework installation.
 */
#include "umicom/sdk/profile.h"
#include <stddef.h>
/* Check that sdk profile satisfies its contract before another service relies on it. */
UmiStatus umi_sdk_profile_validate(const UmiSdkProfile *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->profile_id==NULL||p->profile_id[0]=='\0'||(p->linkage!=UMI_SDK_LINKAGE_STATIC&&p->linkage!=UMI_SDK_LINKAGE_SHARED))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
