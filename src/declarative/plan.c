/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/plan.c
 *
 * PURPOSE:
 *   Create an immutable renderer-facing application plan from a validated semantic document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/plan.h"
#include <stdlib.h>
#include <string.h>
UmiStatus umi_decl_plan_from_document(const UmiDeclDocument *doc,UmiDeclApplicationPlan *out)
{
    UmiDeclDocumentSnapshot snap;
    size_t i;
    if(doc==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_decl_document_snapshot(doc,&snap)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;
    (void)memset(out,0,sizeof(*out));
    (void)umi_decl_copy_text(out->application_id,sizeof(out->application_id),snap.application_id);
    out->version=snap.version;
    out->source_revision=snap.revision;
    out->component_count=snap.node_count;
    if(snap.node_count>0U){
        out->components=(UmiDeclNode *)calloc(snap.node_count,sizeof(UmiDeclNode));
        if(out->components==NULL){out->component_count=0U;return UMI_STATUS_OUT_OF_MEMORY;}
        for(i=0U;i<snap.node_count;++i)(void)umi_decl_document_node_at(doc,i,&out->components[i]);
    }
    return UMI_STATUS_OK;
}

void umi_decl_plan_dispose(UmiDeclApplicationPlan *plan)
{
    if(plan==NULL)return;
    free(plan->components);
    (void)memset(plan,0,sizeof(*plan));
}

UmiStatus umi_decl_plan_find(const UmiDeclApplicationPlan *p,const char *id,UmiDeclNode *out){size_t i;if(p==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<p->component_count;++i){if(strcmp(p->components[i].node_id,id)==0){*out=p->components[i];return UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
