/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/compiler.c
 *
 * PURPOSE:
 *   Coordinate schema validation and immutable plan construction for one declarative application.
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

#include "umicom/declarative/compiler.h"
/* Provide the decl compile operation used by this module and its client applications. */
UmiStatus umi_decl_compile(const UmiDeclDocument *doc,const UmiDeclSchema *schema,UmiDeclApplicationPlan *out,UmiDeclDiagnosticList *diags){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(doc==NULL||schema==NULL||out==NULL||diags==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_validate(doc,schema,diags);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_decl_plan_from_document(doc,out);}
