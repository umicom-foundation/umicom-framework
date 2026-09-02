/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/surface_requirement.c
 *
 * PURPOSE:
 *   required and optional semantic capability requirements for a Framework UI surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_requirement.h"

/*
 * Provide the fc surface requirement make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_surface_requirement_make(const char *surface_id,uint64_t required,uint64_t optional,bool allow_degraded,UmiFcSurfaceRequirement *out_requirement){ UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_requirement==NULL)return UMI_STATUS_INVALID_ARGUMENT; *out_requirement=(UmiFcSurfaceRequirement){0}; st=umi_fc_copy_text(out_requirement->surface_id,sizeof(out_requirement->surface_id),surface_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; out_requirement->required=required; out_requirement->optional=optional; out_requirement->allow_degraded=allow_degraded; return UMI_STATUS_OK; }
/*
 * Provide the fc surface requirement missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_surface_requirement_missing(const UmiFcSurfaceRequirement *requirement,uint64_t actual){ return requirement==NULL?0U:requirement->required & ~actual; }
