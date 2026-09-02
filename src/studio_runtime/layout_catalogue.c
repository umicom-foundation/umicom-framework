/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_catalogue.c
 *
 * PURPOSE:
 *   Implement Studio layout preset discovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    umi_studio_layout_preset_zen,
    umi_studio_layout_preset_learning,
    umi_studio_layout_preset_visual_design,
    umi_studio_layout_preset_code_preview
};

/*
 * Return the number of records represented by studio layout catalogue without changing
 * their state.
 */
size_t umi_studio_layout_catalogue_count(void)
{
    return sizeof(FACTORIES) / sizeof(FACTORIES[0]);
}

/*
 * Find studio layout catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_at(size_t index)
{
    return index < umi_studio_layout_catalogue_count()
        ? FACTORIES[index]()
        : NULL;
}

/*
 * Find studio layout catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_find(const char *preset_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preset_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_studio_layout_catalogue_count(); ++index) {
        const UmiStudioRuntimeLayoutPresetDefinition *preset = FACTORIES[index]();

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (preset != NULL && strcmp(preset->preset_id, preset_id) == 0) {
            return preset;
        }
    }

    return NULL;
}
