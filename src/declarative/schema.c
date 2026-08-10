/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/schema.c
 *
 * PURPOSE:
 *   Initialise declarative schema policy around a reusable semantic component registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/schema.h"
UmiStatus umi_decl_schema_init(UmiDeclSchema *s,UmiDeclComponentRegistry *c){if(s==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->version=umi_decl_version_current();s->components=c;s->allow_unknown_properties=0;return UMI_STATUS_OK;}
