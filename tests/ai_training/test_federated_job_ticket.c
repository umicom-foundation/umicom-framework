/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_job_ticket.c
 *
 * PURPOSE:
 *   Refuse cross-job and altered tickets, duplicate identities and commits
 *   attempted after cancellation or completed finalisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "federated_job_fixture.h"

int main(void)
{
    UmiAiTrainingFederatedJobConfig cfg = TestJobConfig();
    UmiAiTrainingFederatedJob *left = NULL, *right = NULL;
    UmiAiTrainingFederatedTicket ticket = {0};
    UmiAiTrainingFederatedAssignment a = TestAssignment(&cfg, 7U, 80U, 32U);
    const float delta[2] = {0.125F, -0.125F};
    VERIFY(UmiAiTrainingFederatedJobCreate(&cfg, &left) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobCreate(&cfg, &right) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobAdmit(left, &a, &ticket) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobCancel(right, ticket) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingFederatedTicket forged = ticket;
    ++forged.serial;
    VERIFY(UmiAiTrainingFederatedJobCancel(left, forged) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingFederatedJobAdmit(left, &a, &forged) == UMI_STATUS_ALREADY_EXISTS);
    UmiAiTrainingFederatedContribution c = TestContribution(&a, delta);
    VERIFY(UmiAiTrainingFederatedJobComplete(left, ticket, &c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobComplete(left, ticket, &c) == UMI_STATUS_INVALID_STATE);
    float mean[2] = {0};
    VERIFY(UmiAiTrainingFederatedJobFinalise(left, mean, 2U, NULL) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingFederatedJobCancel(left, ticket) == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingFederatedJobDestroy(left);
    UmiAiTrainingFederatedJobDestroy(right);
    puts("Federated-job tickets cannot cross owning jobs or be replayed");
    return 0;
}
