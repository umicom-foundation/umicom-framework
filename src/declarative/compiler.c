/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/compiler.c
 *
 * PURPOSE:
 *   Coordinate schema validation and immutable plan construction for one declarative application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/compiler.h"
UmiStatus umi_decl_compile(const UmiDeclDocument *doc,const UmiDeclSchema *schema,UmiDeclApplicationPlan *out,UmiDeclDiagnosticList *diags){UmiStatus s;if(doc==NULL||schema==NULL||out==NULL||diags==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_validate(doc,schema,diags);if(s!=UMI_STATUS_OK)return s;return umi_decl_plan_from_document(doc,out);}
