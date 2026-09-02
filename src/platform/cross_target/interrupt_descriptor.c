/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/interrupt_descriptor.c
 *
 * PURPOSE:
 *   Describe interrupt vectors, trigger mode and affinity hints independently of the concrete interrupt controller.
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

#include "umicom/platform/cross_target/interrupt_descriptor.h"

/*
 * Check that ct interrupt descriptor satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_interrupt_descriptor_validate(const UmiCtInterruptDescriptor*d,uint32_t max){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->vector>max||d->priority==0U||(d->trigger!=UMI_CT_IRQ_EDGE&&d->trigger!=UMI_CT_IRQ_LEVEL))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
