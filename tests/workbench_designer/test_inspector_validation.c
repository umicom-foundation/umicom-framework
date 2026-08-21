/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_inspector_validation.c
 *
 * PURPOSE:
 *   Verify property inspector validation rejects invalid identifiers, split
 *   ratios and undersized geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerPropertyModel model;
    const UmiWorkbenchDesignerProperty *property;
    const UmiWorkbenchLayoutNode *node;
    UmiWorkbenchDesignerInspectorValidation validation;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_property_model_init(&model);
    TEST_REQUIRE_STATUS(umi_workbench_designer_property_model_build(
        &model, &document, "right"));
    node = umi_workbench_layout_document_find_node(&document, "right");
    property = umi_workbench_designer_property_find(&model, "split-ratio");
    TEST_REQUIRE(node != NULL && property != NULL);
    TEST_REQUIRE(umi_workbench_designer_inspector_validate_number(
        property, node, 1.2, &validation) == UMI_STATUS_INVALID_ARGUMENT);
    TEST_REQUIRE(!validation.valid);
    TEST_REQUIRE_STATUS(umi_workbench_designer_inspector_validate_number(
        property, node, 0.5, &validation));
    TEST_REQUIRE(validation.valid);
    return 0;
}
