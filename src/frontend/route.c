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

/* BEGINNER NOTE:
 * The implementation turns semantic frontend information into portable output while keeping product state outside any specific UI toolkit.
 */

#include "umicom/frontend/route.h"
#include "umicom/web/types.h"
#include <string.h>
UmiStatus umi_frontend_route_init(UmiFrontendRoute *r,const char *path,const char *page){UmiStatus s;if(r==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(r,0,sizeof(*r));s=umi_web_copy_text(r->path,sizeof(r->path),path);if(s==UMI_STATUS_OK)s=umi_web_copy_text(r->page_id,sizeof(r->page_id),page);return s;}
