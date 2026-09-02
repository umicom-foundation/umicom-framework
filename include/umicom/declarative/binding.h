/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/binding.h
 *
 * PURPOSE:
 *   Represent one toolkit-neutral data binding between application state and a component property.
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

#ifndef UMICOM_DECLARATIVE_BINDING_H
#define UMICOM_DECLARATIVE_BINDING_H
#include "umicom/declarative/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the decl binding data shared with callers of this public contract.
 */
typedef struct UmiDeclBinding { char source_expression[UMI_DECL_ID_CAPACITY]; char target_property[UMI_DECL_ID_CAPACITY]; } UmiDeclBinding;
/**
 * Initialise decl binding from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_binding_init(UmiDeclBinding *item,const char *left,const char *right);
/**
 * Check that decl binding satisfies its contract before another service relies on it.
 */
int umi_decl_binding_is_valid(const UmiDeclBinding *item);
#ifdef __cplusplus
}
#endif
#endif
