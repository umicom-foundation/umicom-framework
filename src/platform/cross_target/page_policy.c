/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/page_policy.c
 *
 * PURPOSE:
 *   Provide target page alignment and page-count calculations for virtual-memory and boot mapping plans.
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

#include "umicom/platform/cross_target/page_policy.h"

UmiStatus umi_ct_page_policy_validate(const UmiCtPagePolicy*p){if(p==NULL||p->base_page_size<4096U||(p->base_page_size&(p->base_page_size-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;if(p->huge_pages&&(p->huge_page_size<p->base_page_size||(p->huge_page_size&p->base_page_size)!=0U))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
uint64_t umi_ct_page_align_up(const UmiCtPagePolicy*p,uint64_t a){uint64_t s=p?p->base_page_size:1U;return ((a+s-1U)/s)*s;}
uint64_t umi_ct_page_count(const UmiCtPagePolicy*p,uint64_t b){uint64_t s=p?p->base_page_size:1U;return b==0U?0U:(b+s-1U)/s;}
