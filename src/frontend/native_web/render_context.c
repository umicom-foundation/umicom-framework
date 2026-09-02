/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_context.c
 *
 * PURPOSE:
 *   Carry session, route, theme, density, locale and revision state through web rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/render_context.h"

#include <string.h>
/*
 * Initialise native web render context from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_native_web_render_context_init(UmiNativeWebRenderContext *context,const char *session_id,const char *route){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(context==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(context,0,sizeof(*context));s=umi_native_web_copy_text(context->session_id,sizeof(context->session_id),session_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(context->route,sizeof(context->route),route);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;(void)umi_native_web_copy_text(context->theme,sizeof(context->theme),"system");(void)umi_native_web_copy_text(context->density,sizeof(context->density),"compact");(void)umi_native_web_copy_text(context->locale,sizeof(context->locale),"en-GB");context->revision=1U;return UMI_STATUS_OK;}
/*
 * Check that native web render context satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_native_web_render_context_validate(const UmiNativeWebRenderContext *context){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(context==NULL||context->session_id[0]=='\0'||context->route[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}

