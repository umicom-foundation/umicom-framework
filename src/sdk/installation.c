/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/installation.c
 *
 * PURPOSE:
 *   Describe an installed SDK prefix and its include, library and CMake directories.
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
#include "umicom/sdk/installation.h"
#include <stdio.h>
#include "umicom/platform/path.h"
#include <string.h>
/*
 * Provide the sdk installation from prefix operation used by this module and its client
 * applications.
 */
/* Retain the former path assembly for review. Every derived path now has a
 * checked bound and failed assembly preserves the caller's previous value. */
#if 0
UmiStatus umi_sdk_installation_from_prefix(const char *p,UmiSdkInstallation *o){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p[0]=='\0'||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));n=snprintf(o->prefix,sizeof(o->prefix),"%s",p);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=sizeof(o->prefix))return UMI_STATUS_CAPACITY_EXCEEDED;(void)snprintf(o->include_dir,sizeof(o->include_dir),"%s/include",p);(void)snprintf(o->library_dir,sizeof(o->library_dir),"%s/lib",p);(void)snprintf(o->cmake_dir,sizeof(o->cmake_dir),"%s/lib/cmake/UmicomFramework",p);return UMI_STATUS_OK;}

#endif

UmiStatus umi_sdk_installation_from_prefix(const char *prefix, UmiSdkInstallation *out)
{
    if (prefix == NULL || prefix[0] == '\0' || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSdkInstallation candidate = {0};
    /* Build all paths privately; a long prefix must not publish truncated fields. */
    UmiStatus status = umi_path_copy(candidate.prefix, sizeof candidate.prefix, prefix);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "include", candidate.include_dir, sizeof candidate.include_dir);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "lib", candidate.library_dir, sizeof candidate.library_dir);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "lib/cmake/UmicomFramework", candidate.cmake_dir, sizeof candidate.cmake_dir);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
