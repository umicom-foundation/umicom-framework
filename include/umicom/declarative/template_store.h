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
typedef struct UmiDeclTemplateStore UmiDeclTemplateStore;
UmiStatus umi_decl_template_store_create(UmiDeclTemplateStore **out_store);
void umi_decl_template_store_destroy(UmiDeclTemplateStore *store);
UmiStatus umi_decl_template_store_put(UmiDeclTemplateStore *store,const char *template_id,const char *source);
UmiStatus umi_decl_template_store_get(const UmiDeclTemplateStore *store,const char *template_id,const char **out_source);
size_t umi_decl_template_store_count(const UmiDeclTemplateStore *store);
#ifdef __cplusplus
}
#endif
#endif
