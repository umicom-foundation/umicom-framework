/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/static_files.c
 *
 * PURPOSE:
 *   Implement safe static-file loading below a configured root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/static_files.h"
#include "umicom/web/mime.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_web_static_files_init(UmiWebStaticFiles *files,const char *root){if(files==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_web_copy_text(files->root,sizeof(files->root),root);}
UmiStatus umi_web_static_files_serve(const UmiWebStaticFiles *files,const char *request_path,UmiWebResponse *response){char full[UMI_WEB_PATH_CAPACITY*2U];FILE *f;size_t n;if(files==NULL||request_path==NULL||response==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(strstr(request_path,"..")!=NULL)return umi_web_response_set_text(response,UMI_HTTP_STATUS_FORBIDDEN,"text/plain","Forbidden");if(snprintf(full,sizeof(full),"%s/%s",files->root,request_path[0]=='/'?request_path+1:request_path)<0)return UMI_STATUS_INTERNAL_ERROR;f=fopen(full,"rb");if(f==NULL)return umi_web_response_set_text(response,UMI_HTTP_STATUS_NOT_FOUND,"text/plain","Not Found");n=fread(response->body,1U,sizeof(response->body)-1U,f);(void)fclose(f);response->body[n]='\0';response->body_length=n;response->status=UMI_HTTP_STATUS_OK;response->header_count=0U;return umi_web_response_set_header(response,"Content-Type",umi_web_mime_from_path(full));}
