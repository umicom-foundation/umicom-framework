/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/capability_catalogue.c
 *
 * PURPOSE:
 *   Implement the canonical reusable capability inventory used for discovery,
 *   product manifests, generated repositories, architecture reports, and future
 *   compatibility certification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/capability_catalogue.h"

#include <string.h>

static const UmiFrameworkCapabilityDefinition UMI_CAPABILITIES[] = {
    {"umicom.diagnostics", "observability", UMI_CAPABILITY_IMPLEMENTED,
     "Structured diagnostics, sinks, retained records, and correlation."},
    {"umicom.messaging.events", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Typed publication and subscription for completed facts."},
    {"umicom.messaging.commands", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Point-to-point state-changing requests."},
    {"umicom.messaging.queries", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Typed request and response operations without state changes."},
    {"umicom.messaging.journal", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Append-only event journal and replay foundation."},
    {"umicom.data", "persistence", UMI_CAPABILITY_IMPLEMENTED,
     "Single-authority Data Server with memory and SQLite adapters."},
    {"umicom.configuration", "configuration", UMI_CAPABILITY_IMPLEMENTED,
     "Raw key-value configuration loading."},
    {"umicom.settings", "configuration", UMI_CAPABILITY_IMPLEMENTED,
     "Schema-driven typed settings, validation, reset, load, and save."},
    {"umicom.filesystem", "platform", UMI_CAPABILITY_IMPLEMENTED,
     "Portable paths, files, directories, atomic composition, and traversal."},
    {"umicom.process", "platform", UMI_CAPABILITY_IMPLEMENTED,
     "Argument-based child processes with isolated environments and output."},
    {"umicom.clock", "platform", UMI_CAPABILITY_IMPLEMENTED,
     "Wall, monotonic, and deterministic fake clocks."},
    {"umicom.scheduler", "runtime", UMI_CAPABILITY_IMPLEMENTED,
     "Timers and deterministic scheduled work."},
    {"umicom.runtime.modules", "runtime", UMI_CAPABILITY_IMPLEMENTED,
     "Module and Slave Controller descriptors and registry."},
    {"umicom.runtime.capabilities", "runtime", UMI_CAPABILITY_IMPLEMENTED,
     "Capability registry and explicit dependency injection."},
    {"umicom.runtime.application-manifest", "runtime",
     UMI_CAPABILITY_IMPLEMENTED,
     "Application identity, frontends, aliases, and required capabilities."},
    {"umicom.runtime.suite", "runtime", UMI_CAPABILITY_IMPLEMENTED,
     "Independent applications composed into one suite."},
    {"umicom.security.policy", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Capability-based allow and deny policy decisions."},
    {"umicom.security.secrets", "security", UMI_CAPABILITY_FOUNDATION,
     "Provider-neutral secret lookup with environment provider."},
    {"umicom.toolchain.discovery", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Native compiler, build tool, SDK, and library discovery."},
    {"umicom.toolchain.environment", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Isolated child-process environments and local user presets."},
    {"umicom.toolchain.build", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Configure, compile, test, make, run, and stale-cache repair."},
    {"umicom.repository", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Native local Git and optional GitHub repository operations."},
    {"umicom.scaffold.repository", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Complete Framework application repository generation."},
    {"umicom.plugin", "extension", UMI_CAPABILITY_FOUNDATION,
     "Stable C ABI dynamic-library boundary."},
    {"umicom.ui.contracts", "presentation", UMI_CAPABILITY_FOUNDATION,
     "Toolkit-neutral windows, panes, commands, documents, and notifications."},
    {"umicom.filesystem.watch", "platform", UMI_CAPABILITY_PLANNED,
     "Portable recursive file watching and change coalescing."},
    {"umicom.threading", "platform", UMI_CAPABILITY_PLANNED,
     "Threads, synchronisation, cancellation, and worker primitives."},
    {"umicom.ipc", "platform", UMI_CAPABILITY_PLANNED,
     "Deployment-neutral local process communication."},
    {"umicom.network.client", "networking", UMI_CAPABILITY_PLANNED,
     "HTTP, WebSocket, TLS, cancellation, and deadlines."},
    {"umicom.network.server", "networking", UMI_CAPABILITY_PLANNED,
     "HTTP server, routes, WebSocket, and management endpoints."},
    {"umicom.identity", "security", UMI_CAPABILITY_PLANNED,
     "Human, service, module, worker, and agent identities."},
    {"umicom.audit", "security", UMI_CAPABILITY_PLANNED,
     "Durable security and business audit records."},
    {"umicom.metrics", "observability", UMI_CAPABILITY_PLANNED,
     "Counters, gauges, histograms, health, and exporters."},
    {"umicom.tracing", "observability", UMI_CAPABILITY_PLANNED,
     "Correlation, causation, spans, and cross-process trace context."},
    {"umicom.commands", "application", UMI_CAPABILITY_PLANNED,
     "Application command, action, shortcut, and menu registry."},
    {"umicom.documents", "application", UMI_CAPABILITY_FOUNDATION,
     "Toolkit-neutral document identity, text, revisions, and persistence."},
    {"umicom.workspace", "application", UMI_CAPABILITY_FOUNDATION,
     "Workspace identity, projects, state, trust, and restoration."},
    {"umicom.tasks", "application", UMI_CAPABILITY_PLANNED,
     "Cancellable tasks, progress, deadlines, and worker supervision."},
    {"umicom.problems", "application", UMI_CAPABILITY_PLANNED,
     "Unified compiler, runtime, security, and architecture findings."},
    {"umicom.git", "development", UMI_CAPABILITY_PLANNED,
     "Provider-neutral status, diff, history, branch, and merge operations."},
    {"umicom.ai.provider", "ai", UMI_CAPABILITY_PLANNED,
     "Local and remote model provider contract with streaming and cancellation."},
    {"umicom.ai.retrieval", "ai", UMI_CAPABILITY_PLANNED,
     "Embeddings, indexing, retrieval, sources, and local RAG."},
    {"umicom.delivery", "delivery", UMI_CAPABILITY_PLANNED,
     "Build evidence, staging, packaging, signing, release, and rollback."},
    {"umicom.workflow", "enterprise", UMI_CAPABILITY_PLANNED,
     "Versioned workflows, approvals, retries, and compensating actions."},
    {"umicom.integration", "enterprise", UMI_CAPABILITY_PLANNED,
     "Message flows, APIs, transformations, gateways, and durable delivery."},
    {"umicom.gui.gtk4", "presentation", UMI_CAPABILITY_PLANNED,
     "Primary GTK4 adapter for UI-neutral Framework contracts."},
    {"umicom.gui.headless", "presentation", UMI_CAPABILITY_PLANNED,
     "In-memory UI adapter for tests and automation."},
    {"umicom.designer", "development", UMI_CAPABILITY_PLANNED,
     "Semantic application model, palette, properties, undo, and preview."},
    {"umicom.helix", "automation", UMI_CAPABILITY_PLANNED,
     "Governed Designer, Builder, Suggestion, validation, and release agents."}
};

size_t umi_framework_capability_catalogue_count(void)
{
    return sizeof(UMI_CAPABILITIES) / sizeof(UMI_CAPABILITIES[0]);
}

const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_at(
    size_t index)
{
    return index < umi_framework_capability_catalogue_count()
        ? &UMI_CAPABILITIES[index]
        : NULL;
}

const UmiFrameworkCapabilityDefinition *umi_framework_capability_catalogue_find(
    const char *capability_id)
{
    size_t index;
    if (capability_id == NULL) return NULL;
    for (index = 0U; index < umi_framework_capability_catalogue_count(); ++index) {
        if (strcmp(UMI_CAPABILITIES[index].capability_id, capability_id) == 0)
            return &UMI_CAPABILITIES[index];
    }
    return NULL;
}

const char *umi_capability_maturity_text(UmiCapabilityMaturity maturity)
{
    switch (maturity) {
        case UMI_CAPABILITY_IMPLEMENTED: return "implemented";
        case UMI_CAPABILITY_FOUNDATION: return "foundation";
        case UMI_CAPABILITY_PLANNED: return "planned";
        default: return "unknown";
    }
}
