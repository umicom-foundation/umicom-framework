/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/layout_catalogue.c
 *
 * PURPOSE:
 *   Provide Framework-owned professional layouts for the complete application
 *   portfolio. Applications select and present these definitions; they do not
 *   duplicate window composition logic in their product repositories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/layout_catalogue.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"
#include "umicom/application/portfolio.h"

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static size_t find_layout_index(
    const UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id)
{
    size_t index;
    if (catalogue == NULL || layout_id == NULL) return SIZE_MAX;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->layouts[index].layout_id, layout_id) == 0)
            return index;
    }
    return SIZE_MAX;
}

static UmiStatus initialise_layout(
    UmiDesktopLayout *layout,
    const char *layout_id,
    const char *name,
    const char *category,
    const char *description)
{
    UmiStatus status;
    if (layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(layout, 0, sizeof(*layout));
    status = copy_text(layout->layout_id, sizeof(layout->layout_id), layout_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(layout->name, sizeof(layout->name), name);
    if (status == UMI_STATUS_OK)
        status = copy_text(layout->category, sizeof(layout->category), category);
    if (status == UMI_STATUS_OK)
        status = copy_text(layout->description, sizeof(layout->description),
                           description);
    if (status == UMI_STATUS_OK) {
        layout->built_in = true;
        layout->locked = true;
        layout->revision = 1U;
    }
    return status;
}

static UmiStatus add_window(
    UmiDesktopLayout *layout,
    const char *monitor_id,
    const char *window_id,
    const char *title,
    const char *component_id,
    const char *application_id,
    const char *context_group_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement,
    bool closable,
    int32_t z_order)
{
    UmiDesktopWindow *window;
    UmiStatus status;
    if (layout == NULL || layout->window_count >= UMI_DESKTOP_MAX_LAYOUT_WINDOWS)
        return layout == NULL ? UMI_STATUS_INVALID_ARGUMENT
                              : UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_application_component_catalogue_find(component_id) == NULL ||
        umi_application_portfolio_find(application_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    window = &layout->windows[layout->window_count];
    (void)memset(window, 0, sizeof(*window));
    status = copy_text(window->window_id, sizeof(window->window_id), window_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(window->title, sizeof(window->title), title);
    if (status == UMI_STATUS_OK)
        status = copy_text(window->component_id, sizeof(window->component_id),
                           component_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(window->owner_application_id,
                           sizeof(window->owner_application_id), application_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(window->monitor_id, sizeof(window->monitor_id),
                           monitor_id);
    if (status == UMI_STATUS_OK && context_group_id != NULL &&
        context_group_id[0] != '\0')
        status = copy_text(window->context_group_id,
                           sizeof(window->context_group_id), context_group_id);
    if (status != UMI_STATUS_OK) return status;
    window->bounds = bounds;
    window->dock_placement = placement;
    window->z_order = z_order;
    window->visible = true;
    window->closable = closable;
    window->resizable = true;
    layout->window_count += 1U;
    layout->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus add_seeded_layout(
    UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopLayout *layout)
{
    return umi_desktop_layout_catalogue_add(catalogue, layout);
}

static UmiStatus seed_develop(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "develop", "Develop", "Development",
        "Editor-first coding workspace with compact project, tool and output regions");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "project-explorer", "Project Explorer", "umicom.development.explorer",
        "org.umicom.studio", "project-blue", (UmiDesktopRect){0, 0, 346, 790},
        UMI_DESKTOP_DOCK_LEFT, false, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "editor-main", "Editor", "umicom.development.editor", "org.umicom.ide",
        "project-blue", (UmiDesktopRect){346, 0, 1190, 790},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "authorengine", "AI and AuthorEngine", "umicom.ai.chat", "org.umicom.llm",
        "project-blue", (UmiDesktopRect){1536, 0, 384, 790},
        UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "output", "Output, Problems and Terminal", "umicom.development.terminal",
        "org.umicom.studio", "run-green", (UmiDesktopRect){0, 790, 1920, 250},
        UMI_DESKTOP_DOCK_BOTTOM, true, 4);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_focus(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "focus", "Focus", "Development",
        "Distraction-free editor workspace for concentrated implementation and review");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "focus-editor", "Editor", "umicom.development.editor", "org.umicom.ide",
        "project-blue", (UmiDesktopRect){0, 0, 1920, 1040},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 1);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_debug(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "debug", "Debug", "Development",
        "Source, execution state and console workspace for debugging");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "debug-explorer", "Project Explorer", "umicom.development.explorer",
        "org.umicom.ide", "debug-orange", (UmiDesktopRect){0, 0, 346, 707},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "debug-editor", "Editor", "umicom.development.editor", "org.umicom.ide",
        "debug-orange", (UmiDesktopRect){346, 0, 1113, 707},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "debug-state", "Variables, Watch and Call Stack", "umicom.development.debug",
        "org.umicom.ide", "debug-orange", (UmiDesktopRect){1459, 0, 461, 707},
        UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "debug-console", "Debug Console, Breakpoints and Terminal",
        "umicom.development.terminal", "org.umicom.ide", "debug-orange",
        (UmiDesktopRect){0, 707, 1920, 333}, UMI_DESKTOP_DOCK_BOTTOM, true, 4);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_operations(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "operations", "Operations", "Operations",
        "Metrics, traces, profiler, health and resilience dashboard");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "metrics", "Metrics", "umicom.operations.metrics", "org.umicom.operations",
        "run-green", (UmiDesktopRect){0, 0, 960, 520}, UMI_DESKTOP_DOCK_CANVAS,
        true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "traces", "Logs and Traces", "umicom.operations.logs", "org.umicom.operations",
        "run-green", (UmiDesktopRect){960, 0, 960, 520}, UMI_DESKTOP_DOCK_CANVAS,
        true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "profiler", "Profiler", "umicom.operations.metrics", "org.umicom.operations",
        "run-green", (UmiDesktopRect){0, 520, 960, 520},
        UMI_DESKTOP_DOCK_CANVAS, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "health", "Health and Resilience", "umicom.operations.health",
        "org.umicom.operations", "run-green", (UmiDesktopRect){960, 520, 960, 520},
        UMI_DESKTOP_DOCK_CANVAS, true, 4);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_trading(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "trading", "Trading", "Trading",
        "TWS-inspired watchlist, analytics, order, portfolio and activity workspace");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "trading-watchlists", "Watchlists", "umicom.trading.watchlist",
        "org.umicom.trader", "trading-teal", (UmiDesktopRect){0, 0, 346, 749},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "trading-chart", "Chart and Analytics", "umicom.trading.chart",
        "org.umicom.trader", "trading-teal", (UmiDesktopRect){346, 0, 998, 749},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "trading-order-entry", "Order Entry", "umicom.trading.order-ticket",
        "org.umicom.trader", "trading-teal", (UmiDesktopRect){1344, 0, 576, 375},
        UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "trading-portfolio", "Portfolio and Risk", "umicom.trading.portfolio",
        "org.umicom.trader", "trading-teal", (UmiDesktopRect){1344, 375, 576, 374},
        UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "trading-activity", "Orders, Executions and Messages", "umicom.trading.orders",
        "org.umicom.trader", "trading-teal", (UmiDesktopRect){0, 749, 1920, 291},
        UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_compare(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "compare", "Compare", "Comparison",
        "Beyond Compare-inspired side-by-side file comparison workspace");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "compare-explorer", "Folders", "umicom.development.explorer",
        "org.umicom.studio", "compare-orange", (UmiDesktopRect){0, 0, 346, 1040},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "compare-original", "Original", "umicom.development.editor",
        "org.umicom.studio", "compare-orange", (UmiDesktopRect){346, 0, 787, 832},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "compare-modified", "Modified", "umicom.development.editor",
        "org.umicom.studio", "compare-orange", (UmiDesktopRect){1133, 0, 787, 832},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "compare-results", "Comparison Results", "umicom.development.terminal",
        "org.umicom.studio", "compare-orange", (UmiDesktopRect){346, 832, 1574, 208},
        UMI_DESKTOP_DOCK_BOTTOM, true, 4);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_treasury(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "treasury", "Treasury", "Treasury",
        "Calypso-inspired front-to-back trade, risk, workflow and settlement workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "tms-blotter", "Trade Blotter", "umicom.treasury.trade-blotter", "org.umicom.tms",
        "treasury-blue", (UmiDesktopRect){0, 0, 480, 728}, UMI_DESKTOP_DOCK_LEFT,
        true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "tms-capture", "Trade Capture", "umicom.treasury.trade-capture", "org.umicom.tms",
        "treasury-blue", (UmiDesktopRect){480, 0, 864, 728},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "tms-risk", "Pricing and Risk", "umicom.treasury.risk", "org.umicom.tms",
        "treasury-blue", (UmiDesktopRect){1344, 0, 576, 520}, UMI_DESKTOP_DOCK_RIGHT,
        true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "tms-workflow", "Workflow Inbox", "umicom.treasury.workflow", "org.umicom.tms",
        "treasury-blue", (UmiDesktopRect){1344, 520, 576, 208}, UMI_DESKTOP_DOCK_RIGHT,
        true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "tms-settlement", "Settlement and Accounting", "umicom.treasury.settlement",
        "org.umicom.tms", "treasury-blue", (UmiDesktopRect){0, 728, 1920, 312},
        UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_media(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "media", "Media", "Media",
        "Professional non-linear video editing, monitoring, mixing and delivery workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-assets", "Asset Browser", "umicom.media.asset-browser",
        "org.umicom.media-studio", "media-violet", (UmiDesktopRect){0, 0, 384, 624},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-source", "Source Monitor", "umicom.media.source-monitor",
        "org.umicom.media-studio", "media-violet", (UmiDesktopRect){384, 0, 576, 624},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-program", "Program Monitor", "umicom.media.program-monitor",
        "org.umicom.media-studio", "media-violet", (UmiDesktopRect){960, 0, 576, 624},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-effects", "Effects and Delivery", "umicom.media.effects",
        "org.umicom.media-studio", "media-violet", (UmiDesktopRect){1536, 0, 384, 624},
        UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-timeline", "Timeline", "umicom.media.timeline", "org.umicom.media-studio",
        "media-violet", (UmiDesktopRect){0, 624, 1536, 416}, UMI_DESKTOP_DOCK_BOTTOM,
        false, 5);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "media-mixer", "Audio Mixer", "umicom.media.audio-mixer", "org.umicom.media-studio",
        "media-violet", (UmiDesktopRect){1536, 624, 384, 416}, UMI_DESKTOP_DOCK_RIGHT,
        true, 6);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_music(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "music", "Music", "Music",
        "Suno-inspired generation with professional arrangement, piano roll and stem mixing.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "music-prompt", "Song Prompt", "umicom.music.prompt", "org.umicom.music-studio",
        "music-pink", (UmiDesktopRect){0, 0, 480, 624}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "music-generations", "Song Generations", "umicom.music.generations",
        "org.umicom.music-studio", "music-pink", (UmiDesktopRect){480, 0, 960, 312},
        UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "music-arranger", "Arranger", "umicom.music.arranger", "org.umicom.music-studio",
        "music-pink", (UmiDesktopRect){480, 312, 960, 312},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "music-stems", "Stem Mixer", "umicom.music.stems", "org.umicom.music-studio",
        "music-pink", (UmiDesktopRect){1440, 0, 480, 624}, UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "music-piano-roll", "Piano Roll", "umicom.music.piano-roll",
        "org.umicom.music-studio", "music-pink", (UmiDesktopRect){0, 624, 1920, 416},
        UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_creator(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "creator", "AI Creator", "AI Media",
        "Image and video generation workspace for prompts, storyboards, canvas and variants.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "creator-prompt", "Generation Prompt", "umicom.creator.prompt", "org.umicom.creator",
        "creator-cyan", (UmiDesktopRect){0, 0, 480, 1040}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "creator-canvas", "Generation Canvas", "umicom.creator.canvas", "org.umicom.creator",
        "creator-cyan", (UmiDesktopRect){480, 0, 960, 728},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "creator-variants", "Variant Gallery", "umicom.creator.variants", "org.umicom.creator",
        "creator-cyan", (UmiDesktopRect){1440, 0, 480, 728}, UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "creator-storyboard", "Storyboard", "umicom.creator.storyboard", "org.umicom.creator",
        "creator-cyan", (UmiDesktopRect){480, 728, 1440, 312},
        UMI_DESKTOP_DOCK_BOTTOM, true, 4);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_knowledge(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "knowledge", "RAG", "AI and Knowledge",
        "Governed ingestion, retrieval, citation and AI conversation workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "rag-collections", "Knowledge Collections", "umicom.rag.collections", "org.umicom.rag",
        "rag-green", (UmiDesktopRect){0, 0, 384, 1040}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "rag-sources", "Knowledge Sources", "umicom.rag.sources", "org.umicom.rag",
        "rag-green", (UmiDesktopRect){384, 0, 576, 520}, UMI_DESKTOP_DOCK_DOCUMENT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "rag-pipeline", "Ingestion Pipeline", "umicom.rag.pipeline", "org.umicom.rag",
        "rag-green", (UmiDesktopRect){384, 520, 576, 520}, UMI_DESKTOP_DOCK_DOCUMENT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "rag-chat", "AI Chat", "umicom.ai.chat", "org.umicom.llm", "rag-green",
        (UmiDesktopRect){960, 0, 480, 1040}, UMI_DESKTOP_DOCK_DOCUMENT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "rag-reader", "Source Reader", "umicom.rag.source-reader", "org.umicom.rag",
        "rag-green", (UmiDesktopRect){1440, 0, 480, 1040}, UMI_DESKTOP_DOCK_RIGHT, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_model_lab(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(
        &layout, "model-lab", "Model Lab", "AI and Models",
        "Local and remote model runtime, chat, tooling and evaluation workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "llm-runtime", "Runtime Manager", "umicom.llm.runtime", "org.umicom.llm",
        "llm-purple", (UmiDesktopRect){0, 0, 480, 520}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "llm-models", "Model Catalogue", "umicom.ai.models", "org.umicom.llm",
        "llm-purple", (UmiDesktopRect){0, 520, 480, 520}, UMI_DESKTOP_DOCK_LEFT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "llm-chat", "Model Chat", "umicom.llm.chat", "org.umicom.llm", "llm-purple",
        (UmiDesktopRect){480, 0, 864, 1040}, UMI_DESKTOP_DOCK_DOCUMENT, false, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "llm-evaluation", "AI Evaluation", "umicom.ai.evaluation", "org.umicom.llm",
        "llm-purple", (UmiDesktopRect){1344, 0, 576, 520}, UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "llm-tools", "AI Tools", "umicom.ai.tools", "org.umicom.llm", "llm-purple",
        (UmiDesktopRect){1344, 520, 576, 520}, UMI_DESKTOP_DOCK_RIGHT, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_games(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(&layout, "games", "Games", "Games",
        "Scene, viewport, assets, behaviour and performance workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "games-scene", "Scene Editor", "umicom.games.scene", "org.umicom.games",
        "games-green", (UmiDesktopRect){0, 0, 384, 728}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "games-viewport", "Game Viewport", "umicom.games.viewport", "org.umicom.games",
        "games-green", (UmiDesktopRect){384, 0, 1152, 728},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "games-assets", "Game Assets", "umicom.games.assets", "org.umicom.games",
        "games-green", (UmiDesktopRect){1536, 0, 384, 520}, UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "games-behaviour", "Behaviour Graph", "umicom.games.behaviour", "org.umicom.games",
        "games-green", (UmiDesktopRect){1536, 520, 384, 208}, UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "games-profiler", "Game Profiler", "umicom.games.profiler", "org.umicom.games",
        "games-green", (UmiDesktopRect){0, 728, 1920, 312}, UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_cad(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(&layout, "cad", "CAD", "Design",
        "Parametric modelling, constraints, properties, materials and rendering workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "cad-tree", "Model Tree", "umicom.cad.model-tree", "org.umicom.cad",
        "cad-blue", (UmiDesktopRect){0, 0, 384, 1040}, UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "cad-viewport", "CAD Viewport", "umicom.cad.viewport", "org.umicom.cad",
        "cad-blue", (UmiDesktopRect){384, 0, 1152, 832},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "cad-properties", "CAD Properties", "umicom.cad.properties", "org.umicom.cad",
        "cad-blue", (UmiDesktopRect){1536, 0, 384, 520}, UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "cad-materials", "Materials", "umicom.design.materials", "org.umicom.cad",
        "cad-blue", (UmiDesktopRect){1536, 520, 384, 312}, UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "cad-constraints", "Constraints", "umicom.cad.constraints", "org.umicom.cad",
        "cad-blue", (UmiDesktopRect){384, 832, 1536, 208}, UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_kitchen(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(&layout, "kitchen", "Kitchen", "Design",
        "Kitchen catalogue, parametric planner, materials, costing and render workspace.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "kitchen-catalogue", "Kitchen Catalogue", "umicom.kitchen.catalogue",
        "org.umicom.kitchen-designer", "kitchen-amber", (UmiDesktopRect){0, 0, 384, 1040},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "kitchen-planner", "Kitchen Planner", "umicom.kitchen.planner",
        "org.umicom.kitchen-designer", "kitchen-amber", (UmiDesktopRect){384, 0, 1152, 832},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "kitchen-materials", "Materials", "umicom.design.materials",
        "org.umicom.kitchen-designer", "kitchen-amber", (UmiDesktopRect){1536, 0, 384, 416},
        UMI_DESKTOP_DOCK_RIGHT, true, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "kitchen-render", "Render Studio", "umicom.design.render",
        "org.umicom.kitchen-designer", "kitchen-amber", (UmiDesktopRect){1536, 416, 384, 416},
        UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "kitchen-costing", "Kitchen Costing", "umicom.kitchen.costing",
        "org.umicom.kitchen-designer", "kitchen-amber", (UmiDesktopRect){384, 832, 1536, 208},
        UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

static UmiStatus seed_author(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    UmiDesktopLayout layout;
    UmiStatus status = initialise_layout(&layout, "author", "Author", "Authoring",
        "Research-to-publication workspace for books, courses, sites and media programmes.");
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "author-research", "Research Board", "umicom.author.research",
        "org.umicom.author", "author-gold", (UmiDesktopRect){0, 0, 384, 1040},
        UMI_DESKTOP_DOCK_LEFT, true, 1);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "author-outline", "Structure and Outline", "umicom.author.outline",
        "org.umicom.author", "author-gold", (UmiDesktopRect){384, 0, 384, 1040},
        UMI_DESKTOP_DOCK_LEFT, true, 2);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "author-editor", "Author Editor", "umicom.author.editor", "org.umicom.author",
        "author-gold", (UmiDesktopRect){768, 0, 768, 832},
        UMI_DESKTOP_DOCK_DOCUMENT, false, 3);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "author-chat", "AI Chat", "umicom.ai.chat", "org.umicom.llm", "author-gold",
        (UmiDesktopRect){1536, 0, 384, 832}, UMI_DESKTOP_DOCK_RIGHT, true, 4);
    if (status == UMI_STATUS_OK) status = add_window(&layout, monitor_id,
        "author-publish", "Publishing Centre", "umicom.author.publish", "org.umicom.author",
        "author-gold", (UmiDesktopRect){768, 832, 1152, 208},
        UMI_DESKTOP_DOCK_BOTTOM, true, 5);
    return status == UMI_STATUS_OK ? add_seeded_layout(catalogue, &layout) : status;
}

void umi_desktop_layout_catalogue_init(UmiDesktopLayoutCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
}

UmiStatus umi_desktop_layout_catalogue_add(
    UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopLayout *layout)
{
    if (catalogue == NULL || layout == NULL || layout->layout_id[0] == '\0' ||
        layout->name[0] == '\0' || layout->category[0] == '\0' ||
        layout->window_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_layout_index(catalogue, layout->layout_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_DESKTOP_MAX_LAYOUTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->layouts[catalogue->count++] = *layout;
    if (catalogue->count == 1U)
        (void)copy_text(catalogue->active_layout_id,
                        sizeof(catalogue->active_layout_id), layout->layout_id);
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_catalogue_remove(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id)
{
    size_t index;
    if (catalogue == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_layout_index(catalogue, layout_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (catalogue->layouts[index].built_in)
        return UMI_STATUS_PERMISSION_DENIED;
    if (index + 1U < catalogue->count) {
        (void)memmove(&catalogue->layouts[index], &catalogue->layouts[index + 1U],
                      (catalogue->count - index - 1U) *
                          sizeof(catalogue->layouts[0]));
    }
    catalogue->count -= 1U;
    (void)memset(&catalogue->layouts[catalogue->count], 0,
                 sizeof(catalogue->layouts[0]));
    if (strcmp(catalogue->active_layout_id, layout_id) == 0) {
        if (catalogue->count > 0U)
            (void)copy_text(catalogue->active_layout_id,
                            sizeof(catalogue->active_layout_id),
                            catalogue->layouts[0].layout_id);
        else
            catalogue->active_layout_id[0] = '\0';
    }
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_catalogue_activate(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id)
{
    UmiStatus status;
    if (catalogue == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (find_layout_index(catalogue, layout_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    status = copy_text(catalogue->active_layout_id,
                       sizeof(catalogue->active_layout_id), layout_id);
    if (status == UMI_STATUS_OK) catalogue->revision += 1U;
    return status;
}

UmiStatus umi_desktop_layout_catalogue_clone(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *source_layout_id,
    const char *layout_id,
    const char *name)
{
    const UmiDesktopLayout *source;
    UmiDesktopLayout clone;
    UmiStatus status;
    if (catalogue == NULL || source_layout_id == NULL || layout_id == NULL ||
        name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_desktop_layout_catalogue_find(catalogue, source_layout_id);
    if (source == NULL) return UMI_STATUS_NOT_FOUND;
    clone = *source;
    status = copy_text(clone.layout_id, sizeof(clone.layout_id), layout_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(clone.name, sizeof(clone.name), name);
    if (status != UMI_STATUS_OK) return status;
    clone.built_in = false;
    clone.locked = false;
    clone.revision += 1U;
    return umi_desktop_layout_catalogue_add(catalogue, &clone);
}

UmiStatus umi_desktop_layout_catalogue_seed_professional(
    UmiDesktopLayoutCatalogue *catalogue,
    const char *monitor_id)
{
    typedef UmiStatus (*SeedFn)(UmiDesktopLayoutCatalogue *, const char *);
    static const SeedFn seed_functions[] = {
        seed_develop, seed_focus, seed_debug, seed_operations, seed_trading,
        seed_compare, seed_treasury, seed_media, seed_music, seed_creator,
        seed_knowledge, seed_model_lab, seed_games, seed_cad, seed_kitchen,
        seed_author
    };
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (catalogue == NULL || monitor_id == NULL || monitor_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count != 0U) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < sizeof(seed_functions) / sizeof(seed_functions[0]);
         ++index) {
        status = seed_functions[index](catalogue, monitor_id);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_desktop_layout_catalogue_activate(catalogue, "develop");
}

const UmiDesktopLayout *umi_desktop_layout_catalogue_find(
    const UmiDesktopLayoutCatalogue *catalogue,
    const char *layout_id)
{
    const size_t index = find_layout_index(catalogue, layout_id);
    return index != SIZE_MAX ? &catalogue->layouts[index] : NULL;
}

UmiDesktopLayout *umi_desktop_layout_catalogue_active_mutable(
    UmiDesktopLayoutCatalogue *catalogue)
{
    const size_t index = catalogue != NULL
        ? find_layout_index(catalogue, catalogue->active_layout_id) : SIZE_MAX;
    return index != SIZE_MAX ? &catalogue->layouts[index] : NULL;
}

const UmiDesktopLayout *umi_desktop_layout_catalogue_active(
    const UmiDesktopLayoutCatalogue *catalogue)
{
    const size_t index = catalogue != NULL
        ? find_layout_index(catalogue, catalogue->active_layout_id) : SIZE_MAX;
    return index != SIZE_MAX ? &catalogue->layouts[index] : NULL;
}
