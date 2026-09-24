/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_intelligence/test_service.c
 *
 * PURPOSE:
 *   Verify the complete Language + AI capability catalogue is discoverable and
 *   every registered policy produces a validated snapshot.
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

#include "umicom/developer_intelligence/service.h"

int main(void)
{
    UmiDeveloperIntelligenceInput input;
    UmiDeveloperIntelligenceSnapshot snapshot;
    size_t index;

    UmiDeveloperIntelligenceInputInit(&input);
    input.lexicalScore = 90.0;
    input.semanticScore = 90.0;
    input.contextScore = 90.0;
    input.confidence = 90.0;
    input.risk = 10.0;
    input.baselineScore = 70.0;
    input.matched = 8U;
    input.total = 10U;
    input.citations = 3U;
    input.sourceCount = 4U;
    input.usedUnits = 500U;
    input.budgetUnits = 1000U;
    input.localAvailable = 1;
    input.remoteAvailable = 1;
    input.remoteAllowed = 1;
    input.approved = 1;
    input.trusted = 1;
    input.active = 1;
    input.revision = 77U;

    assert(UmiDeveloperIntelligenceServiceCount() == 80U);
    for (index = 0U; index < UmiDeveloperIntelligenceServiceCount(); ++index) {
        const char *id = NULL;
        assert(UmiDeveloperIntelligenceServiceIdAt(index, &id) == UMI_STATUS_OK);
        assert(id != NULL && id[0] != '\0');
        assert(UmiDeveloperIntelligenceServiceEvaluate(
                   id, &input, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.id, id) == 0);
        assert(snapshot.revision == 77U);
    }

    assert(UmiDeveloperIntelligenceServiceEvaluate(
               "missing.capability", &input, &snapshot) == UMI_STATUS_NOT_FOUND);
    return 0;
}
