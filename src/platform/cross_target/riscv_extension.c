/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_extension.c
 *
 * PURPOSE:
 *   Map standard RISC-V ISA extension names onto Framework CPU capability bits.
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

#include "umicom/platform/cross_target/riscv_extension.h"

#include <string.h>
UmiStatus umi_ct_riscv_extension_feature(const char*e,UmiCtCpuFeature*f){if(e==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strcmp(e,"a")==0)*f=UMI_CT_CPU_ATOMICS;else if(strcmp(e,"f")==0)*f=UMI_CT_CPU_FLOAT;else if(strcmp(e,"d")==0)*f=UMI_CT_CPU_DOUBLE;else if(strcmp(e,"c")==0)*f=UMI_CT_CPU_COMPRESSED;else if(strcmp(e,"v")==0)*f=UMI_CT_CPU_VECTOR;else if(strncmp(e,"zb",2U)==0)*f=UMI_CT_CPU_BITMANIP;else if(strncmp(e,"zk",2U)==0)*f=UMI_CT_CPU_CRYPTO;else if(strcmp(e,"h")==0)*f=UMI_CT_CPU_HYPERVISOR;else return UMI_STATUS_NOT_FOUND;return UMI_STATUS_OK;}
bool umi_ct_riscv_extension_known(const char*e){UmiCtCpuFeature f;return umi_ct_riscv_extension_feature(e,&f)==UMI_STATUS_OK|| (e!=NULL&&(strcmp(e,"i")==0||strcmp(e,"m")==0||strcmp(e,"zicsr")==0||strcmp(e,"zifencei")==0));}
