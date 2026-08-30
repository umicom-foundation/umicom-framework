/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_features/test_editor_features_11.c
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

int main(void)
{
    UmiEditorFeatureCatalog *catalog = NULL;
    UmiEditorFeatureDescriptor found;
    UmiStatus status;

    status = umi_editor_feature_catalog_create(&catalog);
    if (status != UMI_STATUS_OK || catalog == NULL) return EXIT_FAILURE;
    status = umi_editor_feature_catalog_register_builtins(catalog);
    if (status != UMI_STATUS_OK) {
        umi_editor_feature_catalog_destroy(catalog);
        return EXIT_FAILURE;
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_editor_gutter();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_VIEW ||
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
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_line_numbers();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_VIEW ||
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
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_minimap();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_VIEW ||
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
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_overview_ruler();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_VIEW ||
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
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    {
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_word_wrap();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_VIEW ||
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
        if (status != UMI_STATUS_OK ||
            strcmp(found.feature_id, descriptor->feature_id) != 0) {
            umi_editor_feature_catalog_destroy(catalog);
            return EXIT_FAILURE;
        }
    }

    umi_editor_feature_catalog_destroy(catalog);
    return EXIT_SUCCESS;
}
