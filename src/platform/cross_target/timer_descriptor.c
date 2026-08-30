/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/timer_descriptor.c
 *
 * PURPOSE:
 *   Describe platform timer frequency, width and monotonic/oneshot properties for scheduler portability.
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

#include "umicom/platform/cross_target/timer_descriptor.h"

UmiStatus umi_ct_timer_descriptor_validate(const UmiCtTimerDescriptor*d){if(d==NULL||!umi_ct_id_valid(d->timer_id)||d->frequency_hz==0U||(d->counter_bits!=32U&&d->counter_bits!=64U)||!d->monotonic)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
uint64_t umi_ct_timer_ns_to_ticks(const UmiCtTimerDescriptor*d,uint64_t ns){if(d==NULL||d->frequency_hz==0U)return 0U;return (ns/UINT64_C(1000000000))*d->frequency_hz+(ns%UINT64_C(1000000000))*d->frequency_hz/UINT64_C(1000000000);}
