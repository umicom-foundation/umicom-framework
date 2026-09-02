/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/project_template_filter.c
 *
 * PURPOSE:
 *   Implement the project template filter behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_template_filter.h"
#include <string.h>
/*
 * Provide the project workspace project template filter score operation used by this
 * module and its client applications.
 */
int umi_project_workspace_project_template_filter_score(const UmiProjectWorkspaceTemplateCandidate*item,const char*language,const char*frontend,uint64_t required_features) {
    int score=0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL||item->id==NULL)return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if((item->features&required_features)!=required_features)return -1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(language!=NULL&&language[0]!='\0') {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if(item->language==NULL||strcmp(item->language,language)!=0)return -1;
        score+=20;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(frontend!=NULL&&frontend[0]!='\0') {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if(item->frontend==NULL||strcmp(item->frontend,frontend)!=0)return -1;
        score+=10;
    }
    return score+(int)(item->features!=0U);
}
