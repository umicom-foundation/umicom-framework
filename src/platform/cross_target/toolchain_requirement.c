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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/toolchain_requirement.h"

#include <string.h>
/*
 * Provide the ct toolchain requirement satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_toolchain_requirement_satisfied(const UmiCtToolchainRequirement*r,const UmiCtToolchainEvidence*e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||e==NULL||strcmp(r->family,e->family)!=0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(e->major<r->minimum_major||(e->major==r->minimum_major&&e->minor<r->minimum_minor))return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->compiler&&!e->compiler)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->assembler&&!e->assembler)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->linker&&!e->linker)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->debugger&&!e->debugger)return false;return true;}
