/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/probe.c
 *
 * PURPOSE:
 *   Probe required public headers, libraries and CMake package files.
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
#include "umicom/sdk/probe.h"
#include "umicom/sdk/package.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/sdk/installation_inspection.h"
/* Provide the sdk probe prefix operation used by this module and its client applications. */
/* Retain the directory-only probe for review. A library directory alone is
 * not evidence of a usable archive; the active probe requires a base file and
 * uses installation-owned layout metadata when available. */
#if 0
UmiStatus umi_sdk_probe_prefix(const char *p,UmiSdkProbeResult *o){UmiSdkPackage pkg;char header[1024];char libdir[1024];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_sdk_package_from_prefix(p,&pkg)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;(void)snprintf(header,sizeof(header),"%s/include/umicom/umicom.h",p);(void)snprintf(libdir,sizeof(libdir),"%s/lib",p);o->package_config=umi_fs_is_file(pkg.config_file)&&umi_fs_is_file(pkg.targets_file);o->public_header=umi_fs_is_file(header);o->libraries=umi_fs_is_directory(libdir);o->passed=o->package_config&&o->public_header&&o->libraries;return UMI_STATUS_OK;}

#endif

UmiStatus umi_sdk_probe_prefix(const char *prefix, UmiSdkProbeResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (prefix == NULL || !umi_path_is_absolute(prefix)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSdkInstallationObservation *observation = calloc(1U, sizeof *observation);
    if (observation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiSdkInstallationInspect(prefix, observation);
    if (status == UMI_STATUS_OK) {
        out->package_config = observation->package_config && observation->package_targets &&
            observation->package_version;
        out->public_header = observation->header_present;
        out->libraries = observation->base_library_present;
        out->passed = observation->complete;
        free(observation);
        return UMI_STATUS_OK;
    }
    free(observation);
    /* Older installations have no receipt. Keep a conservative presence-only
     * fallback for their conventional layout, without inventing version evidence.
     * Malformed or unreadable metadata is an error, never a reason to fall back. */
    if (status != UMI_STATUS_NOT_FOUND) return status;
    UmiSdkPackage package;
    status = umi_sdk_package_from_prefix(prefix, &package);
    char path[UMI_PATH_CAPACITY], library[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "include/umicom/umicom.h", path, sizeof path);
    if (status != UMI_STATUS_OK) return status;
    UmiSdkProbeResult candidate = {0};
    candidate.package_config = umi_fs_is_file(package.config_file) &&
        umi_fs_is_file(package.targets_file) && umi_fs_is_file(package.version_file);
    candidate.public_header = umi_fs_is_file(path);
    static const char *const libraries[] = {
        "lib/libumicom_base.a", "lib/umicom_base.lib", "lib/umicom_base.a"
    };
    for (size_t index = 0U; index < sizeof libraries / sizeof libraries[0]; ++index) {
        status = umi_path_join(prefix, libraries[index], library, sizeof library);
        if (status != UMI_STATUS_OK) return status;
        if (umi_fs_is_file(library)) candidate.libraries = 1;
    }
    candidate.passed = candidate.package_config && candidate.public_header && candidate.libraries;
    *out = candidate;
    return UMI_STATUS_OK;
}
