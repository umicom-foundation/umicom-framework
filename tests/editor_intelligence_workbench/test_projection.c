/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_projection.c
 *
 * PURPOSE:
 *   Verify bounded canonical-source projection, flags and long-text handling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/projection.h"

int main(void)
{
    UmiEditorSourceLocation source;
    UmiEditorIntelEntry entry;
    char long_label[600];

    (void)memset(long_label, 'x', sizeof(long_label));
    long_label[sizeof(long_label) - 1U] = '\0';
    assert(umi_editor_source_location_initialize(
               &source, "file:///workspace/main.c", 12U, 4U) ==
           UMI_STATUS_OK);
    source.end_line = 12U;
    source.end_column = 9U;
    assert(umi_editor_intel_projection_entry_from_source(
               &entry, "projection-entry", long_label, long_label, &source,
               42U, UMI_EDITOR_INTEL_PROJECTION_PRIMARY, 7U) ==
           UMI_STATUS_OK);
    assert(umi_editor_intel_entry_valid(&entry));
    assert(entry.score == 42U);
    assert(entry.revision == 7U);
    assert((entry.flags & UMI_EDITOR_INTEL_PROJECTION_PRIMARY) != 0U);
    assert((entry.flags & UMI_EDITOR_INTEL_PROJECTION_TRUNCATED) != 0U);
    assert(entry.location.range.start.line == 12U);
    assert(umi_editor_intel_projection_hash_text("stable") ==
           umi_editor_intel_projection_hash_text("stable"));
    assert(umi_editor_intel_projection_score_from_rank(-1) == 0U);
    assert(umi_editor_intel_projection_score_from_rank(25) == 25U);
    return 0;
}
