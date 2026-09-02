/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_features/test_editor_features_07.c
 *
 * PURPOSE:
 *   Verify canonical editor feature descriptors remain valid and discoverable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "umicom/editor/features.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorFeatureCatalog *catalog = NULL;
    UmiEditorFeatureDescriptor found;
    UmiStatus status;

    status = umi_editor_feature_catalog_create(&catalog);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || catalog == NULL) return EXIT_FAILURE;
    status = umi_editor_feature_catalog_register_builtins(catalog);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_editor_feature_catalog_destroy(catalog);
        return EXIT_FAILURE;
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_line_operations();
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_EDITING ||
            descriptor->maturity != UMI_EDITOR_FEATURE_MATURITY_STABLE ||
            descriptor->feature_id[0] == '\0' ||
            descriptor->title[0] == '\0' ||
            descriptor->summary[0] == '\0') {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
        status = umi_editor_feature_catalog_find(catalog,
                                                  descriptor->feature_id,
                                                  &found);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_case_transform();
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_EDITING ||
            descriptor->maturity != UMI_EDITOR_FEATURE_MATURITY_STABLE ||
            descriptor->feature_id[0] == '\0' ||
            descriptor->title[0] == '\0' ||
            descriptor->summary[0] == '\0') {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
        status = umi_editor_feature_catalog_find(catalog,
                                                  descriptor->feature_id,
                                                  &found);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_sort_lines();
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_EDITING ||
            descriptor->maturity != UMI_EDITOR_FEATURE_MATURITY_STABLE ||
            descriptor->feature_id[0] == '\0' ||
            descriptor->title[0] == '\0' ||
            descriptor->summary[0] == '\0') {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
        status = umi_editor_feature_catalog_find(catalog,
                                                  descriptor->feature_id,
                                                  &found);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_snippet();
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_EDITING ||
            descriptor->maturity != UMI_EDITOR_FEATURE_MATURITY_PREVIEW ||
            descriptor->feature_id[0] == '\0' ||
            descriptor->title[0] == '\0' ||
            descriptor->summary[0] == '\0') {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
        status = umi_editor_feature_catalog_find(catalog,
                                                  descriptor->feature_id,
                                                  &found);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_format_on_type();
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_EDITING ||
            descriptor->maturity != UMI_EDITOR_FEATURE_MATURITY_PREVIEW ||
            descriptor->feature_id[0] == '\0' ||
            descriptor->title[0] == '\0' ||
            descriptor->summary[0] == '\0') {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
        status = umi_editor_feature_catalog_find(catalog,
                                                  descriptor->feature_id,
                                                  &found);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    umi_editor_feature_catalog_destroy(catalog);
    return EXIT_SUCCESS;
}
