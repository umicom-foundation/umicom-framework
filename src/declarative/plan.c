/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/plan.c
 *
 * PURPOSE:
 *   Create an immutable renderer-facing application plan from a validated semantic document.
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

#include "umicom/declarative/plan.h"
#include <stdlib.h>
#include <string.h>
/*
 * Provide the decl plan from document operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_plan_from_document(const UmiDeclDocument *doc,UmiDeclApplicationPlan *out)
{
    UmiDeclDocumentSnapshot snap;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(doc==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_decl_document_snapshot(doc,&snap)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_STATE;
    (void)memset(out,0,sizeof(*out));
    (void)umi_decl_copy_text(out->application_id,sizeof(out->application_id),snap.application_id);
    out->version=snap.version;
    out->source_revision=snap.revision;
    out->component_count=snap.node_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if(snap.node_count>0U){
        out->components=(UmiDeclNode *)calloc(snap.node_count,sizeof(UmiDeclNode));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if(out->components==NULL){out->component_count=0U;return UMI_STATUS_OUT_OF_MEMORY;}
        /* Visit each bounded item once so every record receives the same rule. */
        for(i=0U;i<snap.node_count;++i)(void)umi_decl_document_node_at(doc,i,&out->components[i]);
    }
    return UMI_STATUS_OK;
}

/* Release or reset state held by decl plan so the same storage can be reused safely. */
void umi_decl_plan_dispose(UmiDeclApplicationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(plan==NULL)return;
    free(plan->components);
    (void)memset(plan,0,sizeof(*plan));
}

/* Find decl plan while leaving the underlying catalogue or model owned by this module. */
UmiStatus umi_decl_plan_find(const UmiDeclApplicationPlan *p,const char *id,UmiDeclNode *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->component_count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->components[i].node_id,id)==0){*out=p->components[i];return UMI_STATUS_OK;}}return UMI_STATUS_NOT_FOUND;}
