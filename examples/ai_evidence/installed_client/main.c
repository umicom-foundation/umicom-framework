/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ai_evidence/installed_client/main.c
 * PURPOSE:
 *   Public headers only. Inspection does not require a model or a workspace.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Public headers only. Inspection does not require a model or a workspace. */
#include "umicom/ai_workspace/evidence.h"
#include <stdio.h>
int main(void)
{
    UmiAiEvidenceReferences references;
    UmiStatus status=UmiAiEvidenceScanReferences("Save the note. [S1] Check it. [S2]",2U,&references);
    if(status!=UMI_STATUS_OK || references.validCount!=2U)return 1;
    puts("Two reference targets exist. The statements still require source review.");
    return 0;
}
