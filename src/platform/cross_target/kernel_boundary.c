/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/kernel_boundary.c
 *
 * PURPOSE:
 *   Define explicit user/kernel/hypervisor ownership boundaries for Umicom OS reusable services.
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

#include "umicom/platform/cross_target/kernel_boundary.h"

/*
 * Check that ct kernel boundary satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_kernel_boundary_validate(const UmiCtKernelBoundary*b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||!umi_ct_id_valid(b->boundary_id)||b->caller<UMI_CT_DOMAIN_USER||b->caller>UMI_CT_DOMAIN_FIRMWARE||b->callee<UMI_CT_DOMAIN_USER||b->callee>UMI_CT_DOMAIN_FIRMWARE)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->caller==UMI_CT_DOMAIN_USER&&b->callee==UMI_CT_DOMAIN_KERNEL&&!b->privileged)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
