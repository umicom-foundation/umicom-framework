/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/gtkjs.c
 *
 * PURPOSE:
 *   Generate a gtk-js/React-oriented frontend project from the shared plan.
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

#include "umicom/frontend/gtkjs.h"
#include "umicom/frontend/generator.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct GtkJsRenderer{int marker;}GtkJsRenderer;
static UmiStatus render(void *instance,const UmiFrontendPlan *p,const char *root){char path[1024],html[4096],manifest[2048];FILE *f;(void)instance;if(p==NULL||root==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_frontend_generate_html(p,html,sizeof(html))!=UMI_STATUS_OK||umi_frontend_generate_manifest(p,manifest,sizeof(manifest))!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;if(snprintf(path,sizeof(path),"%s/index.html",root)<0)return UMI_STATUS_INTERNAL_ERROR;f=fopen(path,"wb");if(f==NULL)return UMI_STATUS_IO_ERROR;(void)fputs(html,f);(void)fclose(f);if(snprintf(path,sizeof(path),"%s/umicom.frontend.json",root)<0)return UMI_STATUS_INTERNAL_ERROR;f=fopen(path,"wb");if(f==NULL)return UMI_STATUS_IO_ERROR;(void)fputs(manifest,f);(void)fclose(f);return UMI_STATUS_OK;}
static void destroy(void *p){free(p);}
UmiStatus umi_frontend_gtkjs_create(UmiFrontendRenderer *out){GtkJsRenderer *r;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;r=(GtkJsRenderer*)calloc(1U,sizeof(*r));if(r==NULL)return UMI_STATUS_OUT_OF_MEMORY;r->marker=1;out->instance=r;out->kind=UMI_FRONTEND_KIND_WEB;out->render=render;out->destroy=destroy;return UMI_STATUS_OK;}
