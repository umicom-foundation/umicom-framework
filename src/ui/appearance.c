/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance.c
 *
 * PURPOSE:
 *   Implement validated appearance profiles and the thread-safe catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/appearance.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiAppearanceModel {
    UmiUiAppearanceProfile items[UMI_UI_APPEARANCE_MAX_PROFILES];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

static void set_umicom_palette(UmiUiAppearanceProfile *profile)
{
    UmiUiBrandSurface surface = UMI_UI_BRAND_SURFACE_DARK;
    const UmiUiBrandPalette *palette;

    if (profile == NULL) return;

    /* System mode begins with the same neutral light values used historically.
     * A graphical adapter may replace them after reading the system setting. */
    if (profile->mode == UMI_UI_THEME_MODE_LIGHT ||
        profile->mode == UMI_UI_THEME_MODE_SYSTEM) {
        surface = UMI_UI_BRAND_SURFACE_LIGHT;
    } else if (profile->mode == UMI_UI_THEME_MODE_HIGH_CONTRAST) {
        surface = UMI_UI_BRAND_SURFACE_HIGH_CONTRAST;
    }

    palette = umi_ui_brand_palette_get(surface);
    if (umi_ui_brand_palette_validate(palette) != UMI_STATUS_OK) return;

    /* Copying the complete palette here gives existing appearance consumers
     * their fixed buffers while Framework retains one canonical colour source. */
    copy_text(profile->background, sizeof(profile->background), palette->background);
    copy_text(profile->surface, sizeof(profile->surface), palette->surface);
    copy_text(profile->raised_surface, sizeof(profile->raised_surface), palette->surface_raised);
    copy_text(profile->hover_surface, sizeof(profile->hover_surface), palette->surface_hover);
    copy_text(profile->editor_background, sizeof(profile->editor_background), palette->editor);
    copy_text(profile->foreground, sizeof(profile->foreground), palette->foreground);
    copy_text(profile->muted_foreground, sizeof(profile->muted_foreground), palette->foreground_muted);
    copy_text(profile->border, sizeof(profile->border), palette->border);
    copy_text(profile->strong_border, sizeof(profile->strong_border), palette->border_strong);
    copy_text(profile->accent, sizeof(profile->accent), palette->accent);
    copy_text(profile->accent_surface, sizeof(profile->accent_surface), palette->accent_surface);
    copy_text(profile->success, sizeof(profile->success), palette->success);
    copy_text(profile->warning, sizeof(profile->warning), palette->warning);
    copy_text(profile->danger, sizeof(profile->danger), palette->danger);
}

UmiStatus umi_ui_appearance_profile_init(
    UmiUiAppearanceProfile *profile,
    const char *profile_id,
    const char *label,
    UmiUiThemeMode mode,
    UmiUiDensity density)
{
    if (profile == NULL || !umi_ui_id_is_valid(profile_id) || label == NULL ||
        label[0] == '\0' || strlen(label) >= sizeof(profile->label) ||
        mode < UMI_UI_THEME_MODE_SYSTEM ||
        mode > UMI_UI_THEME_MODE_HIGH_CONTRAST ||
        density < UMI_UI_DENSITY_COMPACT ||
        density > UMI_UI_DENSITY_SPACIOUS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(profile, 0, sizeof(*profile));
    copy_text(profile->profile_id, sizeof(profile->profile_id), profile_id);
    copy_text(profile->label, sizeof(profile->label), label);
    profile->mode = mode;
    profile->density = density;
    copy_text(profile->brand_name, sizeof(profile->brand_name), "Umicom");
    copy_text(profile->interface_font, sizeof(profile->interface_font),
              "Segoe UI, Inter, Sans");
    copy_text(profile->editor_font, sizeof(profile->editor_font),
              "Cascadia Code, JetBrains Mono, Monospace");
    profile->interface_font_size = 10.0;
    profile->editor_font_size = 11.0;
    profile->font_scale = 1.0;
    profile->reduce_motion = mode == UMI_UI_THEME_MODE_HIGH_CONTRAST;
    profile->revision = 1U;
    set_umicom_palette(profile);
    return UMI_STATUS_OK;
}

static int valid_colour(const char *colour)
{
    size_t index;
    if (colour == NULL || colour[0] != '#' || strlen(colour) != 7U) return 0;
    for (index = 1U; index < 7U; ++index) {
        if (!isxdigit((unsigned char)colour[index])) return 0;
    }
    return 1;
}

static int valid_font(const char *font)
{
    const unsigned char *cursor = (const unsigned char *)font;
    if (font == NULL || font[0] == '\0') return 0;
    while (*cursor != '\0') {
        if (!isalnum(*cursor) && *cursor != ' ' && *cursor != ',' &&
            *cursor != '-' && *cursor != '_' && *cursor != '.') {
            return 0;
        }
        ++cursor;
    }
    return 1;
}

UmiStatus umi_ui_appearance_profile_validate(
    const UmiUiAppearanceProfile *profile,
    char *out_reason,
    size_t capacity)
{
#define UMI_COLOUR_OK(field) valid_colour(profile->field)
    const char *reason = "Appearance profile is valid";
    UmiStatus result = UMI_STATUS_OK;
    int written;

    if (profile == NULL || out_reason == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_ui_id_is_valid(profile->profile_id) || profile->label[0] == '\0') {
        reason = "Appearance profile identity is invalid";
        result = UMI_STATUS_INVALID_STATE;
    } else if (profile->mode < UMI_UI_THEME_MODE_SYSTEM ||
               profile->mode > UMI_UI_THEME_MODE_HIGH_CONTRAST ||
               profile->density < UMI_UI_DENSITY_COMPACT ||
               profile->density > UMI_UI_DENSITY_SPACIOUS) {
        reason = "Appearance mode or density is invalid";
        result = UMI_STATUS_INVALID_STATE;
    } else if (!valid_font(profile->interface_font) ||
               !valid_font(profile->editor_font)) {
        reason = "Font families contain unsupported characters";
        result = UMI_STATUS_INVALID_STATE;
    } else if (profile->interface_font_size < 8.0 ||
               profile->interface_font_size > 32.0 ||
               profile->editor_font_size < 8.0 ||
               profile->editor_font_size > 40.0 ||
               profile->font_scale < 0.75 || profile->font_scale > 2.0) {
        reason = "Font sizes or scale are outside supported limits";
        result = UMI_STATUS_INVALID_STATE;
    } else if (!UMI_COLOUR_OK(background) || !UMI_COLOUR_OK(surface) ||
               !UMI_COLOUR_OK(raised_surface) ||
               !UMI_COLOUR_OK(hover_surface) ||
               !UMI_COLOUR_OK(editor_background) ||
               !UMI_COLOUR_OK(foreground) ||
               !UMI_COLOUR_OK(muted_foreground) || !UMI_COLOUR_OK(border) ||
               !UMI_COLOUR_OK(strong_border) || !UMI_COLOUR_OK(accent) ||
               !UMI_COLOUR_OK(accent_surface) || !UMI_COLOUR_OK(success) ||
               !UMI_COLOUR_OK(warning) || !UMI_COLOUR_OK(danger)) {
        reason = "Every semantic colour must use #RRGGBB notation";
        result = UMI_STATUS_INVALID_STATE;
    }

    written = snprintf(out_reason, capacity, "%s", reason);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return result;
#undef UMI_COLOUR_OK
}

static size_t find_profile(const UmiUiAppearanceModel *model,
                           const char *profile_id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].profile_id, profile_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_appearance_model_create(UmiUiAppearanceModel **out_model)
{
    UmiUiAppearanceModel *model;
    UmiStatus status;
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiAppearanceModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&model->mutex);
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_ui_appearance_model_destroy(UmiUiAppearanceModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_appearance_model_upsert(
    UmiUiAppearanceModel *model,
    const UmiUiAppearanceProfile *profile)
{
    char reason[192U];
    size_t index;
    size_t other;
    if (model == NULL || profile == NULL ||
        umi_ui_appearance_profile_validate(profile, reason, sizeof(reason)) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile->profile_id);
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_APPEARANCE_MAX_PROFILES) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    } else if (model->items[index].built_in || model->items[index].locked) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    model->items[index] = *profile;
    model->items[index].active = profile->active != 0;
    model->items[index].built_in = profile->built_in != 0;
    model->items[index].locked = profile->built_in || profile->locked != 0;
    model->items[index].reduce_motion = profile->reduce_motion != 0;
    if (model->items[index].active) {
        for (other = 0U; other < model->count; ++other) {
            if (other != index) model->items[other].active = 0;
        }
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_model_remove(
    UmiUiAppearanceModel *model,
    const char *profile_id)
{
    size_t index;
    if (model == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (model->items[index].built_in || model->items[index].locked) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (model->items[index].active) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index], &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(model->items[0]));
    }
    --model->count;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_model_find(
    const UmiUiAppearanceModel *model,
    const char *profile_id,
    UmiUiAppearanceProfile *out_profile)
{
    size_t index;
    if (model == NULL || profile_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_profile = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_model_at(
    const UmiUiAppearanceModel *model,
    size_t index,
    UmiUiAppearanceProfile *out_profile)
{
    if (model == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_profile = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_model_active(
    const UmiUiAppearanceModel *model,
    UmiUiAppearanceProfile *out_profile)
{
    size_t index;
    if (model == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    for (index = 0U; index < model->count; ++index) {
        if (model->items[index].active) {
            *out_profile = model->items[index];
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_ui_appearance_model_set_active(
    UmiUiAppearanceModel *model,
    const char *profile_id)
{
    size_t index;
    size_t selected;
    if (model == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    selected = find_profile(model, profile_id);
    if (selected == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < model->count; ++index) {
        model->items[index].active = index == selected;
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_model_clone(
    UmiUiAppearanceModel *model,
    const char *source_profile_id,
    const char *new_profile_id,
    const char *new_label)
{
    UmiUiAppearanceProfile clone;
    size_t source;
    if (model == NULL || source_profile_id == NULL ||
        !umi_ui_id_is_valid(new_profile_id) || new_label == NULL ||
        new_label[0] == '\0' || strlen(new_label) >= sizeof(clone.label)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    source = find_profile(model, source_profile_id);
    if (source == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (find_profile(model, new_profile_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    clone = model->items[source];
    (void)umi_mutex_unlock(model->mutex);

    copy_text(clone.profile_id, sizeof(clone.profile_id), new_profile_id);
    copy_text(clone.label, sizeof(clone.label), new_label);
    clone.active = 0;
    clone.built_in = 0;
    clone.locked = 0;
    clone.revision = umi_ui_next_revision(clone.revision);
    return umi_ui_appearance_model_upsert(model, &clone);
}

UmiStatus umi_ui_appearance_model_set_locked(
    UmiUiAppearanceModel *model,
    const char *profile_id,
    int locked)
{
    size_t index;
    if (model == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (model->items[index].built_in) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_PERMISSION_DENIED;
    }
    model->items[index].locked = locked != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_appearance_model_count(const UmiUiAppearanceModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_appearance_model_revision(const UmiUiAppearanceModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
