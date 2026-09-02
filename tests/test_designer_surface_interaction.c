/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_surface_interaction.c
 *
 * PURPOSE:
 *   Verify hit-testing, snapping, canvas limits, resize limits and undoable
 *   direct manipulation without starting a graphical application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/umicom.h"

/* Create one movable component and exercise a complete gesture lifecycle. */
int main(void)
{
    UmiDesignerDocument *document = NULL;
    UmiDesignerHistory *history = NULL;
    UmiDeclNode root;
    UmiDeclNode button;
    UmiDeclNode metadata;
    UmiDesignerSurfaceOptions options;
    UmiDesignerSurfaceHit hit;
    UmiDesignerSurfaceInteraction interaction;
    UmiDesignerRect canvas = {0, 0, 320, 240};
    UmiDesignerRect rect = {16, 24, 96, 40};
    UmiDesignerRect current;

    assert(umi_designer_document_create(
        "org.umicom.designer.interaction",
        &document) == UMI_STATUS_OK);
    assert(umi_decl_node_init(&root, "root", "window", "-") ==
        UMI_STATUS_OK);
    assert(umi_decl_node_init(&button, "action", "button", "root") ==
        UMI_STATUS_OK);
    assert(umi_decl_node_init(&metadata, "notes", "metadata", "root") ==
        UMI_STATUS_OK);
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document),
        &root) == UMI_STATUS_OK);
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document),
        &button) == UMI_STATUS_OK);
    /* A later non-visual node must not hide a drawable component below it. */
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document),
        &metadata) == UMI_STATUS_OK);
    assert(umi_designer_surface_set_rect(document, "action", rect) ==
        UMI_STATUS_OK);
    assert(umi_designer_history_create(document, &history) == UMI_STATUS_OK);
    umi_designer_surface_options_init(&options);

    /* The later child is hit before its overlapping parent and its right edge
     * becomes a resize handle. */
    assert(umi_designer_surface_hit_test(
        document,
        110,
        44,
        UMI_DESIGNER_SURFACE_DEFAULT_HANDLE_SIZE,
        &hit) == UMI_STATUS_OK);
    assert(hit.found);
    assert(strcmp(hit.node_id, "action") == 0);
    assert((hit.edges & UMI_DESIGNER_SURFACE_EDGE_RIGHT) != 0U);

    /* Moving by an uneven distance snaps to the eight-pixel grid and records
     * only the final rectangle in history. */
    assert(umi_designer_surface_interaction_begin(
        document,
        "action",
        UMI_DESIGNER_SURFACE_INTERACTION_MOVE,
        UMI_DESIGNER_SURFACE_EDGE_NONE,
        40,
        40,
        canvas,
        &options,
        &interaction) == UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_update(
        &interaction,
        59,
        51) == UMI_STATUS_OK);
    assert(interaction.preview.x == 32);
    assert(interaction.preview.y == 32);
    assert(umi_designer_surface_interaction_commit(
        &interaction,
        document,
        history) == UMI_STATUS_OK);
    assert(umi_designer_surface_get_rect(document, "action", &current) ==
        UMI_STATUS_OK);
    assert(current.x == 32 && current.y == 32);
    assert(umi_designer_history_undo_count(history) == 1U);
    assert(umi_designer_history_undo(history) == UMI_STATUS_OK);
    assert(umi_designer_surface_get_rect(document, "action", &current) ==
        UMI_STATUS_OK);
    assert(current.x == rect.x && current.y == rect.y);
    assert(umi_designer_history_redo(history) == UMI_STATUS_OK);

    /* Resizing the right edge snaps that edge while the left edge stays fixed. */
    assert(umi_designer_surface_interaction_begin(
        document,
        "action",
        UMI_DESIGNER_SURFACE_INTERACTION_RESIZE,
        UMI_DESIGNER_SURFACE_EDGE_RIGHT,
        128,
        48,
        canvas,
        &options,
        &interaction) == UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_update(
        &interaction,
        141,
        48) == UMI_STATUS_OK);
    assert(interaction.preview.x == 32);
    assert(interaction.preview.width == 112);
    umi_designer_surface_interaction_cancel(&interaction);

    /* Dragging a left edge past the right edge keeps the component usable by
     * applying the minimum width while preserving the right edge. */
    assert(umi_designer_surface_interaction_begin(
        document,
        "action",
        UMI_DESIGNER_SURFACE_INTERACTION_RESIZE,
        UMI_DESIGNER_SURFACE_EDGE_LEFT,
        32,
        48,
        canvas,
        &options,
        &interaction) == UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_update(
        &interaction,
        300,
        48) == UMI_STATUS_OK);
    assert(interaction.preview.width >=
        UMI_DESIGNER_SURFACE_DEFAULT_MINIMUM_SIZE);
    assert(interaction.preview.x >= canvas.x);
    umi_designer_surface_interaction_cancel(&interaction);
    assert(!interaction.active && !interaction.changed);

    /* A click without movement completes successfully and does not consume an
     * undo slot that should be reserved for a real edit. */
    assert(umi_designer_surface_interaction_begin(
        document,
        "action",
        UMI_DESIGNER_SURFACE_INTERACTION_MOVE,
        UMI_DESIGNER_SURFACE_EDGE_NONE,
        48,
        48,
        canvas,
        &options,
        &interaction) == UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_commit(
        &interaction,
        document,
        history) == UMI_STATUS_OK);
    assert(umi_designer_history_undo_count(history) == 1U);

    /* A gesture based on old geometry cannot overwrite a newer rectangle. */
    assert(umi_designer_surface_interaction_begin(
        document,
        "action",
        UMI_DESIGNER_SURFACE_INTERACTION_MOVE,
        UMI_DESIGNER_SURFACE_EDGE_NONE,
        48,
        48,
        canvas,
        &options,
        &interaction) == UMI_STATUS_OK);
    current.x = 64;
    assert(umi_designer_surface_set_rect(document, "action", current) ==
        UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_update(
        &interaction,
        56,
        48) == UMI_STATUS_OK);
    assert(umi_designer_surface_interaction_commit(
        &interaction,
        document,
        history) == UMI_STATUS_BUSY);
    umi_designer_surface_interaction_cancel(&interaction);

    /* Empty canvas space has no semantic target and returns a clear result. */
    assert(umi_designer_surface_hit_test(
        document,
        300,
        220,
        UMI_DESIGNER_SURFACE_DEFAULT_HANDLE_SIZE,
        &hit) == UMI_STATUS_NOT_FOUND);

    umi_designer_history_destroy(history);
    umi_designer_document_destroy(document);
    return EXIT_SUCCESS;
}
