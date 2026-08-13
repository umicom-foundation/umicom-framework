/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_features/test_editor_features_01.c
 *
 * PURPOSE:
 *   Verify canonical editor feature descriptors remain valid and discoverable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_text_buffer();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
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
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_line_index();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
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
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_text_position();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
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
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_text_range();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
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
        const UmiEditorFeatureDescriptor *descriptor = umi_editor_feature_text_snapshot();
        if (descriptor == NULL ||
            descriptor->struct_size != sizeof(*descriptor) ||
            descriptor->api_version != UMI_EDITOR_FEATURE_API_VERSION ||
            descriptor->category != UMI_EDITOR_FEATURE_CATEGORY_TEXT ||
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
