/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_sarif_fuzz.c
 *
 * PURPOSE:
 *   Exercise the dependency-free SARIF tokenizer and validator with
 *   deterministic truncations and byte mutations. Malformed interchange must
 *   fail cleanly without memory errors, hangs or partial ownership transfer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/diagnostics/sarif.h"

static uint32_t next_random(uint32_t *state)
{
    *state = *state * UINT32_C(1664525) + UINT32_C(1013904223);
    return *state;
}

int main(void)
{
    static const char valid[] =
        "{\"version\":\"2.1.0\",\"runs\":[{\"tool\":{\"driver\":"
        "{\"name\":\"fuzz\"}},\"results\":[{\"ruleId\":\"F1\","
        "\"level\":\"warning\",\"message\":{\"text\":\"warning\"}}]}]}";
    uint32_t state = UINT32_C(0x95a12f37);
    size_t iteration;
    char message[128];

    assert(umi_diagnostic_sarif_validate(valid, sizeof(valid) - 1U,
                                         message, sizeof(message)) ==
           UMI_STATUS_OK);
    for (iteration = 0U; iteration < 5000U; ++iteration) {
        size_t size = sizeof(valid) - 1U;
        char *mutated = (char *)malloc(size);
        size_t changes;
        size_t change;
        UmiStatus status;
        assert(mutated != NULL);
        (void)memcpy(mutated, valid, size);
        changes = 1U + (size_t)(next_random(&state) % UINT32_C(6));
        for (change = 0U; change < changes; ++change) {
            size_t position = (size_t)(next_random(&state) % (uint32_t)size);
            mutated[position] = (char)(next_random(&state) & UINT32_C(0x7f));
        }
        if ((next_random(&state) & UINT32_C(3)) == 0U) {
            size = (size_t)(next_random(&state) % (uint32_t)size);
        }
        status = umi_diagnostic_sarif_validate(mutated, size,
                                               message, sizeof(message));
        assert(status == UMI_STATUS_OK || status == UMI_STATUS_PARSE_ERROR ||
               status == UMI_STATUS_INVALID_ARGUMENT ||
               status == UMI_STATUS_CAPACITY_EXCEEDED);
        free(mutated);
    }
    return 0;
}
