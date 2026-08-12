/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_preview.c
 *
 * PURPOSE:
 *   Verify designer preview plus reusable palette search, property schema,
 *   geometry, grid/alignment/distribution and preview-profile helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises public Framework contracts only. Future applications can
 * therefore rely on the same behaviour without copying Studio implementation.
 */

#include "umicom/umicom.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void test_palette_and_inspector(void)
{
    UmiDeclComponentRegistry *registry = NULL;
    UmiDeclComponentDescriptor descriptor;
    UmiDeclPropertyDescriptor property;
    UmiDesignerPalette palette;
    UmiDesignerPaletteResults results;
    UmiDesignerInspectorSchema schema;

    assert(umi_decl_component_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_decl_component_descriptor_init(
        &descriptor, "button", "Button", "Controls", 0) == UMI_STATUS_OK);
    assert(umi_decl_property_descriptor_init(
        &property, "visible", UMI_DECL_VALUE_BOOLEAN, "true", 0) ==
        UMI_STATUS_OK);
    assert(umi_decl_component_add_property(&descriptor, &property) ==
        UMI_STATUS_OK);
    assert(umi_decl_component_registry_register(registry, &descriptor) ==
        UMI_STATUS_OK);

    assert(umi_designer_palette_build(registry, NULL, &palette) ==
        UMI_STATUS_OK);
    assert(umi_designer_palette_search(&palette, "btn", &results) ==
        UMI_STATUS_OK);
    assert(results.count == 1U);
    assert(umi_designer_palette_result(&palette, &results, 0U) != NULL);

    assert(umi_designer_inspector_schema(registry, "button", &schema) ==
        UMI_STATUS_OK);
    assert(schema.property_count == 1U);
    assert(umi_designer_inspector_property(&schema, "visible") != NULL);
    assert(umi_designer_inspector_validate_property(
        registry, "button", "visible", "false") == UMI_STATUS_OK);

    umi_designer_palette_dispose(&palette);
    umi_decl_component_registry_destroy(registry);
}

static void test_surface_math(void)
{
    UmiDesignerSurfaceOptions options;
    UmiDesignerRect snapped;
    UmiDesignerRect align_rects[3] = {
        {11, 20, 40, 20},
        {33, 50, 20, 20},
        {58, 80, 30, 20}
    };
    UmiDesignerRect distribute_rects[3] = {
        {0, 0, 10, 10},
        {70, 0, 10, 10},
        {100, 0, 10, 10}
    };

    umi_designer_surface_options_init(&options);
    assert(options.grid_size == 8U);
    snapped = umi_designer_surface_snap_rect(
        &options, (UmiDesignerRect){7, 9, 121, 39});
    assert(snapped.x == 8);
    assert(snapped.y == 8);
    assert(snapped.width == 120);
    assert(snapped.height == 40);

    assert(umi_designer_surface_align(
        align_rects, 3U, UMI_DESIGNER_ALIGN_LEFT) == UMI_STATUS_OK);
    assert(align_rects[0].x == align_rects[1].x);
    assert(align_rects[1].x == align_rects[2].x);

    assert(umi_designer_surface_distribute(
        distribute_rects, 3U, UMI_DESIGNER_DISTRIBUTE_HORIZONTAL) ==
        UMI_STATUS_OK);
    assert(distribute_rects[1].x == 50);
}

static void test_preview_profiles(void)
{
    UmiDesignerPreviewViewport viewport;

    umi_designer_preview_viewport_init(&viewport);
    assert(viewport.rect.width == 1440);
    assert(viewport.rect.height == 900);

    assert(umi_designer_preview_apply_profile(
        &viewport, UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT) == UMI_STATUS_OK);
    assert(viewport.rect.width == 390);
    assert(viewport.rect.height == 844);
    assert(strcmp(umi_designer_preview_profile_text(viewport.profile),
                  "Phone Portrait") == 0);

    assert(umi_designer_preview_set_custom(&viewport, 1600, 1000) ==
        UMI_STATUS_OK);
    assert(viewport.profile == UMI_DESIGNER_PREVIEW_CUSTOM);
}

int main(void)
{
    UmiDeclComponentRegistry *registry = NULL;
    UmiDeclSchema schema;
    UmiDesignerDocument *document = NULL;
    UmiDeclNode root;
    UmiDeclDiagnosticList diagnostics;
    char preview[1024];

    assert(umi_designer_catalogue_create(&registry, &schema) ==
        UMI_STATUS_OK);
    assert(umi_designer_document_create("org.umicom.demo", &document) ==
        UMI_STATUS_OK);
    assert(umi_decl_node_init(&root, "root", "window", "-") ==
        UMI_STATUS_OK);
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document), &root) == UMI_STATUS_OK);
    assert(umi_designer_preview(
        document, &schema, preview, sizeof(preview), &diagnostics) ==
        UMI_STATUS_OK);
    assert(strstr(preview, "root:window") != NULL);

    test_palette_and_inspector();
    test_surface_math();
    test_preview_profiles();

    umi_designer_document_destroy(document);
    umi_decl_component_registry_destroy(registry);
    return EXIT_SUCCESS;
}
