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
#include "../base/value_construction_internal.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise frontend page from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_frontend_page_init(UmiFrontendPage *p,const char *id,const char *title,const char *component){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));s=umi_web_copy_text(p->id,sizeof(p->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->title,sizeof(p->title),title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->component_id,sizeof(p->component_id),component);return s;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_frontend_page_init_build_value(UmiFrontendPage *p,const char *id,const char *title,const char *component){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));s=umi_web_copy_text(p->id,sizeof(p->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->title,sizeof(p->title),title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_web_copy_text(p->component_id,sizeof(p->component_id),component);return s;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_frontend_page_init, UmiFrontendPage, p,
    (UmiFrontendPage *p, const char *id, const char *title, const char *component),
    umi_frontend_page_init_build_value(candidate, id, title, component))
