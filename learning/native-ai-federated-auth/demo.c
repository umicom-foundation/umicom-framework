/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-federated-auth/demo.c
 *
 * PURPOSE:
 *   Demonstrate an authenticated synthetic federated-result round-trip using
 *   canonical Framework wire data and a clearly non-deployment teaching key.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_authenticated_frame.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiAiTrainingFederatedWireRecord proposal={0}, decoded={0};
    float values[1]={0.125F}, recovered[1]={0};
    uint8_t wire[UMI_AI_FEDERATED_WIRE_MAX_BYTES]={0};
    uint8_t protectedFrame[UMI_AI_FEDERATED_AUTH_MAX_BYTES]={0};
    const uint8_t teachingOnlySecret[32]={ 0x4e,0x4f,0x54,0x2d,0x41,0x2d,0x4b,0x45,
        0x59,0x2d,0x46,0x4f,0x52,0x2d,0x44,0x45,0x50,0x4c,0x4f,0x59,
        0x4d,0x45,0x4e,0x54,0x2d,0x4f,0x4e,0x4c,0x59,0x2d,0x21,0x21};
    size_t written=0U, wrapped=0U;
    proposal.kind=UMI_AI_FEDERATED_WIRE_RESULT;
    proposal.roundId=8U;
    proposal.sequence=1U;
    memset(proposal.assignment.contributorId,0x11,32U);
    memset(proposal.assignment.baseCheckpointSha256,0x22,32U);
    memset(proposal.assignment.datasetCommitmentSha256,0x33,32U);
    memset(proposal.assignment.dataPolicySha256,0x44,32U);
    proposal.assignment.distinctSampleCount=10U;
    proposal.assignment.localSteps=2U;
    proposal.assignment.declaredTransferBytes=64U;
    proposal.delta=values;
    proposal.deltaCount=1U;
    if (UmiAiTrainingFederatedWireEncode(&proposal,wire,sizeof(wire),
                                         &written)!=UMI_STATUS_OK ||
        UmiAiTrainingFederatedAuthSeal(wire,written,1U,teachingOnlySecret,
            sizeof(teachingOnlySecret),protectedFrame,sizeof(protectedFrame),
            &wrapped)!=UMI_STATUS_OK ||
        UmiAiTrainingFederatedAuthOpen(protectedFrame,wrapped,1U,
            teachingOnlySecret,sizeof(teachingOnlySecret),&decoded,recovered,
            1U)!=UMI_STATUS_OK) return 1;
    printf("Synthetic contributor MAC checked; round=%llu, delta=%g; "
           "not encrypted, not a training proof or LAN test.\n",
           (unsigned long long)decoded.roundId,(double)recovered[0]);
    return 0;
}
