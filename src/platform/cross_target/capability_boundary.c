/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/capability_boundary.c
 *
 * PURPOSE:
 *   Model capability-based kernel/service admission independently of application-specific permissions.
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

#include "umicom/platform/cross_target/capability_boundary.h"

bool umi_ct_capability_boundary_allows(const UmiCtCapabilityBoundary*b,UmiCtKernelCapability g){return b!=NULL&&(g&b->required)==b->required&&(g&b->denied)==0U;}
