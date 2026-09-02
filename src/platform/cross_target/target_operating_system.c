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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_operating_system.h"

#include <string.h>
/*
 * Read ct operating system into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ct_operating_system_parse(const char *text, UmiCtOperatingSystem *out_os) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_os == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "windows") == 0 || strcmp(text, "win32") == 0) {
        *out_os = UMI_CT_OS_WINDOWS;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "linux") == 0) {
        *out_os = UMI_CT_OS_LINUX;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "darwin") == 0 || strcmp(text, "macos") == 0) {
        *out_os = UMI_CT_OS_MACOS;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "freebsd") == 0) {
        *out_os = UMI_CT_OS_FREEBSD;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "umicom") == 0 || strcmp(text, "umos") == 0) {
        *out_os = UMI_CT_OS_UMICOM;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text, "none") == 0 || strcmp(text, "baremetal") == 0) {
        *out_os = UMI_CT_OS_BARE_METAL;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the ct operating system text operation used by this module and its client
 * applications.
 */
const char *umi_ct_operating_system_text(UmiCtOperatingSystem os){/* Select the behaviour associated with the requested command or state value. */ switch(os){case UMI_CT_OS_WINDOWS:return"windows";case UMI_CT_OS_LINUX:return"linux";case UMI_CT_OS_MACOS:return"macos";case UMI_CT_OS_FREEBSD:return"freebsd";case UMI_CT_OS_UMICOM:return"umicom";case UMI_CT_OS_BARE_METAL:return"bare-metal";default:return"unknown";}}
/*
 * Provide the ct operating system is unix like operation used by this module and its
 * client applications.
 */
bool umi_ct_operating_system_is_unix_like(UmiCtOperatingSystem os){return os==UMI_CT_OS_LINUX||os==UMI_CT_OS_MACOS||os==UMI_CT_OS_FREEBSD||os==UMI_CT_OS_UMICOM;}
