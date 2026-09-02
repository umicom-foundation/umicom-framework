/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/schema.c
 *
 * PURPOSE:
 *   Initialise declarative schema policy around a reusable semantic component registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/schema.h"
/*
 * Initialise decl schema from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_schema_init(UmiDeclSchema *s,UmiDeclComponentRegistry *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->version=umi_decl_version_current();s->components=c;s->allow_unknown_properties=0;return UMI_STATUS_OK;}
