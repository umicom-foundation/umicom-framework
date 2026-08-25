/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/toolchain_requirement.c
 *
 * PURPOSE:
 *   Describe the validated external toolchain capabilities required by a target without reimplementing tool discovery.
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

#include "umicom/platform/cross_target/toolchain_requirement.h"

#include <string.h>
bool umi_ct_toolchain_requirement_satisfied(const UmiCtToolchainRequirement*r,const UmiCtToolchainEvidence*e){if(r==NULL||e==NULL||strcmp(r->family,e->family)!=0)return false;if(e->major<r->minimum_major||(e->major==r->minimum_major&&e->minor<r->minimum_minor))return false;if(r->compiler&&!e->compiler)return false;if(r->assembler&&!e->assembler)return false;if(r->linker&&!e->linker)return false;if(r->debugger&&!e->debugger)return false;return true;}
