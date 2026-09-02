/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/feature_pack.c
 *
 * PURPOSE:
 *   Publish reusable cross-application feature packs assembled exclusively from
 *   existing Framework capabilities and component catalogue entries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/feature_pack.h"

#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/runtime/capability_catalogue.h"

#define ARRAY_COUNT(values) (sizeof(values) / sizeof((values)[0]))

static const char *const CORE_REQUIRED[] = {
    "umicom.runtime",
    "umicom.messaging",
    "umicom.configuration",
    "umicom.settings",
    "umicom.filesystem",
    "umicom.process",
    "umicom.security",
    "umicom.diagnostics",
    "umicom.resilience",
    "umicom.metrics",
    "umicom.tracing",
    "umicom.application"
};

static const char *const SHELL_REQUIRED[] = {
    "umicom.ui",
    "umicom.workspace",
    "umicom.documents",
    "umicom.application.context",
    "umicom.application.components",
    "umicom.desktop.layouts"
};

static const char *const SHELL_COMPONENTS[] = {
    "umicom.shell.activity-bar",
    "umicom.shell.command-palette",
    "umicom.shell.dock-canvas",
    "umicom.shell.layout-tabs",
    "umicom.shell.workspace-profiles",
    "umicom.shell.multi-monitor",
    "umicom.shell.context-links",
    "umicom.shell.notifications"
};

static const char *const DATA_REQUIRED[] = {
    "umicom.data",
    "umicom.data.transactions",
    "umicom.data.migrations",
    "umicom.data.repositories",
    "umicom.data.durable-messages",
    "umicom.messaging.reliability"
};

static const char *const DEVELOPER_REQUIRED[] = {
    "umicom.build",
    "umicom.toolchain.discovery",
    "umicom.toolchain.environment",
    "umicom.toolchain.build",
    "umicom.protocol",
    "umicom.vcs",
    "umicom.tasks",
    "umicom.testing",
    "umicom.debug",
    "umicom.workspace",
    "umicom.documents"
};

static const char *const DEVELOPER_OPTIONAL[] = {
    "umicom.codeguard",
    "umicom.abi",
    "umicom.sdk",
    "umicom.delivery"
};

static const char *const DEVELOPER_COMPONENTS[] = {
    "umicom.development.explorer",
    "umicom.development.editor",
    "umicom.development.build",
    "umicom.development.debug",
    "umicom.development.testing",
    "umicom.development.source-control",
    "umicom.development.terminal"
};

static const char *const DESIGNER_REQUIRED[] = {
    "umicom.declarative",
    "umicom.designer",
    "umicom.ui"
};

static const char *const DESIGNER_COMPONENTS[] = {
    "umicom.development.designer"
};

static const char *const AI_REQUIRED[] = {
    "umicom.ai",
    "umicom.ai.provider",
    "umicom.ai.retrieval",
    "umicom.ai.tools",
    "umicom.ai.coding-assistant"
};

static const char *const AI_OPTIONAL[] = {
    "umicom.helix",
    "umicom.ai.authorengine"
};

static const char *const AI_COMPONENTS[] = {
    "umicom.ai.chat",
    "umicom.ai.context",
    "umicom.ai.models",
    "umicom.ai.tools",
    "umicom.ai.evaluation"
};

static const char *const WEB_REQUIRED[] = {
    "umicom.web",
    "umicom.security",
    "umicom.metrics",
    "umicom.process"
};

static const char *const INTEGRATION_REQUIRED[] = {
    "umicom.integration",
    "umicom.messaging",
    "umicom.messaging.reliability",
    "umicom.messaging.workflows",
    "umicom.data"
};

static const char *const DELIVERY_REQUIRED[] = {
    "umicom.delivery",
    "umicom.sdk",
    "umicom.abi",
    "umicom.codeguard",
    "umicom.build",
    "umicom.security"
};

static const char *const DESKTOP_REQUIRED[] = {
    "umicom.desktop",
    "umicom.desktop.layouts",
    "umicom.desktop.windows",
    "umicom.desktop.monitors",
    "umicom.desktop.context",
    "umicom.desktop.persistence",
    "umicom.desktop.shell",
    "umicom.application.context",
    "umicom.application.components"
};

