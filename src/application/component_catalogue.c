/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue.c
 *
 * PURPOSE:
 *   Define reusable professional surfaces for the complete Umicom portfolio.
 *   The catalogue expresses product vocabulary without embedding product code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component_catalogue.h"

#include <string.h>

#define COMP(id, domain, label, capability, detail, component_role, state, multi, context) \
    {(id), (domain), (label), (capability), (detail), (component_role), \
     (state), (multi), (context)}

static const UmiApplicationComponentDefinition COMPONENTS[] = {
    /* Universal shell, layouts and linked-window services. */
    COMP("umicom.shell.activity-bar", "shell", "Activity Bar", "umicom.ui",
         "Switch primary work areas and application-contributed containers.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.command-palette", "shell", "Command Palette", "umicom.ui",
         "Search and invoke permission-filtered Framework commands.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.dock-canvas", "shell", "Dock Canvas", "umicom.ui",
         "Dock, split, tab, float and restore reusable component instances.",
         UMI_APPLICATION_COMPONENT_SERVICE_SURFACE, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shell.layout-tabs", "shell", "Layout Tabs", "umicom.ui",
         "TWS-inspired bottom tabs for named user and task layouts.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.workspace-profiles", "shell", "Workspace Profiles", "umicom.workspace",
         "Save, restore, import and export application-agnostic layouts.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.multi-monitor", "shell", "Monitor Manager", "umicom.ui",
         "Persist windows, bounds, scale and monitor affinity.",
         UMI_APPLICATION_COMPONENT_SERVICE_SURFACE, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.context-links", "shell", "Context Link Groups", "umicom.application.context",
         "Colour-linked windows share symbols, documents, assets or subjects through Framework.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 0, 1),
    COMP("umicom.shell.notifications", "shell", "Notification Centre", "umicom.ui",
         "Unified non-blocking information, warning and approval notifications.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 0, 1),

    /* Developer and IDE surfaces. */
    COMP("umicom.development.explorer", "development", "Project Explorer", "umicom.workspace",
         "Projects, files, symbols and virtual resources.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.editor", "development", "Code Editor", "umicom.documents",
         "Language-aware editing, navigation, diagnostics and refactoring.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.development.build", "development", "Build Dashboard", "umicom.build",
         "Configure, build, graph, history, diagnostics and artifacts.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.debug", "development", "Debug Workspace", "umicom.debug",
         "Sessions, call stacks, variables, watches, breakpoints and console.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.testing", "development", "Test Explorer", "umicom.testing",
         "Discovery, hierarchy, runs, failures, coverage and artifacts.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.source-control", "development", "Source Control", "umicom.vcs",
         "Changes, commits, branches, history, remotes and conflicts.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.terminal", "development", "Terminal", "umicom.process",
         "Supervised terminal sessions and task output.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.development.designer", "development", "Visual Designer", "umicom.designer",
         "Declarative component palette, canvas and property editing.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),

    /* Direct capability projections used across product workbenches. */
    COMP("umicom.shared.editor", "development", "Document Editor", "umicom.editor",
         "Shared editing surface for source, text and structured documents.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.diagnostics", "quality", "Diagnostics Centre", "umicom.diagnostics",
         "Problems, diagnostics, logs, fixes and correlated evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.terminal", "development", "Terminal Sessions", "umicom.terminal",
         "Supervised command shells, process sessions and task output.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.application-federation", "application", "Application Federation", "umicom.application.federation",
         "Cross-application discovery, selection, context and lifecycle projection.",
         UMI_APPLICATION_COMPONENT_SERVICE_SURFACE, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.quality", "quality", "Quality Centre", "umicom.quality",
         "Architecture, conformance, duplicate and product-quality evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.audit", "governance", "Audit Trail", "umicom.audit",
         "Immutable user, service, decision and business-operation evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.banking", "finance", "Banking Workbench", "umicom.banking",
         "Customers, accounts, deposits, lending and banking operations.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.payments", "finance", "Payments Hub", "umicom.payments",
         "Payment initiation, routing, status, exceptions and investigation.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.accounting", "finance", "Accounting Workbench", "umicom.accounting",
         "Journals, ledgers, postings, periods and financial statements.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.digital-assets", "finance", "Digital Assets", "umicom.digital-asset",
         "Token, wallet, custody, balance and transaction operations.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.security", "security", "Security Centre", "umicom.security",
         "Identity, access, policy, secrets and security posture.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.analytics", "analytics", "Analytics Workbench", "umicom.analytics",
         "Metrics, dimensions, scenarios and explainable analytical results.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.risk", "finance", "Risk Centre", "umicom.risk",
         "Limits, exposures, sensitivities, scenarios and approvals.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.settlement", "finance", "Settlement Monitor", "umicom.settlement",
         "Instructions, matching, settlement status and exception queues.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.shared.collateral", "finance", "Collateral Manager", "umicom.collateral",
         "Agreements, inventory, calls, substitutions and optimisation.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.shared.operations", "operations", "Operations Centre", "umicom.operations",
         "Operational queues, controls, incidents and recovery actions.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.market-data", "finance", "Market Data Monitor", "umicom.market-data",
         "Quotes, curves, surfaces, fixings, lineage and quality controls.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.positions", "finance", "Position Centre", "umicom.position",
         "Positions, lots, cash, aggregation and lifecycle movements.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.valuation", "finance", "Valuation Centre", "umicom.valuation",
         "Pricing inputs, measures, sensitivities and valuation evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.reconciliation", "finance", "Reconciliation Workbench", "umicom.reconciliation",
         "Matching, breaks, investigation, adjustment and sign-off.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.ai-research", "ai", "Research Workspace", "umicom.ai.research",
         "Research plans, sources, claims, evidence and governed synthesis.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.shared.commodity", "finance", "Commodity Workbench", "umicom.commodity",
         "Physical products, locations, quality, logistics and contracts.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.exchange", "finance", "Exchange Workbench", "umicom.exchange",
         "Markets, RFQs, auctions, orders, trades and clearing state.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.shared.surveillance", "governance", "Surveillance Centre", "umicom.surveillance",
         "Alerts, patterns, investigations, cases and regulatory evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.shared.payroll", "people", "Payroll Workbench", "umicom.payroll",
         "Pay runs, earnings, deductions, tax, pensions and payments.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.shared.hr", "people", "People Centre", "umicom.hr",
         "Employees, organisation, leave, performance and compliance.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.shared.tasks", "workflow", "Task Centre", "umicom.tasks",
         "Assignments, queues, priorities, deadlines and completion evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.runtime", "runtime", "Runtime Manager", "umicom.runtime",
         "Runtime discovery, lifecycle, health, resources and diagnostics.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.web", "web", "Web Workbench", "umicom.web",
         "Pages, routes, requests, responsive preview and browser diagnostics.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.messaging", "integration", "Messaging Centre", "umicom.messaging",
         "Queues, topics, subscriptions, deliveries and dead letters.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.resilience", "operations", "Resilience Centre", "umicom.resilience",
         "Readiness, failover, recovery, continuity and resilience evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.product", "product", "Product Catalogue", "umicom.product",
         "Product definitions, packages, compatibility and lifecycle state.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.shared.abi", "development", "ABI Inspector", "umicom.abi",
         "Binary contracts, symbols, compatibility and conformance evidence.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),

    /* Trading surfaces inspired by professional multi-window terminals. */
    COMP("umicom.trading.watchlist", "trading", "Watchlist", "umicom.trading",
         "Linked instruments, market state, alerts and quick actions.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.chart", "trading", "Advanced Chart", "umicom.chart",
         "Multi-series charting, indicators, drawings and strategy overlays.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.market-depth", "trading", "Market Depth", "umicom.trading",
         "Price ladder, order book imbalance and liquidity inspection.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.order-ticket", "trading", "Order Ticket", "umicom.trading",
         "Risk-gated simulation, paper and live order preparation.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.orders", "trading", "Orders", "umicom.trading",
         "Order lifecycle, amendments, cancellations and audit state.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.executions", "trading", "Executions", "umicom.trading",
         "Fills, allocations, fees and execution-quality evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.portfolio", "trading", "Portfolio", "umicom.trading",
         "Positions, cash, realised and unrealised profit and loss.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.risk", "trading", "Trading Risk", "umicom.trading",
         "Limits, exposure, scenarios, kill switch and readiness gates.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.trading.strategy", "trading", "Strategy Lab", "umicom.trading",
         "Code, backtest, replay, compare and deploy governed strategies.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),

    /* Treasury-management surfaces. */
    COMP("umicom.treasury.trade-capture", "treasury", "Trade Capture", "umicom.tms",
         "Product-aware capture with validation, enrichment and lifecycle events.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.trade-blotter", "treasury", "Trade Blotter", "umicom.tms",
         "Search, filter and act on treasury transactions.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.market-data", "treasury", "Market Data", "umicom.tms",
         "Quotes, curves, surfaces, fixings and data-quality controls.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.pricing", "treasury", "Pricing and Analytics", "umicom.tms",
         "Valuation measures, sensitivities and explainable results.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.risk", "treasury", "Treasury Risk", "umicom.tms",
         "Market, credit, liquidity and counterparty exposure.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.workflow", "treasury", "Workflow Inbox", "umicom.workflow",
         "Tasks, approvals, exceptions and four-eyes controls.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.settlement", "treasury", "Settlement", "umicom.tms",
         "Cashflows, instructions, matching, settlement and exceptions.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.treasury.accounting", "treasury", "Accounting", "umicom.tms",
         "Events, postings, reconciliation and ledger interfaces.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),

    /* Media, music and generative creation. */
    COMP("umicom.media.asset-browser", "media", "Asset Browser", "umicom.media",
         "Indexed video, audio, image, proxy and metadata assets.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.source-monitor", "media", "Source Monitor", "umicom.media",
         "Inspect, mark and trim source media.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.program-monitor", "media", "Program Monitor", "umicom.media",
         "Preview the active edit, composite or delivery output.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.timeline", "media", "Timeline", "umicom.media",
         "Track-based non-linear editing with clips, transitions and keyframes.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.effects", "media", "Effects", "umicom.media",
         "Search and apply Framework-owned effects and transitions.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.color", "media", "Colour", "umicom.media",
         "Scopes, grading, transforms and shot matching.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.audio-mixer", "media", "Audio Mixer", "umicom.media",
         "Track, bus, effect, automation and loudness control.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.media.delivery", "media", "Delivery Queue", "umicom.delivery",
         "Presets, render jobs, evidence and publishing destinations.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.music.prompt", "music", "Song Prompt", "umicom.music",
         "Style, lyrics, structure, voice and reference guidance.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.music.generations", "music", "Song Generations", "umicom.music",
         "Versioned generated takes with lineage and comparison.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.music.arranger", "music", "Arranger", "umicom.music",
         "Sections, stems, tempo, key, chord and structure editing.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.music.piano-roll", "music", "Piano Roll", "umicom.music",
         "Note, expression, timing and quantisation editing.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.music.stems", "music", "Stem Mixer", "umicom.music",
         "Separate, arrange, process and export generated or recorded stems.",
         UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.creator.prompt", "creator", "Generation Prompt", "umicom.ai.media-generation",
         "Structured text, image, video and motion conditioning.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.creator.storyboard", "creator", "Storyboard", "umicom.ai.media-generation",
         "Shots, continuity, camera direction and generation state.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.creator.canvas", "creator", "Generation Canvas", "umicom.ai.media-generation",
         "Masking, inpainting, outpainting, layers and region controls.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.creator.variants", "creator", "Variant Gallery", "umicom.ai.media-generation",
         "Compare seeds, models, prompts, quality and provenance.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),

    /* AI, RAG and model operations. */
    COMP("umicom.ai.chat", "ai", "AI Chat", "umicom.ai",
         "Provider-neutral conversations, tools, citations and approvals.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.ai.context", "ai", "AI Context", "umicom.ai",
         "Inspect the governed context assembled for a request.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.ai.models", "ai", "Model Catalogue", "umicom.ai.provider",
         "Discover local and remote models and policy constraints.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.ai.tools", "ai", "AI Tools", "umicom.ai.tools",
         "Inspect permissions, inputs, approvals and outcomes.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.ai.evaluation", "ai", "AI Evaluation", "umicom.ai",
         "Datasets, tests, quality, safety and regression evidence.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.rag.collections", "rag", "Knowledge Collections", "umicom.knowledge",
         "Governed collections, access and lifecycle.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.rag.sources", "rag", "Knowledge Sources", "umicom.ai.retrieval",
         "Files, sites, connectors, parsing and ingestion state.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.rag.pipeline", "rag", "Ingestion Pipeline", "umicom.ai.retrieval",
         "Chunking, metadata, embeddings, indexing and failures.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.rag.search", "rag", "Semantic Search", "umicom.ai.retrieval",
         "Hybrid retrieval, ranking, filters and citations.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.rag.source-reader", "rag", "Source Reader", "umicom.knowledge",
         "Inspect cited ranges alongside generated answers.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.llm.chat", "llm", "Model Chat", "umicom.llm",
         "Token-streaming local and remote model interaction.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.llm.runtime", "llm", "Runtime Manager", "umicom.llm",
         "Load, unload, schedule and monitor inference runtimes.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.llm.model-lab", "llm", "Model Lab", "umicom.llm",
         "Datasets, tokenisation, tuning, quantisation and evaluation.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),

    /* Games, CAD and specialist design. */
    COMP("umicom.games.scene", "games", "Scene Editor", "umicom.games",
         "Entity hierarchy, transforms, components and world composition.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.games.viewport", "games", "Game Viewport", "umicom.games",
         "Interactive rendering, selection, gizmos and play mode.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.games.assets", "games", "Game Assets", "umicom.games",
         "Models, materials, animation, audio and import pipelines.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.games.behaviour", "games", "Behaviour Graph", "umicom.games",
         "Visual state, event, AI and gameplay logic composition.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.games.profiler", "games", "Game Profiler", "umicom.games",
         "Frame, render, memory, physics and script performance.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.cad.viewport", "cad", "CAD Viewport", "umicom.cad",
         "2D and 3D geometry, selection, snapping and navigation.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.cad.model-tree", "cad", "Model Tree", "umicom.cad",
         "Features, bodies, components, assemblies and references.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.cad.constraints", "cad", "Constraints", "umicom.cad",
         "Dimensional and geometric constraints with solver evidence.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.cad.properties", "cad", "CAD Properties", "umicom.cad",
         "Parametric feature, material and manufacturing properties.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.design.materials", "design", "Materials", "umicom.cad",
         "Reusable material, texture, finish and sustainability catalogue.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.design.render", "design", "Render Studio", "umicom.cad",
         "Lighting, camera, environment and render jobs.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.kitchen.catalogue", "kitchen", "Kitchen Catalogue", "umicom.kitchen",
         "Cabinets, doors, worktops, appliances and accessories.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.kitchen.planner", "kitchen", "Kitchen Planner", "umicom.kitchen",
         "Room capture, parametric placement and rule validation.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.kitchen.costing", "kitchen", "Kitchen Costing", "umicom.kitchen",
         "Bill of materials, prices, alternatives and quotation.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),

    /* Authoring, web, data, integration, operations and OS. */
    COMP("umicom.author.research", "author", "Research Board", "umicom.ai.authorengine",
         "Sources, notes, citations, claims and research plans.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.author.outline", "author", "Structure and Outline", "umicom.ai.authorengine",
         "Hierarchical books, courses, sites and media programmes.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.author.editor", "author", "Author Editor", "umicom.ai.authorengine",
         "Long-form structured writing with citations and reusable media.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.author.publish", "author", "Publishing Centre", "umicom.delivery",
         "Generate, review and deliver books, sites, courses and video packages.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.web.page-canvas", "web", "Page Canvas", "umicom.frontend",
         "Responsive semantic layout and component composition.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_FOUNDATION, 1, 1),
    COMP("umicom.web.dom-inspector", "web", "DOM Inspector", "umicom.frontend",
         "Structure, accessibility, style and event inspection.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.web.preview", "web", "Web Preview", "umicom.frontend",
         "Live responsive preview with logs and reload state.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.database.explorer", "database", "Database Explorer", "umicom.data",
         "Connections, schemas, tables, views and routines.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.database.query", "database", "Query Editor", "umicom.data",
         "Safe queries, parameters, plans, results and history.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.database.migrations", "database", "Migration Centre", "umicom.data.migrations",
         "Plan, validate, apply and evidence schema changes.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.integration.designer", "integration", "Integration Designer", "umicom.integration",
         "Connectors, mappings, routes, workflows and error handling.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.integration.monitor", "integration", "Integration Monitor", "umicom.integration",
         "Throughput, failures, replay, dead letters and lineage.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.operations.health", "operations", "Health Dashboard", "umicom.observability.readiness",
         "Readiness, dependencies, incidents and recovery state.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.operations.logs", "operations", "Log Explorer", "umicom.tracing",
         "Correlated diagnostics, events, traces and redaction.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.operations.metrics", "operations", "Metrics", "umicom.metrics",
         "Time-series counters, gauges, alerts and service objectives.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.security.identities", "security", "Identity Centre", "umicom.identity",
         "Users, services, agents, roles and sessions.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.security.policy", "security", "Policy Centre", "umicom.security.policy",
         "Capabilities, decisions, approvals and audit evidence.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
    COMP("umicom.os.launcher", "os", "Application Launcher", "umicom.os",
         "Discover and start installed Umicom applications through Framework.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_PLANNED, 0, 1),
    COMP("umicom.os.control-centre", "os", "Control Centre", "umicom.os",
         "Devices, network, displays, storage, identity and updates.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 0, 1),
    COMP("umicom.os.resource-monitor", "os", "Resource Monitor", "umicom.os",
         "Processes, CPU, memory, storage, network and accelerators.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.education.course", "education", "Course Designer", "umicom.education",
         "Outcomes, modules, lessons, resources and delivery sequence.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1),
    COMP("umicom.education.assessment", "education", "Assessment Designer", "umicom.education",
         "Questions, rubrics, simulations, feedback and evidence.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1)
};

size_t umi_application_component_catalogue_count(void)
{
    return sizeof(COMPONENTS) / sizeof(COMPONENTS[0]);
}

const UmiApplicationComponentDefinition *
umi_application_component_catalogue_at(size_t index)
{
    return index < umi_application_component_catalogue_count()
        ? &COMPONENTS[index] : NULL;
}

const UmiApplicationComponentDefinition *
umi_application_component_catalogue_find(const char *component_id)
{
    size_t index;
    if (component_id == NULL) return NULL;
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        if (strcmp(COMPONENTS[index].component_id, component_id) == 0)
            return &COMPONENTS[index];
    }
    return NULL;
}

size_t umi_application_component_capability_count(const char *capability_id)
{
    size_t index;
    size_t count = 0U;
    if (capability_id == NULL) return 0U;
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        if (strcmp(COMPONENTS[index].capability_id, capability_id) == 0)
            count += 1U;
    }
    return count;
}

const UmiApplicationComponentDefinition *
umi_application_component_capability_at(
    const char *capability_id,
    size_t capability_index)
{
    size_t index;
    size_t match_index = 0U;
    if (capability_id == NULL) return NULL;
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        if (strcmp(COMPONENTS[index].capability_id, capability_id) != 0)
            continue;
        if (match_index == capability_index) return &COMPONENTS[index];
        match_index += 1U;
    }
    return NULL;
}

size_t umi_application_component_domain_count(const char *domain_id)
{
    size_t index;
    size_t count = 0U;
    if (domain_id == NULL) return 0U;
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        if (strcmp(COMPONENTS[index].domain_id, domain_id) == 0) count += 1U;
    }
    return count;
}

int umi_application_component_domain_exists(const char *domain_id)
{
    return umi_application_component_domain_count(domain_id) > 0U;
}

const char *umi_application_component_role_text(
    UmiApplicationComponentRole role)
{
    switch (role) {
        case UMI_APPLICATION_COMPONENT_VIEW: return "view";
        case UMI_APPLICATION_COMPONENT_EDITOR: return "editor";
        case UMI_APPLICATION_COMPONENT_TOOL: return "tool";
        case UMI_APPLICATION_COMPONENT_DASHBOARD: return "dashboard";
        case UMI_APPLICATION_COMPONENT_INSPECTOR: return "inspector";
        case UMI_APPLICATION_COMPONENT_SERVICE_SURFACE: return "service-surface";
        default: return "unknown";
    }
}
