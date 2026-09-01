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
UmiStatus umi_decl_migrate_to_current(UmiDeclDocument *doc,size_t *out_steps){UmiDeclDocumentSnapshot snap;UmiDeclVersion current=umi_decl_version_current();if(doc==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_decl_document_snapshot(doc,&snap)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;if(snap.version.major>current.major)return UMI_STATUS_UNAVAILABLE;if(out_steps!=NULL)*out_steps=umi_decl_version_equal(snap.version,current)?0U:1U;return umi_decl_document_set_version(doc,current);}
