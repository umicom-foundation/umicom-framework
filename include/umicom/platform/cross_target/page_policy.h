/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/page_policy.h
 *
 * PURPOSE:
 *   Provide target page alignment and page-count calculations for virtual-memory and boot mapping plans.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_PAGE_POLICY_H
#define UMICOM_PLATFORM_CROSS_TARGET_PAGE_POLICY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtPagePolicy { uint32_t base_page_size; uint32_t huge_page_size; bool huge_pages; bool execute_never; } UmiCtPagePolicy;
UmiStatus umi_ct_page_policy_validate(const UmiCtPagePolicy *policy);
uint64_t umi_ct_page_align_up(const UmiCtPagePolicy *policy,uint64_t address);
uint64_t umi_ct_page_count(const UmiCtPagePolicy *policy,uint64_t bytes);

#ifdef __cplusplus
}
#endif

#endif
