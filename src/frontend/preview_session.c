/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/frontend/preview_session.h"
#include <string.h>
UmiStatus umi_frontend_preview_session_init(UmiFrontendPreviewSession *s,const char *id,const char *url,const UmiFrontendDevicePreset *d){UmiStatus st;if(s==NULL||id==NULL||url==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));st=umi_frontend_dev_copy_text(s->id,sizeof(s->id),id);if(st==UMI_STATUS_OK)st=umi_frontend_dev_copy_text(s->url,sizeof(s->url),url);if(st!=UMI_STATUS_OK)return st;s->device=*d;s->connected=1;s->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_preview_session_navigate(UmiFrontendPreviewSession *s,const char *url){UmiStatus st;if(s==NULL||url==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_frontend_dev_copy_text(s->url,sizeof(s->url),url);if(st==UMI_STATUS_OK)s->revision+=1U;return st;}
UmiStatus umi_frontend_preview_session_reload(UmiFrontendPreviewSession *s){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(!s->connected)return UMI_STATUS_INVALID_STATE;s->reload_count+=1U;s->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_frontend_preview_session_set_device(UmiFrontendPreviewSession *s,const UmiFrontendDevicePreset *d){if(s==NULL||d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->device=*d;s->revision+=1U;return UMI_STATUS_OK;}
