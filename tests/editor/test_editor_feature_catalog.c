/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_feature_catalog.c
 *
 * PURPOSE:
 *   Implement the test editor feature catalog behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework editor feature catalogue tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
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
    UmiEditorFeatureCatalogSnapshot snapshot;
    UmiEditorFeatureQueryResult results;
    UmiEditorFeatureDescriptor descriptor;
    char message[UMI_EDITOR_FEATURE_MESSAGE_CAPACITY];

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_feature_catalog_create(&catalog) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_feature_catalog_register_builtins(catalog) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_feature_catalog_snapshot(catalog, &snapshot) != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (snapshot.total_count != 119U || snapshot.stable_count == 0U ||
        snapshot.preview_count == 0U || snapshot.experimental_count == 0U) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_feature_catalog_query_category(
            catalog, UMI_EDITOR_FEATURE_CATEGORY_TEXT, &results) != UMI_STATUS_OK ||
        results.count == 0U) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_feature_catalog_query_flags(
            catalog, UMI_EDITOR_FEATURE_FLAG_GUI, &results) != UMI_STATUS_OK ||
        results.count == 0U) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_feature_catalog_find(catalog,
            "umicom.editor.text_buffer", &descriptor) != UMI_STATUS_OK ||
        strcmp(descriptor.title, "Text Buffer") != 0) return 7;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_editor_feature_catalog_validate_dependencies(
            catalog, message, sizeof(message)) != UMI_STATUS_OK) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_feature_catalog_remove(
            catalog, "umicom.editor.text_buffer") != UMI_STATUS_OK) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_feature_catalog_count(catalog) != 118U) return 10;
    umi_editor_feature_catalog_destroy(catalog);
    return EXIT_SUCCESS;
}
