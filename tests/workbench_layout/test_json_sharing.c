/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_json_sharing.c
 *
 * PURPOSE:
 *   Verify lossless .umilayout JSON, portable bundle integrity and controlled
 *   ownership transfer during import.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.portable");
    UmiWorkbenchLayoutDocument *decoded =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*decoded));
    UmiWorkbenchLayoutDocument *imported =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*imported));
    UmiWorkbenchLayoutShareBundle *bundle =
        (UmiWorkbenchLayoutShareBundle *)calloc(1U, sizeof(*bundle));
    UmiWorkbenchLayoutShareBundle *decoded_bundle =
        (UmiWorkbenchLayoutShareBundle *)calloc(1U, sizeof(*decoded_bundle));
    UmiWorkbenchLayoutJsonOptions options;
    UmiWorkbenchLayoutJsonResult json_result;
    UmiWorkbenchLayoutImportOptions import_options;
    char *json = NULL;
    char *bundle_text = NULL;
    size_t required = 0U;
    uint64_t original_hash;
    UmiStatus status;

    TEST_REQUIRE(
        document != NULL && decoded != NULL && imported != NULL &&
        bundle != NULL && decoded_bundle != NULL,
        "Portable layout state must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document,
        "Development \"Workbench\"",
        document->category,
        document->description));
    umi_workbench_layout_document_refresh_hash(document);
    original_hash = document->content_hash;

    options = umi_workbench_layout_json_options_default();
    status = umi_workbench_layout_json_encode(
        document, &options, NULL, 0U, &json_result);
    TEST_REQUIRE(
        status == UMI_STATUS_CAPACITY_EXCEEDED,
        "Sizing pass must report required capacity");
    json = (char *)malloc(json_result.bytes_required);
    TEST_REQUIRE(json != NULL, "JSON buffer must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_json_encode(
        document,
        &options,
        json,
        json_result.bytes_required,
        &json_result));
    TEST_REQUIRE(
        strstr(json, "umicom.workbench-layout/2") != NULL,
        "Portable schema identity must be encoded");
    TEST_STATUS_OK(umi_workbench_layout_json_decode(
        json, strlen(json), decoded, &json_result));
    TEST_REQUIRE(
        decoded->content_hash == original_hash,
        "JSON round trip must preserve the semantic content hash");
    TEST_REQUIRE(
        decoded->node_count == document->node_count,
        "JSON round trip must preserve every semantic node");

    TEST_STATUS_OK(umi_workbench_layout_share_bundle_create(
        document,
        "bundle.development",
        "developer",
        2000U,
        true,
        true,
        bundle));
    status = umi_workbench_layout_share_bundle_encode(
        bundle, NULL, 0U, &required);
    TEST_REQUIRE(
        status == UMI_STATUS_CAPACITY_EXCEEDED,
        "Bundle sizing pass must report required capacity");
    bundle_text = (char *)malloc(required);
    TEST_REQUIRE(bundle_text != NULL, "Bundle buffer must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_share_bundle_encode(
        bundle, bundle_text, required, &required));
    TEST_STATUS_OK(umi_workbench_layout_share_bundle_decode(
        bundle_text, strlen(bundle_text), decoded_bundle));
    TEST_STATUS_OK(umi_workbench_layout_share_bundle_validate(decoded_bundle));

    import_options = umi_workbench_layout_import_options_default();
    test_copy_text(
        import_options.target_layout_id,
        sizeof(import_options.target_layout_id),
        "layout.imported");
    test_copy_text(
        import_options.target_owner_user_id,
        sizeof(import_options.target_owner_user_id),
        "second-developer");
    test_copy_text(
        import_options.target_workspace_id,
        sizeof(import_options.target_workspace_id),
        "workspace.imported");
    import_options.preserve_source_audit = false;
    import_options.preserve_runtime_geometry = false;
    TEST_STATUS_OK(umi_workbench_layout_share_bundle_import(
        decoded_bundle, &import_options, imported));
    TEST_REQUIRE(
        strcmp(imported->identity.layout_id, "layout.imported") == 0,
        "Import must assign the requested target identity");
    TEST_REQUIRE(
        strcmp(imported->identity.owner_user_id, "second-developer") == 0,
        "Import must transfer ownership explicitly");
    TEST_REQUIRE(
        imported->audit.created_by[0] == '\0',
        "Source audit must be removed when import policy requires it");
    TEST_REQUIRE(
        umi_workbench_layout_document_has_flag(
            imported, UMI_WORKBENCH_LAYOUT_DOCUMENT_SHARED),
        "Imported shared bundle must retain sharing provenance");

    free(bundle_text);
    free(json);
    free(decoded_bundle);
    free(bundle);
    free(imported);
    free(decoded);
    free(document);
    return 0;
}
