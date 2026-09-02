/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/preview_session.c
 *
 * PURPOSE:
 *   Implement the preview session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/preview_session.h"
#include <string.h>
/*
 * Initialise frontend preview session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_preview_session_init(UmiFrontendPreviewSession *s,const char *id,const char *url,const UmiFrontendDevicePreset *d){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL||url==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));st=umi_frontend_dev_copy_text(s->id,sizeof(s->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)st=umi_frontend_dev_copy_text(s->url,sizeof(s->url),url);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;s->device=*d;s->connected=1;s->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend preview session navigate operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_preview_session_navigate(UmiFrontendPreviewSession *s,const char *url){UmiStatus st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_frontend_dev_copy_text(s->url,sizeof(s->url),url);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)s->revision+=1U;return st;}
/*
 * Provide the frontend preview session reload operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_preview_session_reload(UmiFrontendPreviewSession *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->connected)return UMI_STATUS_INVALID_STATE;s->reload_count+=1U;s->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the frontend preview session set device operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_preview_session_set_device(UmiFrontendPreviewSession *s,const UmiFrontendDevicePreset *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->device=*d;s->revision+=1U;return UMI_STATUS_OK;}
