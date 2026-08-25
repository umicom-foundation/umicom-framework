/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/clock_semantics.c
 *
 * PURPOSE:
 *   Describe monotonic/wall-clock availability and timer resolution for scheduler and profiling portability.
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

#include "umicom/platform/cross_target/clock_semantics.h"

UmiStatus umi_ct_clock_semantics_validate(const UmiCtClockSemantics*s){if(s==NULL||s->frequency_hz==0U||s->resolution_ns==0U)return UMI_STATUS_INVALID_ARGUMENT;if(!s->monotonic)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
uint64_t umi_ct_clock_ticks_to_ns(const UmiCtClockSemantics*s,uint64_t t){if(s==NULL||s->frequency_hz==0U)return 0U;return (t/ s->frequency_hz)*UINT64_C(1000000000)+(t % s->frequency_hz)*UINT64_C(1000000000)/s->frequency_hz;}
