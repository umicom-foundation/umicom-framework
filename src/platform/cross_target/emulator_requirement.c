/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/emulator_requirement.c
 *
 * PURPOSE:
 *   Describe optional emulator requirements for cross-target tests and boot validation.
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

#include "umicom/platform/cross_target/emulator_requirement.h"

#include <string.h>
/*
 * Provide the ct emulator requirement satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_emulator_requirement_satisfied(const UmiCtEmulatorRequirement*r,const UmiCtEmulatorEvidence*e){return r!=NULL&&e!=NULL&&strcmp(r->emulator_id,e->emulator_id)==0&&r->architecture==e->architecture&&e->major>=r->minimum_major&&(!r->system_mode||e->system_mode)&&(!r->user_mode||e->user_mode);}
