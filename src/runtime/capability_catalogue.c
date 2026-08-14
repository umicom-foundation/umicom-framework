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

#include <stddef.h>

#include <string.h>

static const UmiFrameworkCapabilityDefinition UMI_CAPABILITIES[] = {
    {"umicom.runtime", "runtime", UMI_CAPABILITY_IMPLEMENTED,
     "Aggregate runtime lifecycle, services, capabilities, health, and product composition."},
    {"umicom.messaging", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Aggregate commands, queries, events, durable channels, routing, reliability, and replay."},
    {"umicom.ui", "presentation", UMI_CAPABILITY_IMPLEMENTED,
     "Toolkit-neutral workbench contracts with headless and GTK4 adapters."},
    {"umicom.build", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Build profiles, CMake/Ninja providers, diagnostics, history, and cancellation."},
    {"umicom.protocol", "development", UMI_CAPABILITY_IMPLEMENTED,
     "JSON-RPC, LSP, DAP, framing, and process-client protocol services."},
    {"umicom.vcs", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Provider-neutral source control with the Git CLI reference provider."},
    {"umicom.ai", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Provider registry, prompting, retrieval, tools, evaluation, and policy-aware AI runtime."},
    {"umicom.declarative", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Semantic application templates, validation, planning, and generation."},
    {"umicom.chart", "presentation", UMI_CAPABILITY_IMPLEMENTED,
     "Toolkit-neutral chart models, candles, indicators, viewports, and snapshots."},
    {"umicom.web", "networking", UMI_CAPABILITY_IMPLEMENTED,
     "HTTP, routing, REST, static resources, SSE, WebSocket, sessions, and listeners."},
    {"umicom.security", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Identity, sessions, authorisation, policy, secrets, trust, and redaction."},
    {"umicom.abi", "release", UMI_CAPABILITY_IMPLEMENTED,
     "ABI descriptors, structure layouts, compatibility checks, fingerprints, and baselines."},
    {"umicom.sdk", "release", UMI_CAPABILITY_IMPLEMENTED,
     "Installed SDK discovery, package probing, compatibility, and consumer conformance."},
    {"umicom.product", "application", UMI_CAPABILITY_IMPLEMENTED,
     "Product profiles, reference products, suites, manifests, and compatibility validation."},
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
    {"umicom.plugin", "extension", UMI_CAPABILITY_IMPLEMENTED,
     "Manifests, permissions, discovery, contributions, loading, and lifecycle."},
    {"umicom.ui.contracts", "presentation", UMI_CAPABILITY_FOUNDATION,
     "Toolkit-neutral windows, panes, commands, documents, and notifications."},
    {"umicom.filesystem.watch", "platform", UMI_CAPABILITY_IMPLEMENTED,
     "Portable recursive polling watcher, deterministic fake backend, and indexed updates."},
    {"umicom.threading", "platform", UMI_CAPABILITY_IMPLEMENTED,
     "Threads, synchronisation, cancellation, bounded task queues, and process supervision."},
    {"umicom.ipc", "platform", UMI_CAPABILITY_PLANNED,
     "Deployment-neutral local process communication."},
    {"umicom.network.client", "networking", UMI_CAPABILITY_PLANNED,
     "HTTP, WebSocket, TLS, cancellation, and deadlines."},
    {"umicom.network.server", "networking", UMI_CAPABILITY_PLANNED,
     "HTTP server, routes, WebSocket, and management endpoints."},
    {"umicom.identity", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Human, service, plug-in, worker, and agent identities."},
    {"umicom.audit", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Bounded security and operational audit evidence."},
    {"umicom.metrics", "observability", UMI_CAPABILITY_IMPLEMENTED,
     "Counters, gauges, readiness, profiling, and exporters."},
    {"umicom.tracing", "observability", UMI_CAPABILITY_IMPLEMENTED,
     "Correlation, parent-child spans, completion, and trace snapshots."},
    {"umicom.commands", "application", UMI_CAPABILITY_PLANNED,
     "Application command, action, shortcut, and menu registry."},
    {"umicom.documents", "application", UMI_CAPABILITY_FOUNDATION,
     "Toolkit-neutral document identity, text, revisions, and persistence."},
    {"umicom.workspace", "application", UMI_CAPABILITY_IMPLEMENTED,
     "Workspace identity, project graph, trust state, file indexing, search, and restoration."},
    {"umicom.tasks", "application", UMI_CAPABILITY_IMPLEMENTED,
     "Cancellable tasks, progress, bounded workers, process jobs, deadlines, and timeouts."},
    {"umicom.problems", "application", UMI_CAPABILITY_PLANNED,
     "Unified compiler, runtime, security, and architecture findings."},
    {"umicom.git", "development", UMI_CAPABILITY_PLANNED,
     "Provider-neutral status, diff, history, branch, and merge operations."},
    {"umicom.ai.provider", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Local, remote, AuthorEngine and test provider contracts with policy-aware dispatch."},
    {"umicom.ai.retrieval", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Chunking, embeddings, bounded indexing, retrieval, sources, citations, and local RAG."},
    {"umicom.delivery", "delivery", UMI_CAPABILITY_PLANNED,
     "Build evidence, staging, packaging, signing, release, and rollback."},
    {"umicom.workflow", "enterprise", UMI_CAPABILITY_PLANNED,
     "Versioned workflows, approvals, retries, and compensating actions."},
    {"umicom.messaging.schema-registry", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Versioned schemas, compatibility rules, and payload validation."},
    {"umicom.messaging.channels", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Bounded owned-message channels with backpressure."},
    {"umicom.messaging.dispatcher", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Typed subscriptions, dispatch, routing, filtering, and transformation."},
    {"umicom.messaging.reliability", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Retries, idempotency, inbox, outbox, leases, and dead letters."},
    {"umicom.messaging.replay", "messaging", UMI_CAPABILITY_IMPLEMENTED,
     "Durable replay, aggregation, resequencing, and consumer positions."},
    {"umicom.messaging.workflows", "enterprise", UMI_CAPABILITY_IMPLEMENTED,
     "Executable flows, workflows, compensation, and saga state."},
    {"umicom.data.transactions", "persistence", UMI_CAPABILITY_IMPLEMENTED,
     "Memory and SQLite transactions with rollback."},
    {"umicom.data.migrations", "persistence", UMI_CAPABILITY_IMPLEMENTED,
     "Ordered schema versions and transactional migrations."},
    {"umicom.data.repositories", "persistence", UMI_CAPABILITY_IMPLEMENTED,
     "Repositories, blobs, cache, metadata, and lineage."},
    {"umicom.data.durable-messages", "persistence", UMI_CAPABILITY_IMPLEMENTED,
     "Durable message and journal stores for replay."},
    {"umicom.integration", "enterprise", UMI_CAPABILITY_IMPLEMENTED,
     "Message flows, routing, transformations, reliability, and workflows."},
    {"umicom.security.sessions", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Revocable sessions, roles, credentials, trust, and authorisation."},
    {"umicom.security.redaction", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Secret-aware diagnostic and export redaction."},
    {"umicom.resilience", "operations", UMI_CAPABILITY_IMPLEMENTED,
     "Retry, circuit breaking, rate limiting, bulkheads, and supervision."},
    {"umicom.plugin.contributions", "extension", UMI_CAPABILITY_IMPLEMENTED,
     "Typed commands, panes, tools, providers, and product contributions."},
    {"umicom.observability.readiness", "observability", UMI_CAPABILITY_IMPLEMENTED,
     "Readiness checks, operational events, profiling, and snapshots."},
    {"umicom.gui.gtk4", "presentation", UMI_CAPABILITY_PLANNED,
     "Primary GTK4 adapter for UI-neutral Framework contracts."},
    {"umicom.gui.headless", "presentation", UMI_CAPABILITY_PLANNED,
     "In-memory UI adapter for tests and automation."},
    {"umicom.designer", "development", UMI_CAPABILITY_PLANNED,
     "Semantic application model, palette, properties, undo, and preview."},
    {"umicom.helix", "automation", UMI_CAPABILITY_IMPLEMENTED,
     "Governed specialist agents, evidence, candidate fitness, approvals, release gates, and rollback."},
    {"umicom.ai.authorengine", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Process-boundary integration contract for Umicom AuthorEngine AI."},
    {"umicom.ai.coding-assistant", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Repository-aware coding task plans and reviewed, conflict-safe patch transactions."},
    {"umicom.ai.tools", "ai", UMI_CAPABILITY_IMPLEMENTED,
     "Permission-aware tool registry and human approval boundary for AI actions."},
    {"umicom.codeguard", "security", UMI_CAPABILITY_IMPLEMENTED,
     "Native C/C++ vulnerability scanning, architecture checks, duplicate detection, reports, and CI quality gates."},
    {"umicom.architecture.conformance", "development", UMI_CAPABILITY_IMPLEMENTED,
     "Public/private boundary, toolkit neutrality, Data Server authority, coupling, and consolidation checks."}
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
