/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/render_result.c
 *
 * PURPOSE:
 *   Accumulate HTML, CSS and patch output plus render revision/evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/render_result.h"

/*
 * Initialise native web render result from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_render_result_init(UmiNativeWebRenderResult *result,uint64_t revision){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL)return;umi_native_web_markup_buffer_init(&result->markup);umi_native_web_css_buffer_init(&result->css);result->revision=revision;result->rendered_elements=0U;}
/*
 * Provide the native web render result note element operation used by this module and its
 * client applications.
 */
void umi_native_web_render_result_note_element(UmiNativeWebRenderResult *result){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(result!=NULL)++result->rendered_elements;}

