/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/generator.h
 *
 * PURPOSE:
 *   Generate small C23 application and CMake entry-point source from an immutable declarative application plan.
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

#ifndef UMICOM_DECLARATIVE_GENERATOR_H
#define UMICOM_DECLARATIVE_GENERATOR_H
#include "umicom/declarative/plan.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_decl_generate_c_main(const UmiDeclApplicationPlan *plan,char *out_text,size_t capacity);
UmiStatus umi_decl_generate_cmake(const UmiDeclApplicationPlan *plan,char *out_text,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
