/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/release_gate.c
 *
 * PURPOSE:
 *   release decision combining signature, checksum, compatibility and test evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_gate.h"

bool umi_dr_release_gate_pass(const UmiDrReleaseGateInput*i){return i!=NULL&&i->signatures_ok&&i->checksums_ok&&i->compatibility_ok&&i->tests_ok&&i->frontend_conformance_ok&&i->blockers==0U;}
