/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/declarative/schema.h
 *
 * PURPOSE:
 *   Bind a declarative language version to a component registry and validation policy.
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

#ifndef UMICOM_DECLARATIVE_SCHEMA_H
#define UMICOM_DECLARATIVE_SCHEMA_H
#include "umicom/declarative/component_registry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the decl schema data shared with callers of this public contract.
 */
typedef struct UmiDeclSchema { UmiDeclVersion version; UmiDeclComponentRegistry *components; int allow_unknown_properties; } UmiDeclSchema;
/**
 * Initialise decl schema from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_schema_init(UmiDeclSchema *schema,UmiDeclComponentRegistry *components);
#ifdef __cplusplus
}
#endif
#endif
