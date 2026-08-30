/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/frontend.h
 *
 * PURPOSE:
 *   Describe one requested frontend and the adapter expected to render the application plan.
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

#ifndef UMICOM_DECLARATIVE_FRONTEND_H
#define UMICOM_DECLARATIVE_FRONTEND_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclFrontend { char frontend_id[UMI_DECL_ID_CAPACITY]; char adapter_id[UMI_DECL_ID_CAPACITY]; } UmiDeclFrontend;
UmiStatus umi_decl_frontend_init(UmiDeclFrontend *item,const char *left,const char *right);
int umi_decl_frontend_is_valid(const UmiDeclFrontend *item);
#ifdef __cplusplus
}
#endif
#endif
