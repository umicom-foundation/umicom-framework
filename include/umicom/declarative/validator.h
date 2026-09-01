/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/validator.h
 *
 * PURPOSE:
 *   Validate semantic application documents against component schemas, parent relationships and required properties.
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

#ifndef UMICOM_DECLARATIVE_VALIDATOR_H
#define UMICOM_DECLARATIVE_VALIDATOR_H
#include "umicom/declarative/diagnostic.h"
#include "umicom/declarative/document.h"
#include "umicom/declarative/schema.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_decl_validate(const UmiDeclDocument *document,const UmiDeclSchema *schema,UmiDeclDiagnosticList *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
