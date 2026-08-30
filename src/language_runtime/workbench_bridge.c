/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/workbench_bridge.c
 *
 * PURPOSE:
 *   Implement real Navigate command handlers backed by the language runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workbench_bridge.h"
#include <stdio.h>
#include <stdlib.h>
struct UmiLanguageRuntimeWorkbenchBridge{UmiDeveloperWorkbench*w;UmiLanguageRuntimePlatform*p;UmiLanguageRuntimeWorkbenchContext c;};
static int docc(void*u,const char*a){UmiLanguageRuntimeWorkbenchBridge*b=u;(void)a;return b&&b->c.document_id[0];}static int workc(void*u,const char*a){UmiLanguageRuntimeWorkbenchBridge*b=u;return b&&b->c.language_id[0]&&b->c.root_uri[0]&&a!=NULL;}
static UmiStatus def(void*u,const char*a,char*out,size_t cap){UmiLanguageRuntimeWorkbenchBridge*b=u;UmiStatus q;(void)a;q=umi_language_runtime_platform_request_definition(b->p,b->c.document_id,b->c.symbol_id,b->c.line,b->c.column);if(out&&cap)snprintf(out,cap,"%s",q==UMI_STATUS_OK?"Definition request sent.":"Definition request failed.");return q;}
static UmiStatus refs(void*u,const char*a,char*out,size_t cap){UmiLanguageRuntimeWorkbenchBridge*b=u;UmiStatus q;(void)a;q=umi_language_runtime_platform_request_references(b->p,b->c.document_id,b->c.symbol_id,b->c.line,b->c.column,1);if(out&&cap)snprintf(out,cap,"%s",q==UMI_STATUS_OK?"References request sent.":"References request failed.");return q;}
static UmiStatus syms(void*u,const char*a,char*out,size_t cap){UmiLanguageRuntimeWorkbenchBridge*b=u;UmiStatus q;q=umi_language_runtime_platform_request_workspace_symbols(b->p,b->c.language_id,b->c.root_uri,a?a:"");if(out&&cap)snprintf(out,cap,"%s",q==UMI_STATUS_OK?"Workspace symbol request sent.":"Workspace symbol request failed.");return q;}
UmiStatus umi_language_runtime_workbench_bridge_create(UmiDeveloperWorkbench*w,UmiLanguageRuntimePlatform*p,UmiLanguageRuntimeWorkbenchBridge**out){UmiLanguageRuntimeWorkbenchBridge*b;if(!w||!p||!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;b=calloc(1,sizeof(*b));if(!b)return UMI_STATUS_OUT_OF_MEMORY;b->w=w;b->p=p;*out=b;return UMI_STATUS_OK;}void umi_language_runtime_workbench_bridge_destroy(UmiLanguageRuntimeWorkbenchBridge*b){free(b);}UmiStatus umi_language_runtime_workbench_bridge_set_context(UmiLanguageRuntimeWorkbenchBridge*b,const UmiLanguageRuntimeWorkbenchContext*c){if(!b||!c)return UMI_STATUS_INVALID_ARGUMENT;b->c=*c;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_workbench_bridge_bind(UmiLanguageRuntimeWorkbenchBridge*b){UmiStatus q;if(!b)return UMI_STATUS_INVALID_ARGUMENT;q=umi_developer_workbench_bind_action(b->w,"navigate.definition",def,docc,b);if(q!=UMI_STATUS_OK)return q;q=umi_developer_workbench_bind_action(b->w,"navigate.references",refs,docc,b);if(q!=UMI_STATUS_OK)return q;return umi_developer_workbench_bind_action(b->w,"navigate.symbol",syms,workc,b);}
