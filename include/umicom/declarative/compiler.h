/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/compiler.h
 *
 * PURPOSE:
 *   Compile a semantic document into a validated immutable application plan.
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

#ifndef UMICOM_DECLARATIVE_COMPILER_H
#define UMICOM_DECLARATIVE_COMPILER_H
#include "umicom/declarative/plan.h"
#include "umicom/declarative/validator.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the decl compile operation used by this module and its client applications.
 */
UmiStatus umi_decl_compile(const UmiDeclDocument *document,const UmiDeclSchema *schema,UmiDeclApplicationPlan *out_plan,UmiDeclDiagnosticList *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
