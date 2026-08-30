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

UmiCtCallingConvention umi_ct_calling_convention_default(UmiCtArchitecture a,UmiCtOperatingSystem os){if(os==UMI_CT_OS_UMICOM)return UMI_CT_CALL_UMICOM;if(a==UMI_CT_ARCH_RISCV32||a==UMI_CT_ARCH_RISCV64)return UMI_CT_CALL_RISCV;if(os==UMI_CT_OS_WINDOWS&&a==UMI_CT_ARCH_X86_64)return UMI_CT_CALL_WIN64;if(os==UMI_CT_OS_LINUX||os==UMI_CT_OS_MACOS||os==UMI_CT_OS_FREEBSD)return UMI_CT_CALL_SYSV;return UMI_CT_CALL_C;}
const char *umi_ct_calling_convention_text(UmiCtCallingConvention c){switch(c){case UMI_CT_CALL_C:return"c";case UMI_CT_CALL_SYSV:return"sysv";case UMI_CT_CALL_WIN64:return"win64";case UMI_CT_CALL_RISCV:return"riscv";case UMI_CT_CALL_UMICOM:return"umicom";default:return"unknown";}}
