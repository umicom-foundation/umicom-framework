/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/renderer.h
 *
 * PURPOSE:
 *   Define the frontend-neutral renderer contract used by GTK4, web and deterministic headless adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the decl renderer data shared with callers of this public contract.
 */
typedef struct UmiDeclRenderer { uint32_t structure_size; uint32_t abi_version; void *instance; UmiStatus (*begin)(void *,const UmiDeclApplicationPlan *); UmiStatus (*component)(void *,const UmiDeclNode *); UmiStatus (*end)(void *); void (*destroy)(void *); } UmiDeclRenderer;
/**
 * Provide the decl render operation used by this module and its client applications.
 */
UmiStatus umi_decl_render(const UmiDeclApplicationPlan *plan,const UmiDeclRenderer *renderer);
/**
 * Provide the decl headless render operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_headless_render(const UmiDeclApplicationPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
