/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/locator.c
 *
 * PURPOSE:
 *   Locate an installed SDK from an explicit prefix or conventional directory.
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
#include "umicom/sdk/locator.h"
#include "umicom/sdk/package.h"
#include "umicom/platform/filesystem.h"
#include "umicom/base/version.h"
#include <stdio.h>
#include "umicom/sdk/probe.h"
/* Provide the valid operation used by this module and its client applications. */
/* Preserve the original fallback search for review. The active locator treats
 * a supplied prefix as an explicit selection and searches defaults only when
 * no selection was supplied. Presence checks now require a real base archive. */
#if 0
static int valid(const char *p){UmiSdkPackage pkg;return umi_sdk_package_from_prefix(p,&pkg)==UMI_STATUS_OK&&umi_fs_is_file(pkg.config_file);}
/* Provide the sdk locate operation used by this module and its client applications. */
UmiStatus umi_sdk_locate(const char *preferred,char *out,size_t cap){const char *candidates[]={preferred,"C:/Dev/umicom/sdk/umicom-framework-" UMICOM_FRAMEWORK_VERSION_STRING "-local","/usr/local","/usr"};size_t i;int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<sizeof(candidates)/sizeof(candidates[0]);++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(candidates[i]!=NULL&&valid(candidates[i])){n=snprintf(out,cap,"%s",candidates[i]);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}

#endif

static UmiStatus LocateSelected(const char *prefix, char *out, size_t capacity)
{
    UmiSdkProbeResult probe;
    UmiStatus status = umi_sdk_probe_prefix(prefix, &probe);
    if (status != UMI_STATUS_OK) return status;
    if (!probe.passed) return UMI_STATUS_NOT_FOUND;
    return umi_path_copy(out, capacity, prefix);
}

UmiStatus umi_sdk_locate(const char *preferred, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    /* An explicit selection is authoritative. Silently substituting another SDK
     * can build against a different ABI while the UI still shows the user's path. */
    if (preferred != NULL && preferred[0] != '\0')
        return LocateSelected(preferred, out, capacity);
    const char *candidates[] = {
        "C:/Dev/umicom/sdk/umicom-framework-" UMICOM_FRAMEWORK_VERSION_STRING "-local",
        "/usr/local", "/usr"
    };
    for (size_t i = 0U; i < sizeof candidates / sizeof candidates[0]; ++i) {
        UmiStatus status = LocateSelected(candidates[i], out, capacity);
        if (status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED ||
            status == UMI_STATUS_OUT_OF_MEMORY) return status;
    }
    return UMI_STATUS_NOT_FOUND;
}
