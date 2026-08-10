/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/preset.c
 *
 * PURPOSE:
 *   Implement preset defaults, validation and human-readable descriptions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/preset.h"

#include <stdio.h>
#include <string.h>

void umi_build_preset_init(UmiBuildPreset *preset,
                           const UmiBuildProfile *profile)
{
    if (preset == NULL) {
        return;
    }
    (void)memset(preset, 0, sizeof(*preset));
    if (profile != NULL) {
        preset->profile = *profile;
        (void)snprintf(preset->preset_id,
                       sizeof(preset->preset_id),
                       "%s",
                       profile->profile_id);
    } else {
        umi_build_profile_init(&preset->profile);
        (void)snprintf(preset->preset_id,
                       sizeof(preset->preset_id),
                       "%s",
                       preset->profile.profile_id);
    }
    preset->configure_enabled = 1;
    preset->build_enabled = 1;
    preset->test_enabled = 1;
}

UmiStatus umi_build_preset_validate(const UmiBuildPreset *preset,
                                    char *out_message,
                                    size_t message_capacity)
{
    if (preset == NULL || preset->preset_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!preset->configure_enabled &&
        !preset->build_enabled &&
        !preset->test_enabled) {
        if (out_message != NULL && message_capacity > 0U) {
            (void)snprintf(out_message,
                           message_capacity,
                           "%s",
                           "Preset does not enable any operation");
        }
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_build_profile_validate(&preset->profile,
                                      out_message,
                                      message_capacity);
}

UmiStatus umi_build_preset_describe(const UmiBuildPreset *preset,
                                    char *out_text,
                                    size_t capacity)
{
    int written;
    if (preset == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text,
                       capacity,
                       "%s: source=%s build=%s configuration=%s "
                       "configure=%d build=%d test=%d clean-first=%d",
                       preset->preset_id,
                       preset->profile.source_directory,
                       preset->profile.build_directory,
                       preset->profile.configuration,
                       preset->configure_enabled,
                       preset->build_enabled,
                       preset->test_enabled,
                       preset->clean_first);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
