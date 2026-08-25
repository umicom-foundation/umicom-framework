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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/sysroot_requirement.h"
#include "umicom/platform/cross_target/target_triple.h"

#include <string.h>
bool umi_ct_sysroot_requirement_satisfied(const UmiCtSysrootRequirement*r,const UmiCtSysrootEvidence*e){if(r==NULL||e==NULL||strcmp(r->sysroot_id,e->sysroot_id)!=0||!umi_ct_target_compatible(&r->target,&e->target))return false;if(r->require_headers&&!e->headers)return false;if(r->require_libraries&&!e->libraries)return false;if(r->require_crt&&!e->crt)return false;return true;}
