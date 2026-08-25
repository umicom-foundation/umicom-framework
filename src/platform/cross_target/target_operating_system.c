/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_operating_system.c
 *
 * PURPOSE:
 *   Parse supported host and target operating-system identities including Umicom OS and bare-metal targets.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_operating_system.h"

#include <string.h>
UmiStatus umi_ct_operating_system_parse(const char *text, UmiCtOperatingSystem *out_os) {
    if (text == NULL || out_os == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(text, "windows") == 0 || strcmp(text, "win32") == 0) {
        *out_os = UMI_CT_OS_WINDOWS;
    } else if (strcmp(text, "linux") == 0) {
        *out_os = UMI_CT_OS_LINUX;
    } else if (strcmp(text, "darwin") == 0 || strcmp(text, "macos") == 0) {
        *out_os = UMI_CT_OS_MACOS;
    } else if (strcmp(text, "freebsd") == 0) {
        *out_os = UMI_CT_OS_FREEBSD;
    } else if (strcmp(text, "umicom") == 0 || strcmp(text, "umos") == 0) {
        *out_os = UMI_CT_OS_UMICOM;
    } else if (strcmp(text, "none") == 0 || strcmp(text, "baremetal") == 0) {
        *out_os = UMI_CT_OS_BARE_METAL;
    } else {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
const char *umi_ct_operating_system_text(UmiCtOperatingSystem os){switch(os){case UMI_CT_OS_WINDOWS:return"windows";case UMI_CT_OS_LINUX:return"linux";case UMI_CT_OS_MACOS:return"macos";case UMI_CT_OS_FREEBSD:return"freebsd";case UMI_CT_OS_UMICOM:return"umicom";case UMI_CT_OS_BARE_METAL:return"bare-metal";default:return"unknown";}}
bool umi_ct_operating_system_is_unix_like(UmiCtOperatingSystem os){return os==UMI_CT_OS_LINUX||os==UMI_CT_OS_MACOS||os==UMI_CT_OS_FREEBSD||os==UMI_CT_OS_UMICOM;}
