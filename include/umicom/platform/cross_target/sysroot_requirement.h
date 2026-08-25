/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/sysroot_requirement.h
 *
 * PURPOSE:
 *   Validate sysroot evidence against target identity and required headers/libraries without owning sysroot discovery.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_SYSROOT_REQUIREMENT_H
#define UMICOM_PLATFORM_CROSS_TARGET_SYSROOT_REQUIREMENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtSysrootRequirement { char sysroot_id[UMI_CT_ID_CAPACITY]; UmiCtTarget target; bool require_headers; bool require_libraries; bool require_crt; } UmiCtSysrootRequirement;
typedef struct UmiCtSysrootEvidence { char sysroot_id[UMI_CT_ID_CAPACITY]; UmiCtTarget target; bool headers; bool libraries; bool crt; } UmiCtSysrootEvidence;
bool umi_ct_sysroot_requirement_satisfied(const UmiCtSysrootRequirement *requirement,const UmiCtSysrootEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