static const UmiApplicationFeaturePackDefinition PACKS[] = {
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.core-runtime",
        "Core Runtime",
        "Lifecycle, messaging, configuration, files, processes, security, diagnostics and resilience.",
        CORE_REQUIRED, ARRAY_COUNT(CORE_REQUIRED),
        NULL, 0U,
        NULL, 0U,
        UMI_APPLICATION_FEATURE_PACK_HEADLESS |
            UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.shell",
        "Application Shell",
        "Reusable professional shell, workspace, layout and context-link surfaces.",
        SHELL_REQUIRED, ARRAY_COUNT(SHELL_REQUIRED),
        NULL, 0U,
        SHELL_COMPONENTS, ARRAY_COUNT(SHELL_COMPONENTS),
        UMI_APPLICATION_FEATURE_PACK_PRESENTATION |
            UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.data",
        "Data Platform",
        "Transactions, migrations, repositories, durable messaging and reliable persistence.",
        DATA_REQUIRED, ARRAY_COUNT(DATA_REQUIRED),
        NULL, 0U,
        NULL, 0U,
        UMI_APPLICATION_FEATURE_PACK_DATA |
            UMI_APPLICATION_FEATURE_PACK_HEADLESS
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.developer",
        "Developer Platform",
        "Build, test, debug, source control, protocol, workspace and toolchain capabilities.",
        DEVELOPER_REQUIRED, ARRAY_COUNT(DEVELOPER_REQUIRED),
        DEVELOPER_OPTIONAL, ARRAY_COUNT(DEVELOPER_OPTIONAL),
        DEVELOPER_COMPONENTS, ARRAY_COUNT(DEVELOPER_COMPONENTS),
        UMI_APPLICATION_FEATURE_PACK_DEVELOPMENT |
            UMI_APPLICATION_FEATURE_PACK_PRESENTATION |
            UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.designer",
        "Visual Designer",
        "Declarative application modelling, visual composition and preview foundation.",
        DESIGNER_REQUIRED, ARRAY_COUNT(DESIGNER_REQUIRED),
        NULL, 0U,
        DESIGNER_COMPONENTS, ARRAY_COUNT(DESIGNER_COMPONENTS),
        UMI_APPLICATION_FEATURE_PACK_DEVELOPMENT |
            UMI_APPLICATION_FEATURE_PACK_PRESENTATION
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.ai-assistant",
        "AI Assistant",
        "Provider-neutral AI, retrieval, governed tools and repository-aware coding assistance.",
        AI_REQUIRED, ARRAY_COUNT(AI_REQUIRED),
        AI_OPTIONAL, ARRAY_COUNT(AI_OPTIONAL),
        AI_COMPONENTS, ARRAY_COUNT(AI_COMPONENTS),
        UMI_APPLICATION_FEATURE_PACK_AI |
            UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.web-service",
        "Web Service",
        "HTTP, routing, security, metrics and supervised process foundation.",
        WEB_REQUIRED, ARRAY_COUNT(WEB_REQUIRED),
        NULL, 0U,
        NULL, 0U,
        UMI_APPLICATION_FEATURE_PACK_HEADLESS |
            UMI_APPLICATION_FEATURE_PACK_ENTERPRISE
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.integration",
        "Integration Fabric",
        "Message routing, workflows, reliability and Data Server-backed integration.",
        INTEGRATION_REQUIRED, ARRAY_COUNT(INTEGRATION_REQUIRED),
        NULL, 0U,
        NULL, 0U,
        UMI_APPLICATION_FEATURE_PACK_ENTERPRISE |
            UMI_APPLICATION_FEATURE_PACK_HEADLESS
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.delivery",
        "Delivery and Release",
        "Build evidence, SDK/ABI checks, CodeGuard, packaging, signing and release lifecycle.",
        DELIVERY_REQUIRED, ARRAY_COUNT(DELIVERY_REQUIRED),
        NULL, 0U,
        NULL, 0U,
        UMI_APPLICATION_FEATURE_PACK_DELIVERY |
            UMI_APPLICATION_FEATURE_PACK_HEADLESS
    },
    {
        (uint32_t)sizeof(UmiApplicationFeaturePackDefinition),
        UMI_APPLICATION_FEATURE_PACK_API_VERSION,
        "umicom.pack.desktop-suite",
        "Federated Desktop",
        "Multi-application desktop, layouts, windows, monitors, context routing and persistence.",
        DESKTOP_REQUIRED, ARRAY_COUNT(DESKTOP_REQUIRED),
        NULL, 0U,
        SHELL_COMPONENTS, ARRAY_COUNT(SHELL_COMPONENTS),
        UMI_APPLICATION_FEATURE_PACK_PRESENTATION |
            UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE
    }
};

#undef ARRAY_COUNT

/* Provide the valid list operation used by this module and its client applications. */
static int valid_list(const char *const *items, size_t count)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (count > 0U && items == NULL) return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (items[index] == NULL || items[index][0] == '\0') return 0;
    }

    return 1;
}

/*
 * Return the number of records represented by application feature pack catalogue without
 * changing their state.
 */
size_t umi_application_feature_pack_catalogue_count(void)
{
    return sizeof(PACKS) / sizeof(PACKS[0]);
}

/*
 * Find application feature pack catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationFeaturePackDefinition *
umi_application_feature_pack_catalogue_at(size_t index)
{
    return index < umi_application_feature_pack_catalogue_count()
        ? &PACKS[index]
        : NULL;
}

/*
 * Find application feature pack catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationFeaturePackDefinition *
umi_application_feature_pack_catalogue_find(const char *pack_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_feature_pack_catalogue_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(PACKS[index].pack_id, pack_id) == 0) {
            return &PACKS[index];
        }
    }

    return NULL;
}

/*
 * Check that application feature pack satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_application_feature_pack_validate(
    const UmiApplicationFeaturePackDefinition *pack)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pack == NULL ||
        pack->structure_size != sizeof(*pack) ||
        pack->api_version != UMI_APPLICATION_FEATURE_PACK_API_VERSION ||
        pack->pack_id == NULL || pack->pack_id[0] == '\0' ||
        pack->title == NULL || pack->title[0] == '\0' ||
        pack->description == NULL || pack->description[0] == '\0' ||
        pack->required_capability_count == 0U ||
        pack->required_capability_count >
            UMI_APPLICATION_FEATURE_PACK_MAX_CAPABILITIES ||
        pack->optional_capability_count >
            UMI_APPLICATION_FEATURE_PACK_MAX_CAPABILITIES ||
        pack->component_count >
            UMI_APPLICATION_FEATURE_PACK_MAX_COMPONENTS ||
        !valid_list(pack->required_capabilities,
                    pack->required_capability_count) ||
        !valid_list(pack->optional_capabilities,
                    pack->optional_capability_count) ||
        !valid_list(pack->component_ids,
                    pack->component_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->required_capability_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_framework_capability_catalogue_find(
                pack->required_capabilities[index]) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->optional_capability_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_framework_capability_catalogue_find(
                pack->optional_capabilities[index]) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pack->component_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_component_catalogue_find(
                pack->component_ids[index]) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    return UMI_STATUS_OK;
}
