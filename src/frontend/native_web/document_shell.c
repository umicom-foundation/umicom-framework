/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/document_shell.c
 *
 * PURPOSE:
 *   Generate a complete accessible HTML document shell for a server-driven Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/document_shell.h"

#include "umicom/frontend/native_web/html_escape.h"
/*
 * Provide the native web document shell build operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_document_shell_build(const UmiNativeWebRenderContext *context,const char *title,const char *body_markup,const char *css,const char *runtime_js,UmiNativeWebMarkupBuffer *out_document){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK||title==NULL||body_markup==NULL||css==NULL||runtime_js==NULL||out_document==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_markup_buffer_init(out_document);s=umi_native_web_markup_buffer_append(out_document,"<!doctype html><html lang=\"");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(out_document,context->locale);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(out_document,title);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</title><style>");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,css);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</style></head><body><main id=\"umicom-root\">");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,body_markup);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</main><script>");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,runtime_js);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_native_web_markup_buffer_append(out_document,"</script></body></html>");}

