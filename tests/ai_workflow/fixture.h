/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/fixture.h
 *
 * PURPOSE:
 *   Share bounded real workflow descriptions between contract and allocation regression fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_AI_WORKFLOW_FIXTURE_H
#define UMICOM_TEST_AI_WORKFLOW_FIXTURE_H
#include "umicom/ai_workflow/preflight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WorkflowFixture {
    UmiAiWorkflowNodeType types[UMI_AI_WORKFLOW_MAX_TYPES];
    UmiAiWorkflowNode nodes[UMI_AI_WORKFLOW_MAX_NODES];
    UmiAiWorkflowLink links[UMI_AI_WORKFLOW_MAX_LINKS];
    size_t outputs[UMI_AI_WORKFLOW_MAX_NODES];
    UmiAiWorkflowDescription description;
    UmiAiWorkflowPlan plan;
    unsigned char before[sizeof(UmiAiWorkflowPlan)];
    UmiAiWorkflowDiagnostic diagnostic;
} WorkflowFixture;

static WorkflowFixture *workflow_fixture_create(void)
{
    WorkflowFixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return NULL;
    fixture->types[0] = (UmiAiWorkflowNodeType){
        .operation_id="input.text", .outputs={{"text", UMI_AI_WORKFLOW_TEXT, false}},
        .output_count=1U, .available=true};
    fixture->types[1] = (UmiAiWorkflowNodeType){
        .operation_id="text.transform", .inputs={{"prompt", UMI_AI_WORKFLOW_TEXT, true}},
        .outputs={{"text", UMI_AI_WORKFLOW_TEXT, false}},
        .input_count=1U, .output_count=1U, .available=true};
    fixture->types[2] = (UmiAiWorkflowNodeType){
        .operation_id="output.text", .inputs={{"text", UMI_AI_WORKFLOW_TEXT, true}},
        .input_count=1U, .available=true};
    fixture->nodes[0] = (UmiAiWorkflowNode){"z-source",0U,0U};
    fixture->nodes[1] = (UmiAiWorkflowNode){"m-filter",1U,0U};
    fixture->nodes[2] = (UmiAiWorkflowNode){"a-output",2U,0U};
    fixture->links[0] = (UmiAiWorkflowLink){0U,0U,1U,0U};
    fixture->links[1] = (UmiAiWorkflowLink){1U,0U,2U,0U};
    fixture->outputs[0] = 2U;
    fixture->description = (UmiAiWorkflowDescription){
        fixture->types,3U,fixture->nodes,3U,fixture->links,2U,fixture->outputs,1U};
    memset(&fixture->plan,0xa5,sizeof(fixture->plan));
    memcpy(fixture->before,&fixture->plan,sizeof(fixture->plan));
    return fixture;
}
#endif
