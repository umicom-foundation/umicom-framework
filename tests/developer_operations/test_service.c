/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/test_service.c
 *
 * PURPOSE:
 *   Verify the complete developer-operations catalogue is discoverable and
 *   every registered evaluator can produce a valid snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_operations/service.h"

int main(void)
{
    UmiDeveloperOperationInput input;
    UmiDeveloperOperationSnapshot snapshot;
    size_t index;

    UmiDeveloperOperationInputInit(&input);
    input.primary = 7U;
    input.secondary = 1U;
    input.total = 10U;
    input.baseline = 65.0;
    input.threshold = 0.0;
    input.active = 1;
    input.revision = 99U;

    assert(UmiDeveloperOperationsServiceCount() == 80U);
    for (index = 0U; index < UmiDeveloperOperationsServiceCount(); ++index) {
        const char *id = NULL;
        assert(UmiDeveloperOperationsServiceIdAt(index, &id) == UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(UmiDeveloperOperationsServiceEvaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 99U);
    }

    assert(UmiDeveloperOperationsServiceEvaluate(
               "unknown.operation", &input, &snapshot) == UMI_STATUS_NOT_FOUND);
    return 0;
}
