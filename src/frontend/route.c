/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/route.c
 *
 * PURPOSE:
 *   Map a URL path to a semantic page.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/route.h"
#include "../base/value_construction_internal.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise frontend route from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_frontend_route_init(UmiFrontendRoute *r,const char *path,const char *page){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_web_copy_text(r->path,sizeof(r->path),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(r->page_id,sizeof(r->page_id),page);return s;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_frontend_route_init_build_value(UmiFrontendRoute *r,const char *path,const char *page){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_web_copy_text(r->path,sizeof(r->path),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(r->page_id,sizeof(r->page_id),page);return s;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_frontend_route_init, UmiFrontendRoute, r,
    (UmiFrontendRoute *r, const char *path, const char *page),
    umi_frontend_route_init_build_value(candidate, path, page))
