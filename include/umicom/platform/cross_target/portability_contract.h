/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/portability_contract.h
 *
 * PURPOSE:
 *   Declare required platform semantics for a Framework subsystem so cross-target gaps are explicit.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PORTABILITY_CONTRACT_H
#define UMICOM_PLATFORM_CROSS_TARGET_PORTABILITY_CONTRACT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t UmiCtPortabilityCapability;
enum { UMI_CT_PORT_PATHS=UINT64_C(1)<<0, UMI_CT_PORT_ENVIRONMENT=UINT64_C(1)<<1, UMI_CT_PORT_PROCESS=UINT64_C(1)<<2, UMI_CT_PORT_FILESYSTEM=UINT64_C(1)<<3, UMI_CT_PORT_CLOCK=UINT64_C(1)<<4, UMI_CT_PORT_THREADS=UINT64_C(1)<<5, UMI_CT_PORT_MEMORY=UINT64_C(1)<<6, UMI_CT_PORT_ATOMICS=UINT64_C(1)<<7, UMI_CT_PORT_SOCKETS=UINT64_C(1)<<8, UMI_CT_PORT_DYNLIB=UINT64_C(1)<<9 };
typedef struct UmiCtPortabilityContract { char contract_id[UMI_CT_ID_CAPACITY]; UmiCtPortabilityCapability required; UmiCtPortabilityCapability optional; } UmiCtPortabilityContract;
uint64_t umi_ct_portability_missing(const UmiCtPortabilityContract *contract,uint64_t available);
bool umi_ct_portability_satisfied(const UmiCtPortabilityContract *contract,uint64_t available);

#ifdef __cplusplus
}
#endif

#endif
