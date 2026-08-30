/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/plan.h
 *
 * PURPOSE:
 *   Define the immutable application plan produced after declarative validation and used by frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The declarations below describe semantic application data and behaviour.
 * They deliberately avoid GUI-toolkit types so the same contract can be used
 * by GTK4, web, headless tests and future frontend adapters.
 */

#ifndef UMICOM_DECLARATIVE_PLAN_H
#define UMICOM_DECLARATIVE_PLAN_H
#include "umicom/declarative/document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclApplicationPlan { char application_id[UMI_DECL_ID_CAPACITY]; UmiDeclVersion version; UmiDeclNode *components; size_t component_count; uint64_t source_revision; } UmiDeclApplicationPlan;
UmiStatus umi_decl_plan_from_document(const UmiDeclDocument *document,UmiDeclApplicationPlan *out_plan);
void umi_decl_plan_dispose(UmiDeclApplicationPlan *plan);
UmiStatus umi_decl_plan_find(const UmiDeclApplicationPlan *plan,const char *node_id,UmiDeclNode *out_node);
#ifdef __cplusplus
}
#endif
#endif
