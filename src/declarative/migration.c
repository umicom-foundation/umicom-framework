/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/migration.c
 *
 * PURPOSE:
 *   Apply explicit version migrations without hiding source compatibility decisions.
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

#include "umicom/declarative/migration.h"
/*
 * Provide the decl migrate to current operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_migrate_to_current(UmiDeclDocument *doc,size_t *out_steps){UmiDeclDocumentSnapshot snap;UmiDeclVersion current=umi_decl_version_current();/* Protect caller-owned memory by checking that required state is available before it is used. */ if(doc==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_decl_document_snapshot(doc,&snap)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(snap.version.major>current.major)return UMI_STATUS_UNAVAILABLE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_steps!=NULL)*out_steps=umi_decl_version_equal(snap.version,current)?0U:1U;return umi_decl_document_set_version(doc,current);}
