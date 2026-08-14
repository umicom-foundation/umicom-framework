/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/renderer.h
 *
 * PURPOSE:
 *   Define the frontend-neutral renderer contract used by GTK4, web and deterministic headless adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_RENDERER_H
#define UMICOM_DECLARATIVE_RENDERER_H
#include "umicom/declarative/plan.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclRenderer { uint32_t structure_size; uint32_t abi_version; void *instance; UmiStatus (*begin)(void *,const UmiDeclApplicationPlan *); UmiStatus (*component)(void *,const UmiDeclNode *); UmiStatus (*end)(void *); void (*destroy)(void *); } UmiDeclRenderer;
UmiStatus umi_decl_render(const UmiDeclApplicationPlan *plan,const UmiDeclRenderer *renderer);
UmiStatus umi_decl_headless_render(const UmiDeclApplicationPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
