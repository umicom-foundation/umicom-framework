/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_document_validation.c
 *
 * PURPOSE:
 *   Verify canonical document topology, metadata, validation and deterministic
 *   content hashes for a multi-panel Studio workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.development");
    UmiWorkbenchLayoutValidationOptions options;
    UmiWorkbenchLayoutValidationReport report;
    uint64_t original_hash;

    TEST_REQUIRE(document != NULL, "Fixture layout must be allocated");
    TEST_REQUIRE(document->node_count == 8U, "Fixture must contain eight nodes");
    TEST_REQUIRE(
        document->root_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE,
        "Fixture must have a root node");
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_document_root(document)->node_id,
            "root") == 0,
        "Root node identity must be stable");
    TEST_REQUIRE(
        umi_workbench_layout_document_has_tag(document, "studio"),
        "Product tag must be retained");

    options = umi_workbench_layout_validation_options_default();
    options.validate_hash = true;
    TEST_STATUS_OK(umi_workbench_layout_validate(
        document, &options, &report));
    TEST_REQUIRE(report.valid, "The development layout must validate");
    TEST_REQUIRE(report.error_count == 0U, "No validation errors are expected");

    original_hash = document->content_hash;
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document,
        "Development Workbench Revised",
        document->category,
        document->description));
    umi_workbench_layout_document_refresh_hash(document);
    TEST_REQUIRE(
        document->content_hash != original_hash,
        "Metadata changes must affect the deterministic content hash");

    document->nodes[document->root_index].split_ratio = 1.5;
    TEST_REQUIRE(
        umi_workbench_layout_validate(document, &options, &report) ==
            UMI_STATUS_INVALID_STATE,
        "Invalid layout state must be reported through the status contract");
    TEST_REQUIRE(!report.valid, "Invalid split ratio must invalidate the layout");
    TEST_REQUIRE(
        umi_workbench_layout_validation_report_has_code(
            &report,
            UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_SPLIT_RATIO),
        "Validation must identify the split-ratio invariant");

    free(document);
    return 0;
}
