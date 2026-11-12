/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_chain_admission_failures.c
 *
 * PURPOSE:
 *   Verify failed chain allocation and exhausted identities leave existing pipeline storage intact.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "umicom/developer/pipeline.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t allocation_call, fail_allocation;
static void *ChainCalloc(size_t count, size_t size)
{
    ++allocation_call;
    if (fail_allocation != 0U && allocation_call == fail_allocation) return NULL;
    return calloc(count, size);
}
/* Include the actual implementation to inject allocation refusal and inspect
 * otherwise unreachable identity exhaustion. Public integration cases remain
 * in the separate workflow transaction suite. No alternate queue is modeled. */
#define calloc ChainCalloc
#include "../../src/developer/pipeline.c"
#undef calloc

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDeveloperPipeline *pipeline = NULL;
    assert(umi_developer_pipeline_create(&pipeline) == UMI_STATUS_OK);
    UmiDeveloperOperationSnapshot *operations = calloc(2U, sizeof(*operations));
    assert(operations != NULL);
    assert(umi_developer_operation_init(&operations[0], "existing", UMI_DEVELOPER_OPERATION_CUSTOM, "Existing") == UMI_STATUS_OK);
    assert(umi_developer_pipeline_submit(pipeline, &operations[0]) == UMI_STATUS_OK);
    assert(umi_developer_pipeline_start(pipeline, "existing") == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&operations[0], "new.first", UMI_DEVELOPER_OPERATION_CUSTOM, "First") == UMI_STATUS_OK);
    assert(umi_developer_operation_init(&operations[1], "new.second", UMI_DEVELOPER_OPERATION_CUSTOM, "Second") == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    if (!strcmp(argv[1], "allocation-owner")) { fail_allocation = 1U; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (!strcmp(argv[1], "allocation-operations")) { fail_allocation = 2U; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (!strcmp(argv[1], "allocation-dependencies")) { fail_allocation = 3U; expected = UMI_STATUS_OUT_OF_MEMORY; }
    else if (!strcmp(argv[1], "pipeline-revision")) pipeline->revision = UINT64_MAX - 1U;
    else if (!strcmp(argv[1], "sequence")) pipeline->next_sequence = UINT64_MAX - 1U;
    else if (!strcmp(argv[1], "operation-revision")) pipeline->operations[0].revision = UINT64_MAX;
    else return 2;
    UmiDeveloperPipeline before = *pipeline;
    UmiDeveloperOperationSnapshot existing = pipeline->operations[0];
    allocation_call = 0U;
    assert(umi_developer_pipeline_submit_chain(pipeline, operations, 2U) == expected);
    assert(memcmp(&before, pipeline, sizeof(before)) == 0);
    assert(memcmp(&existing, &pipeline->operations[0], sizeof(existing)) == 0);
    assert(pipeline->operations[0].state == UMI_DEVELOPER_OPERATION_RUNNING);
    fail_allocation = 0U;
    free(operations);
    umi_developer_pipeline_destroy(pipeline);
    return 0;
}
