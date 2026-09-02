/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/sysroot_requirement.c
 *
 * PURPOSE:
 *   Validate sysroot evidence against target identity and required headers/libraries without owning sysroot discovery.
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

#include "umicom/platform/cross_target/sysroot_requirement.h"
#include "umicom/platform/cross_target/target_triple.h"

#include <string.h>
/*
 * Provide the ct sysroot requirement satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_sysroot_requirement_satisfied(const UmiCtSysrootRequirement*r,const UmiCtSysrootEvidence*e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||e==NULL||strcmp(r->sysroot_id,e->sysroot_id)!=0||!umi_ct_target_compatible(&r->target,&e->target))return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->require_headers&&!e->headers)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->require_libraries&&!e->libraries)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->require_crt&&!e->crt)return false;return true;}
