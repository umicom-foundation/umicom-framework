/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/catalogue.h
 *
 * PURPOSE:
 *   Populate the default semantic component catalogue shared by Studio, Designer and generated applications.
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

#ifndef UMICOM_DECLARATIVE_CATALOGUE_H
#define UMICOM_DECLARATIVE_CATALOGUE_H
#include "umicom/declarative/component_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_decl_catalogue_register_defaults(UmiDeclComponentRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
