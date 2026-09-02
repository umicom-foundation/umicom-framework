/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_palette_properties.c
 *
 * PURPOSE:
 *   Verify Framework panel discovery, palette filtering and semantic property-
 *   to-operation conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchDesignerPaletteQuery query;
    size_t indices[32U];
    size_t count;
    UmiWorkbenchDesignerPropertyModel properties;
    UmiWorkbenchLayoutOperation operation;
    const UmiWorkbenchDesignerProperty *title;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_palette_init(&palette);
    TEST_REQUIRE_STATUS(umi_workbench_designer_palette_seed_framework(&palette));
    TEST_REQUIRE(palette.count > 8U);
    query = umi_workbench_designer_palette_query_default();
    TEST_REQUIRE_STATUS(test_copy_text(
        query.text, sizeof(query.text), "editor"));
    count = umi_workbench_designer_palette_query(
        &palette, &query, indices, 32U);
    TEST_REQUIRE(count > 0U);
    TEST_REQUIRE(indices[0] < palette.count);

    umi_workbench_designer_property_model_init(&properties);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_model_build(
        &properties, &document, "editor"));
    TEST_REQUIRE(properties.count > 4U);
    title = umi_workbench_designer_property_find(&properties, "title");
    TEST_REQUIRE(title != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_set_text(
        &properties, "title", "Primary Editor"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_to_operation(
        &properties, "title", "user.sammy", "correlation.properties",
        document.version.revision, &operation));
    TEST_REQUIRE(operation.kind ==
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT ||
        operation.kind == UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS ||
        operation.kind == UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY ||
        operation.kind == UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP);
    return 0;
}
