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
/*
 * Initialise frontend page from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_frontend_page_init(UmiFrontendPage *p,const char *id,const char *title,const char *component){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));s=umi_web_copy_text(p->id,sizeof(p->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->title,sizeof(p->title),title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->component_id,sizeof(p->component_id),component);return s;}
