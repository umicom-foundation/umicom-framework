/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_dynamic_discovery.c
 *
 * PURPOSE:
 *   Verify that a portable layout file can be discovered and refreshed in live
 *   template and reflection catalogues without rebuilding an application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_fixture.h"

#include "umicom/messaging/message.h"
#include "umicom/platform/filesystem.h"
#include "umicom/reflection/reflection.h"
#include "umicom/workbench_layout/discovery.h"

int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.dynamic.test");
    UmiWorkbenchLayoutTemplateRegistry *templates =
        (UmiWorkbenchLayoutTemplateRegistry *)calloc(1U, sizeof(*templates));
    UmiReflectionEngine *reflection = NULL;
    UmiReflectionResourceDescriptor resource;
    UmiWorkbenchLayoutJsonOptions options;
    UmiWorkbenchLayoutJsonResult json_result;
    char temporary_directory[UMI_PATH_CAPACITY];
    char file_name[96];
    char path[UMI_PATH_CAPACITY];
    char *json;
    int replaced = 0;
    UmiStatus status;

    TEST_REQUIRE(document != NULL && templates != NULL,
                 "Dynamic discovery state must be allocated");
    TEST_STATUS_OK(umi_reflection_engine_create(NULL, &reflection));
    umi_workbench_layout_template_registry_init(templates);
    options = umi_workbench_layout_json_options_default();
    status = umi_workbench_layout_json_encode(
        document, &options, NULL, 0U, &json_result);
    TEST_REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED,
                 "The JSON sizing pass must report its required capacity");
    json = (char *)malloc(json_result.bytes_required);
    TEST_REQUIRE(json != NULL, "The portable layout buffer must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_json_encode(
        document,
        &options,
        json,
        json_result.bytes_required,
        &json_result));
    TEST_STATUS_OK(umi_fs_temp_directory(
        temporary_directory, sizeof(temporary_directory)));
    (void)snprintf(file_name,
                   sizeof(file_name),
                   "umicom-layout-%llu.umilayout",
                   (unsigned long long)umi_message_next_id());
    TEST_STATUS_OK(umi_fs_join(
        path, sizeof(path), temporary_directory, file_name));
    TEST_STATUS_OK(umi_fs_write_text(path, json));

    TEST_STATUS_OK(umi_workbench_layout_discovery_load_file(
        templates, reflection, path, NULL, &replaced));
    TEST_REQUIRE(replaced == 0,
                 "The first discovery must add a new layout template");
    TEST_REQUIRE(umi_workbench_layout_template_registry_find(
                     templates, "layout.dynamic.test") != NULL,
                 "The discovered layout must be available to layout selectors");
    TEST_STATUS_OK(umi_reflection_registry_get_resource(
        umi_reflection_engine_registry(reflection),
        "layout.dynamic.test",
        &resource));
    TEST_REQUIRE(resource.kind == UMI_REFLECTION_RESOURCE_LAYOUT,
                 "Reflection must identify the discovered resource as a layout");

    TEST_STATUS_OK(umi_workbench_layout_document_set_metadata(
        document,
        "Refreshed Dynamic Layout",
        document->category,
        document->description));
    document->version.revision += 1U;
    umi_workbench_layout_document_refresh_hash(document);
    free(json);
    status = umi_workbench_layout_json_encode(
        document, &options, NULL, 0U, &json_result);
    TEST_REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED,
                 "The refreshed JSON sizing pass must report capacity");
    json = (char *)malloc(json_result.bytes_required);
    TEST_REQUIRE(json != NULL, "The refreshed layout buffer must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_json_encode(
        document,
        &options,
        json,
        json_result.bytes_required,
        &json_result));
    TEST_STATUS_OK(umi_fs_write_text(path, json));
    TEST_STATUS_OK(umi_workbench_layout_discovery_load_file(
        templates, reflection, path, NULL, &replaced));
    TEST_REQUIRE(replaced == 1,
                 "Rediscovery must refresh the existing layout template");
    TEST_REQUIRE(strcmp(
                     umi_workbench_layout_template_registry_find(
                         templates, "layout.dynamic.test")->display_name,
                     "Refreshed Dynamic Layout") == 0,
                 "The refreshed template must expose its new display name");

    (void)remove(path);
    free(json);
    umi_reflection_engine_destroy(reflection);
    free(templates);
    free(document);
    return 0;
}
