/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/package.c
 *
 * PURPOSE:
 *   Represent the CMake package files expected from an installed SDK.
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
#include "umicom/sdk/package.h"
#include <stdio.h>
#include "umicom/platform/path.h"
#include <string.h>
/* Provide the copy path operation used by this module and its client applications. */
/* Retain the former path assembly for review. Every derived path now has a
 * checked bound and failed assembly preserves the caller's previous value. */
#if 0
static UmiStatus copy_path(char *b,size_t c,const char *f,const char *p){int n=snprintf(b,c,f,p);return n<0||(size_t)n>=c?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
/*
 * Provide the sdk package from prefix operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_package_from_prefix(const char *p,UmiSdkPackage *o){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p[0]=='\0'||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(copy_path(o->prefix,sizeof(o->prefix),"%s",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(copy_path(o->config_file,sizeof(o->config_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkConfig.cmake",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(copy_path(o->version_file,sizeof(o->version_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkConfigVersion.cmake",p)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return copy_path(o->targets_file,sizeof(o->targets_file),"%s/lib/cmake/UmicomFramework/UmicomFrameworkTargets.cmake",p);}

#endif

UmiStatus umi_sdk_package_from_prefix(const char *prefix, UmiSdkPackage *out)
{
    if (prefix == NULL || prefix[0] == '\0' || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSdkPackage candidate = {0};
    /* Build all paths privately; a long prefix must not publish truncated fields. */
    UmiStatus status = umi_path_copy(candidate.prefix, sizeof candidate.prefix, prefix);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkConfig.cmake", candidate.config_file, sizeof candidate.config_file);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkConfigVersion.cmake", candidate.version_file, sizeof candidate.version_file);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "lib/cmake/UmicomFramework/UmicomFrameworkTargets.cmake", candidate.targets_file, sizeof candidate.targets_file);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
