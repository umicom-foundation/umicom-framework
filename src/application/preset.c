/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/preset.c
 *
 * PURPOSE:
 *   Publish reusable Framework-first application archetypes for rapid creation
 *   of console tools, desktop clients, data applications, IDE workbenches, AI
 *   applications, web services and enterprise integration products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/preset.h"

#include <string.h>

#define ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

static const char *const CONSOLE_PACKS[] = {
    "umicom.pack.core-runtime"
};

static const char *const DESKTOP_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.shell"
};

static const char *const DATA_APP_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.shell",
    "umicom.pack.data"
};

static const char *const DEVELOPER_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.shell",
    "umicom.pack.data",
    "umicom.pack.developer",
    "umicom.pack.designer",
    "umicom.pack.ai-assistant",
    "umicom.pack.delivery"
};

static const char *const AI_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.shell",
    "umicom.pack.data",
    "umicom.pack.ai-assistant"
};

static const char *const WEB_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.data",
    "umicom.pack.web-service"
};

static const char *const WORKER_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.data",
    "umicom.pack.integration"
};

static const char *const ENTERPRISE_PACKS[] = {
    "umicom.pack.core-runtime",
    "umicom.pack.shell",
    "umicom.pack.data",
    "umicom.pack.integration",
    "umicom.pack.delivery"
};

static const UmiApplicationPresetDefinition PRESETS[] = {
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.console-tool",
        "Console Tool",
        "Small standalone/federated native tool with the complete core runtime.",
        UMI_APPLICATION_FAMILY_PLATFORM,
        UMI_FRONTEND_CONSOLE,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_HEADLESS_SAFE,
        CONSOLE_PACKS,
        ARRAY_COUNT(CONSOLE_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.desktop-client",
        "Desktop Client",
        "GTK4-first thin desktop client composed from Framework shell components.",
        UMI_APPLICATION_FAMILY_ENTERPRISE,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_MULTI_WINDOW |
            UMI_APPLICATION_MULTI_MONITOR,
        DESKTOP_PACKS,
        ARRAY_COUNT(DESKTOP_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.data-application",
        "Data Application",
        "Desktop application with Framework Data Server, migrations and repositories.",
        UMI_APPLICATION_FAMILY_ENTERPRISE,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_MULTI_WINDOW,
        DATA_APP_PACKS,
        ARRAY_COUNT(DATA_APP_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.developer-workbench",
        "Developer Workbench",
        "IDE-class application using shared build, test, debug, source-control, AI and delivery services.",
        UMI_APPLICATION_FAMILY_DEVELOPMENT,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_MULTI_WINDOW |
            UMI_APPLICATION_MULTI_MONITOR,
        DEVELOPER_PACKS,
        ARRAY_COUNT(DEVELOPER_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.ai-workbench",
        "AI Workbench",
        "Provider-neutral AI/RAG application with governed tools and reusable shell.",
        UMI_APPLICATION_FAMILY_AI,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_MULTI_WINDOW,
        AI_PACKS,
        ARRAY_COUNT(AI_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.web-service",
        "Web Service",
        "Headless HTTP/data service with security, metrics and Framework runtime.",
        UMI_APPLICATION_FAMILY_ENTERPRISE,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_WEB,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_HEADLESS_SAFE,
        WEB_PACKS,
        ARRAY_COUNT(WEB_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.integration-worker",
        "Integration Worker",
        "Headless message/workflow worker with durable Data Server integration.",
        UMI_APPLICATION_FAMILY_ENTERPRISE,
        UMI_FRONTEND_CONSOLE,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_HEADLESS_SAFE,
        WORKER_PACKS,
        ARRAY_COUNT(WORKER_PACKS)
    },
    {
        (uint32_t)sizeof(UmiApplicationPresetDefinition),
        UMI_APPLICATION_PRESET_API_VERSION,
        "umicom.preset.enterprise-workbench",
        "Enterprise Workbench",
        "Reusable shell, data, integration and delivery foundation for large operational applications.",
        UMI_APPLICATION_FAMILY_ENTERPRISE,
        UMI_FRONTEND_CONSOLE | UMI_FRONTEND_GTK4 | UMI_FRONTEND_WEB,
        UMI_APPLICATION_STANDALONE |
            UMI_APPLICATION_FEDERATED |
            UMI_APPLICATION_MULTI_WINDOW |
            UMI_APPLICATION_MULTI_MONITOR,
        ENTERPRISE_PACKS,
        ARRAY_COUNT(ENTERPRISE_PACKS)
    }
};

#undef ARRAY_COUNT

size_t umi_application_preset_catalogue_count(void)
{
    return sizeof(PRESETS) / sizeof(PRESETS[0]);
}

const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_at(size_t index)
{
    return index < umi_application_preset_catalogue_count()
        ? &PRESETS[index]
        : NULL;
}

const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_find(const char *preset_id)
{
    size_t index;

    if (preset_id == NULL) return NULL;

    for (index = 0U;
         index < umi_application_preset_catalogue_count();
         ++index) {
        if (strcmp(PRESETS[index].preset_id, preset_id) == 0) {
            return &PRESETS[index];
        }
    }

    return NULL;
}

UmiStatus umi_application_preset_validate(
    const UmiApplicationPresetDefinition *preset)
{
    size_t index;

    if (preset == NULL ||
        preset->structure_size != sizeof(*preset) ||
        preset->api_version != UMI_APPLICATION_PRESET_API_VERSION ||
        preset->preset_id == NULL || preset->preset_id[0] == '\0' ||
        preset->title == NULL || preset->title[0] == '\0' ||
        preset->description == NULL || preset->description[0] == '\0' ||
        preset->family < UMI_APPLICATION_FAMILY_PLATFORM ||
        preset->family > UMI_APPLICATION_FAMILY_EDUCATION ||
        preset->recommended_frontends == UMI_FRONTEND_NONE ||
        preset->feature_pack_ids == NULL ||
        preset->feature_pack_count == 0U ||
        preset->feature_pack_count > UMI_APPLICATION_PRESET_MAX_PACKS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < preset->feature_pack_count; ++index) {
        const UmiApplicationFeaturePackDefinition *pack =
            umi_application_feature_pack_catalogue_find(
                preset->feature_pack_ids[index]);

        if (pack == NULL) return UMI_STATUS_NOT_FOUND;
        if (umi_application_feature_pack_validate(pack) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_STATE;
        }
    }

    return UMI_STATUS_OK;
}
