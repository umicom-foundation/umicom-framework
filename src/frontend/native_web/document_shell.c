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
UmiStatus umi_native_web_document_shell_build(const UmiNativeWebRenderContext *context,const char *title,const char *body_markup,const char *css,const char *runtime_js,UmiNativeWebMarkupBuffer *out_document){UmiStatus s;if(umi_native_web_render_context_validate(context)!=UMI_STATUS_OK||title==NULL||body_markup==NULL||css==NULL||runtime_js==NULL||out_document==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_native_web_markup_buffer_init(out_document);s=umi_native_web_markup_buffer_append(out_document,"<!doctype html><html lang=\"");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_attribute(out_document,context->locale);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_html_escape_text(out_document,title);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</title><style>");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,css);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</style></head><body><main id=\"umicom-root\">");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,body_markup);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,"</main><script>");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_document,runtime_js);if(s!=UMI_STATUS_OK)return s;return umi_native_web_markup_buffer_append(out_document,"</script></body></html>");}

