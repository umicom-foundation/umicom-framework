/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/memory_semantics.c
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

#include "umicom/platform/cross_target/memory_semantics.h"

UmiStatus umi_ct_memory_semantics_validate(const UmiCtMemorySemantics*s){if(s==NULL||s->page_size==0U||(s->page_size&(s->page_size-1U))!=0U||s->allocation_granularity<s->page_size)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
uint64_t umi_ct_memory_round_up(const UmiCtMemorySemantics*s,uint64_t b){uint64_t g;if(s==NULL||s->allocation_granularity==0U)return b;g=s->allocation_granularity;return b==0U?0U:((b+g-1U)/g)*g;}
