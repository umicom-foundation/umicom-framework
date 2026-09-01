/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/palette.c
 *
 * PURPOSE:
 *   Build and search designer palettes from semantic metadata instead of
 *   hard-coded toolkit widget lists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Search ranking is intentionally toolkit-neutral. Studio can use GtkSearchEntry
 * while another product can present exactly the same results in a web UI.
 */

#include "umicom/designer/palette.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_designer_palette_build(
    const UmiDeclComponentRegistry *registry,
    const char *filter,
    UmiDesignerPalette *out_palette)
{
    size_t index;
    size_t matches = 0U;

    if (registry == NULL || out_palette == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_palette, 0, sizeof(*out_palette));

    for (index = 0U; index < umi_decl_component_registry_count(registry);
         ++index) {
        UmiDeclComponentDescriptor descriptor;

        if (umi_decl_component_registry_at(registry,
                                           index,
                                           &descriptor) != UMI_STATUS_OK) {
            continue;
        }

        if (filter == NULL || filter[0] == '\0' ||
            strcmp(filter, descriptor.category) == 0) {
            ++matches;
        }
    }

    if (matches == 0U) {
        return UMI_STATUS_OK;
    }

    out_palette->items = (UmiDeclComponentDescriptor *)calloc(
        matches,
        sizeof(*out_palette->items)
    );
    if (out_palette->items == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    for (index = 0U; index < umi_decl_component_registry_count(registry);
         ++index) {
        UmiDeclComponentDescriptor descriptor;

        if (umi_decl_component_registry_at(registry,
                                           index,
                                           &descriptor) != UMI_STATUS_OK) {
            continue;
        }

        if (filter != NULL && filter[0] != '\0' &&
            strcmp(filter, descriptor.category) != 0) {
            continue;
        }

        out_palette->items[out_palette->count++] = descriptor;
    }

    return UMI_STATUS_OK;
}

void umi_designer_palette_dispose(UmiDesignerPalette *palette)
{
    if (palette == NULL) {
        return;
    }

    free(palette->items);
    palette->items = NULL;
    palette->count = 0U;
}

static int fuzzy_text_score(const char *query, const char *candidate)
{
    size_t query_index = 0U;
    size_t candidate_index;
    int score = 0;
    int contiguous = 0;

    if (query == NULL || candidate == NULL) {
        return -1;
    }

    if (query[0] == '\0') {
        return 1;
    }

    for (candidate_index = 0U;
         candidate[candidate_index] != '\0' && query[query_index] != '\0';
         ++candidate_index) {
        unsigned char wanted =
            (unsigned char)tolower((unsigned char)query[query_index]);
        unsigned char actual =
            (unsigned char)tolower((unsigned char)candidate[candidate_index]);

        if (wanted == actual) {
            score += 10;
            if (candidate_index == 0U) {
                score += 8;
            }
            if (contiguous != 0) {
                score += 4;
            }
            contiguous = 1;
            ++query_index;
        } else {
            contiguous = 0;
            --score;
        }
    }

    return query[query_index] == '\0' ? score : -1;
}

int umi_designer_palette_score(
    const char *query,
    const UmiDeclComponentDescriptor *component)
{
    int type_score;
    int name_score;
    int category_score;
    int result;

    if (query == NULL || component == NULL) {
        return -1;
    }

    type_score = fuzzy_text_score(query, component->component_type);
    name_score = fuzzy_text_score(query, component->display_name);
    category_score = fuzzy_text_score(query, component->category);

    result = type_score;
    if (name_score > result) {
        result = name_score;
    }
    if (category_score > result) {
        result = category_score;
    }

    return result;
}

static void insert_match(UmiDesignerPaletteResults *results,
                         UmiDesignerPaletteMatch match)
{
    size_t position = 0U;
    size_t move_count;

    if (match.score < 0) {
        return;
    }

    while (position < results->count &&
           results->matches[position].score >= match.score) {
        ++position;
    }

    if (position >= UMI_DESIGNER_MAX_PALETTE_RESULTS) {
        return;
    }

    if (results->count < UMI_DESIGNER_MAX_PALETTE_RESULTS) {
        ++results->count;
    }

    move_count = results->count - position - 1U;
    if (move_count > 0U) {
        (void)memmove(&results->matches[position + 1U],
                      &results->matches[position],
                      move_count * sizeof(results->matches[0]));
    }

    results->matches[position] = match;
}

UmiStatus umi_designer_palette_search(
    const UmiDesignerPalette *palette,
    const char *query,
    UmiDesignerPaletteResults *out_results)
{
    size_t index;

    if (palette == NULL || query == NULL || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_results, 0, sizeof(*out_results));

    for (index = 0U; index < palette->count; ++index) {
        UmiDesignerPaletteMatch match;
        match.palette_index = index;
        match.score = umi_designer_palette_score(query, &palette->items[index]);
        insert_match(out_results, match);
    }

    return UMI_STATUS_OK;
}

const UmiDeclComponentDescriptor *umi_designer_palette_result(
    const UmiDesignerPalette *palette,
    const UmiDesignerPaletteResults *results,
    size_t result_index)
{
    size_t palette_index;

    if (palette == NULL || results == NULL ||
        result_index >= results->count) {
        return NULL;
    }

    palette_index = results->matches[result_index].palette_index;
    return palette_index < palette->count
        ? &palette->items[palette_index]
        : NULL;
}
