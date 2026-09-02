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
/*
 * Provide the ct riscv extension feature operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_riscv_extension_feature(const char*e,UmiCtCpuFeature*f){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e==NULL||f==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"a")==0)*f=UMI_CT_CPU_ATOMICS;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"f")==0)*f=UMI_CT_CPU_FLOAT;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"d")==0)*f=UMI_CT_CPU_DOUBLE;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"c")==0)*f=UMI_CT_CPU_COMPRESSED;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"v")==0)*f=UMI_CT_CPU_VECTOR;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strncmp(e,"zb",2U)==0)*f=UMI_CT_CPU_BITMANIP;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strncmp(e,"zk",2U)==0)*f=UMI_CT_CPU_CRYPTO;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(e,"h")==0)*f=UMI_CT_CPU_HYPERVISOR;/* Use this fallback path when the earlier condition does not apply. */ else return UMI_STATUS_NOT_FOUND;return UMI_STATUS_OK;}
/*
 * Provide the ct riscv extension known operation used by this module and its client
 * applications.
 */
bool umi_ct_riscv_extension_known(const char*e){UmiCtCpuFeature f;return umi_ct_riscv_extension_feature(e,&f)==UMI_STATUS_OK|| (e!=NULL&&(strcmp(e,"i")==0||strcmp(e,"m")==0||strcmp(e,"zicsr")==0||strcmp(e,"zifencei")==0));}
