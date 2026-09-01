/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/page.c
 *
 * PURPOSE:
 *   Describe one semantic frontend page.
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

#include "umicom/frontend/page.h"
#include "umicom/web/types.h"
#include <string.h>
UmiStatus umi_frontend_page_init(UmiFrontendPage *p,const char *id,const char *title,const char *component){UmiStatus s;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));s=umi_web_copy_text(p->id,sizeof(p->id),id);if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->title,sizeof(p->title),title);if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->component_id,sizeof(p->component_id),component);return s;}
