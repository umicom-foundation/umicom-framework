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

/*
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
/**
 * Represent the decl starter data shared with callers of this public contract.
 */
typedef struct UmiDeclStarter { char starter_id[UMI_DECL_ID_CAPACITY]; char capability_id[UMI_DECL_ID_CAPACITY]; } UmiDeclStarter;
/**
 * Initialise decl starter from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_starter_init(UmiDeclStarter *item,const char *left,const char *right);
/**
 * Check that decl starter satisfies its contract before another service relies on it.
 */
int umi_decl_starter_is_valid(const UmiDeclStarter *item);
#ifdef __cplusplus
}
#endif
#endif
