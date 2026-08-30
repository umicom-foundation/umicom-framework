/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_command_catalogue.c
 *
 * PURPOSE:
 *   Contract coverage for AI Developer Experience command catalogue.
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
#include "umicom/ai_developer_experience/command.h"

int main(void)
{
    size_t index;
    size_t count = umi_ai_developer_command_count();

    assert(count == 23U);
    assert(umi_ai_developer_command_find(
        "ai.developer.open-overview") != NULL);
    assert(umi_ai_developer_command_find(
        "ai.developer.patch-apply")->mutates_state);

    for (index = 0U; index < count; ++index) {
        const UmiAiDeveloperCommandDescriptor *item =
            umi_ai_developer_command_at(index);
        size_t other;

        assert(item != NULL);
        assert(item->command_id != NULL);
        assert(item->label != NULL);

        for (other = index + 1U; other < count; ++other) {
            assert(strcmp(
                item->command_id,
                umi_ai_developer_command_at(other)->command_id) != 0);
        }
    }

    return 0;
}

