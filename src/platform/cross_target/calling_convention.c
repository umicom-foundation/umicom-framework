/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/calling_convention.c
 *
 * PURPOSE:
 *   Map target architecture/OS combinations onto canonical calling-convention identities.
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

#include "umicom/platform/cross_target/calling_convention.h"

/*
 * Provide the ct calling convention default operation used by this module and its client
 * applications.
 */
UmiCtCallingConvention umi_ct_calling_convention_default(UmiCtArchitecture a,UmiCtOperatingSystem os){/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_UMICOM)return UMI_CT_CALL_UMICOM;/* Apply this branch only when its contract condition is satisfied. */ if(a==UMI_CT_ARCH_RISCV32||a==UMI_CT_ARCH_RISCV64)return UMI_CT_CALL_RISCV;/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_WINDOWS&&a==UMI_CT_ARCH_X86_64)return UMI_CT_CALL_WIN64;/* Apply this branch only when its contract condition is satisfied. */ if(os==UMI_CT_OS_LINUX||os==UMI_CT_OS_MACOS||os==UMI_CT_OS_FREEBSD)return UMI_CT_CALL_SYSV;return UMI_CT_CALL_C;}
/*
 * Provide the ct calling convention text operation used by this module and its client
 * applications.
 */
const char *umi_ct_calling_convention_text(UmiCtCallingConvention c){/* Select the behaviour associated with the requested command or state value. */ switch(c){case UMI_CT_CALL_C:return"c";case UMI_CT_CALL_SYSV:return"sysv";case UMI_CT_CALL_WIN64:return"win64";case UMI_CT_CALL_RISCV:return"riscv";case UMI_CT_CALL_UMICOM:return"umicom";default:return"unknown";}}
