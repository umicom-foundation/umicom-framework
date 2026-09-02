/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/boot_phase.c
 *
 * PURPOSE:
 *   Define deterministic Umicom OS boot phases and legal phase progression.
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

#include "umicom/platform/cross_target/boot_phase.h"

/*
 * Provide the ct boot phase text operation used by this module and its client
 * applications.
 */
const char*umi_ct_boot_phase_text(UmiCtBootPhase p){static const char*n[]={"reset","early","memory","interrupts","devices","services","userspace","ready"};return (unsigned)p<8U?n[(unsigned)p]:"invalid";}
/*
 * Check that ct boot phase transition satisfies its contract before another service relies
 * on it.
 */
bool umi_ct_boot_phase_transition_valid(UmiCtBootPhase a,UmiCtBootPhase b){return (unsigned)a<8U&&(unsigned)b<8U&&((unsigned)b==(unsigned)a+1U||(a==b));}
