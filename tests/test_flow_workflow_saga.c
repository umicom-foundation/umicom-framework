/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_flow_workflow_saga.c
 *
 * PURPOSE:
 *   Verify message flow stages, workflow compensation and saga terminal-state classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

static UmiStatus flow_stage(const UmiMessageEnvelope *input,
                            UmiOwnedMessage *output,
                            void *user_data)
{
    UmiMessageEnvelope next = *input;
    next.name = (const char *)user_data;
    return umi_message_copy(&next, output);
}
static UmiStatus workflow_ok(void *context, void *user_data)
{
    int *value = (int *)context;
    *value += *(const int *)user_data;
    return UMI_STATUS_OK;
}
static UmiStatus workflow_fail(void *context, void *user_data)
{
    (void)context; (void)user_data;
    return UMI_STATUS_INTERNAL_ERROR;
}
static void workflow_compensate(void *context, void *user_data)
{
    int *value = (int *)context;
    *value -= *(const int *)user_data;
}
int main(void)
{
    UmiFlow *flow = NULL;
    UmiWorkflow *workflow = NULL;
    UmiOwnedMessage output;
    UmiMessageEnvelope input;
    UmiSaga saga;
    size_t completed = 0U;
    int context = 0;
    int amount = 5;
    assert(umi_flow_create("flow", 2U, &flow) == UMI_STATUS_OK);
    assert(umi_flow_add_stage(flow, "rename", flow_stage, "renamed") == UMI_STATUS_OK);
    umi_message_envelope_init(&input, UMI_MESSAGE_EVENT, "start", "p");
    assert(umi_flow_execute(flow, &input, &output, &completed) == UMI_STATUS_OK);
    assert(completed == 1U && strcmp(output.envelope.name, "renamed") == 0);
    umi_message_dispose(&output);
    umi_flow_destroy(flow);
    assert(umi_workflow_create("workflow", 1U, 2U, &workflow) == UMI_STATUS_OK);
    assert(umi_workflow_add_step(workflow, "one", workflow_ok,
                                 workflow_compensate, &amount) == UMI_STATUS_OK);
    assert(umi_workflow_add_step(workflow, "fail", workflow_fail,
                                 NULL, NULL) == UMI_STATUS_OK);
    umi_saga_init(&saga, 1U, 2U, workflow);
    assert(umi_saga_execute(&saga, &context) == UMI_STATUS_INTERNAL_ERROR);
    assert(context == 0 && saga.state == UMI_SAGA_COMPENSATED);
    umi_workflow_destroy(workflow);
    return 0;
}
