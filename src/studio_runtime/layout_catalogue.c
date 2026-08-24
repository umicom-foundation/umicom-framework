/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_catalogue.c
 *
 * PURPOSE:
 *   Implement Studio layout preset discovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_catalogue.h"

#include <string.h>

typedef const UmiStudioRuntimeLayoutPresetDefinition *(*PresetFactory)(void);

static const PresetFactory FACTORIES[] = {
    umi_studio_layout_preset_default,
    umi_studio_layout_preset_debugging,
    umi_studio_layout_preset_testing,
    umi_studio_layout_preset_source_control,
    umi_studio_layout_preset_ai_development,
    umi_studio_layout_preset_review,
    umi_studio_layout_preset_zen
};

size_t umi_studio_layout_catalogue_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_at(size_t index)
{
    return index < umi_studio_layout_catalogue_count()
        ? FACTORIES[index]()
        : NULL;
}

const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_find(const char *preset_id)
{
    size_t index;

    if (preset_id == NULL) return NULL;

    for (index = 0U; index < umi_studio_layout_catalogue_count(); ++index) {
        const UmiStudioRuntimeLayoutPresetDefinition *preset = FACTORIES[index]();

        if (preset != NULL && strcmp(preset->preset_id, preset_id) == 0) {
            return preset;
        }
    }

    return NULL;
}
