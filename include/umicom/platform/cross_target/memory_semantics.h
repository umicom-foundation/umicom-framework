/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/memory_semantics.h
 *
 * PURPOSE:
 *   Describe virtual memory, guard page and executable mapping capabilities without embedding OS APIs in applications.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_MEMORY_SEMANTICS_H
#define UMICOM_PLATFORM_CROSS_TARGET_MEMORY_SEMANTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtMemorySemantics { uint32_t page_size; uint32_t allocation_granularity; bool virtual_memory; bool guard_pages; bool executable_memory; bool huge_pages; } UmiCtMemorySemantics;
UmiStatus umi_ct_memory_semantics_validate(const UmiCtMemorySemantics *semantics);
uint64_t umi_ct_memory_round_up(const UmiCtMemorySemantics *semantics,uint64_t bytes);

#ifdef __cplusplus
}
#endif

#endif
