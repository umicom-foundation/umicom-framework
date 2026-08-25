/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_context.c
 *
 * PURPOSE:
 *   Carry session, route, theme, density, locale and revision state through web rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/render_context.h"

#include <string.h>
UmiStatus umi_native_web_render_context_init(UmiNativeWebRenderContext *context,const char *session_id,const char *route){UmiStatus s;if(context==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(context,0,sizeof(*context));s=umi_native_web_copy_text(context->session_id,sizeof(context->session_id),session_id);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(context->route,sizeof(context->route),route);if(s!=UMI_STATUS_OK)return s;(void)umi_native_web_copy_text(context->theme,sizeof(context->theme),"system");(void)umi_native_web_copy_text(context->density,sizeof(context->density),"compact");(void)umi_native_web_copy_text(context->locale,sizeof(context->locale),"en-GB");context->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_native_web_render_context_validate(const UmiNativeWebRenderContext *context){if(context==NULL||context->session_id[0]=='\0'||context->route[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

