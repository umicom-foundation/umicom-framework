/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/template_store.h
 *
 * PURPOSE:
 *   Store a bounded set of named declarative templates for starters, examples and visual designer insertion.
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

#ifndef UMICOM_DECLARATIVE_TEMPLATE_STORE_H
#define UMICOM_DECLARATIVE_TEMPLATE_STORE_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the decl template store data shared with callers of this public contract.
 */
typedef struct UmiDeclTemplateStore UmiDeclTemplateStore;
/**
 * Initialise decl template store from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_decl_template_store_create(UmiDeclTemplateStore **out_store);
/**
 * Release or reset state held by decl template store so the same storage can be reused
 * safely.
 */
void umi_decl_template_store_destroy(UmiDeclTemplateStore *store);
/**
 * Provide the decl template store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_put(UmiDeclTemplateStore *store,const char *template_id,const char *source);
/**
 * Provide the decl template store get operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_template_store_get(const UmiDeclTemplateStore *store,const char *template_id,const char **out_source);
/**
 * Return the number of records represented by decl template store without changing their
 * state.
 */
size_t umi_decl_template_store_count(const UmiDeclTemplateStore *store);
#ifdef __cplusplus
}
#endif
#endif
