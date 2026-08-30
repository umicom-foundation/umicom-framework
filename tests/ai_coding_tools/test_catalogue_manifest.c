/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_catalogue_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI coding tools catalogue manifest.
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
#include "umicom/ai_coding_tools/manifest.h"

int main(void)
{
    size_t i;
    size_t j;

    assert(umi_ai_coding_tool_catalogue_count() == 40U);
    assert(umi_ai_coding_tool_manifest_count() == 40U);

    for (i = 0U; i < umi_ai_coding_tool_catalogue_count(); ++i) {
        const UmiAiCodingToolDescriptor *left =
            umi_ai_coding_tool_catalogue_at(i);
        UmiAiCodingToolManifestEntry manifest;

        assert(left != NULL);
        assert(left->tool_id[0] != '\0');
        assert(umi_ai_coding_tool_manifest_find(
            left->tool_id, &manifest) == UMI_STATUS_OK);
        assert(manifest.arguments_schema[0] == '{');

        for (j = i + 1U;
             j < umi_ai_coding_tool_catalogue_count();
             ++j) {
            const UmiAiCodingToolDescriptor *right =
                umi_ai_coding_tool_catalogue_at(j);
            assert(right != NULL);
            assert(strcmp(left->tool_id, right->tool_id) != 0);
        }
    }

    return 0;
}

