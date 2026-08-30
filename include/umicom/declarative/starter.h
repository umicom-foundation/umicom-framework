/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/starter.h
 *
 * PURPOSE:
 *   Describe an opinionated starter capability selected by a declarative application.
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

#ifndef UMICOM_DECLARATIVE_STARTER_H
#define UMICOM_DECLARATIVE_STARTER_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDeclStarter { char starter_id[UMI_DECL_ID_CAPACITY]; char capability_id[UMI_DECL_ID_CAPACITY]; } UmiDeclStarter;
UmiStatus umi_decl_starter_init(UmiDeclStarter *item,const char *left,const char *right);
int umi_decl_starter_is_valid(const UmiDeclStarter *item);
#ifdef __cplusplus
}
#endif
#endif
