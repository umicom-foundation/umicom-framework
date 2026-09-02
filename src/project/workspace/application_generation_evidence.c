/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/application_generation_evidence.c
 *
 * PURPOSE:
 *   Implement the application generation evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_generation_evidence.h"
/*
 * Provide the project workspace application generation evidence fingerprint operation used
 * by this module and its client applications.
 */
uint64_t umi_project_workspace_application_generation_evidence_fingerprint(const char*text) {
    uint64_t h=UINT64_C(1469598103934665603);
    const unsigned char*p=(const unsigned char*)text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(text==NULL)return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while(*p!='\0') {
        h^=(uint64_t)*p++;
        h*=UINT64_C(1099511628211);
    }
    return h;
}
