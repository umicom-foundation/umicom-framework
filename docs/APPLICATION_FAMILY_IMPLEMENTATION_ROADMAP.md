<!--
Umicom Framework
File: docs/APPLICATION_FAMILY_IMPLEMENTATION_ROADMAP.md

PURPOSE:
Define a Framework-first development order and the reusable capabilities that
each Umicom application should adopt instead of implementing locally.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Family Implementation Roadmap

## The rule that guides every update

Build the reusable part in Umicom Framework first. An application should keep
only its product identity, specialist workflows and adapter choices. If two
applications need the same panel, layout behaviour, validation rule or
background-job pattern, that piece belongs in the Framework.

Before writing a new module, search the existing catalogue. The Framework is
already large and contains many foundations. The next stage is often to join,
verify and adopt existing parts, not create a second version with a new name.

## Development order

The agreed order is:

1. Complete and stabilise Umicom Framework.
2. Make Umicom Studio IDE the reference application for developer workbenches.
3. Make Umicom Trader the reference application for linked real-time
   workstations.
4. Use those proven Framework pieces to complete the remaining applications.

This order does not mean other applications are ignored. Their requirements
are used to design Framework contracts now, so the shared pieces are not tied
only to Studio or Trader.

## Framework capabilities to finish and prove

| Shared capability | What “complete” means | First proving applications |
| --- | --- | --- |
| Application bootstrap | One lifecycle, dependency scope, configuration and shutdown contract for every frontend. | Studio, Web Studio, Bank |
| Experience catalogue | Panels, layouts, commands and feature ownership resolve without hard-coded repository paths. | All applications |
| Workbench composition | Dock, split, tab, float, auto-hide, multi-monitor, responsive layout, save and restore. | Studio, Trader, Media |
| Context-linked panels | Typed selection, colour groups, safe active/passive endpoints, history, inspection and persistence. | Studio, Trader, TMS, Bank |
| Commands and permissions | Commands have stable IDs, enablement, permission checks, shortcuts, audit and undo boundaries. | Studio, Bank, TMS |
| Forms and state | Schema-backed fields, validation, change tracking, history, transitions and accessible errors. | Bank, TMS, Accountant |
| Data views | Reusable tree, grid, filter, sort, grouping, paging, streaming, export and virtualisation contracts. | Studio, Trader, Database Studio |
| Jobs and operations | Progress, cancellation, retry, scheduling, logs, health and recovery are consistent. | RAG, OS, Integration Studio |
| Documents and editors | Document identity, dirty state, autosave, recovery, navigation, diagnostics and language services. | Studio, Author, Web Studio |
| Designer surfaces | Palette, canvas, hierarchy, properties, snapping, constraints, preview and generated-code boundary. | Web Studio, Mobile Studio, CAD |
| Security and evidence | Authentication boundary, secrets, permissions, policy, audit, redaction and release evidence. | Bank, Security Centre, LLM |
| Adapter boundary | Network, database, broker, model, device and operating-system providers stay replaceable. | Trader, LLM, OS |
| Accessibility and themes | Keyboard operation, focus, screen-reader meaning, contrast, scaling and theme tokens. | All graphical applications |
| Packaging and updates | Install, package, update, rollback and dependency evidence use the Umicom command. | All applications |

## Reference application plans

### Umicom Studio IDE

Studio should prove the complete desktop workbench. Its next vertical slices
are project and solution navigation, multi-document editing, language-server
sessions, build/test/debug orchestration, source control, extensions, visual
designers, remote workspaces, services, output and diagnostics. The Framework
must own the docking shell, command system, documents, editor contracts,
selection providers, services view, task progress and saved layouts.

### Umicom Trader

Trader should prove fast linked workstations. Its next vertical slices are
watchlists, market depth, charts, order tickets, order state, executions,
positions, account views, alerts, scanners, option tools, risk scenarios,
what-if portfolios and saved multi-monitor mosaics. The Framework must own the
linked panels, grids, charts, streaming state, safe command confirmation,
layout persistence and broker-adapter contracts. Trading decisions and broker
rules remain outside generic UI components.

## Plans for the remaining application catalogue

| Application | Product work to implement or deepen | Framework pieces it should reuse or help prove |
| --- | --- | --- |
| AI Creator | Prompt projects, generation graph, variants, review, provenance, asset library and export. | AI jobs, approvals, media canvas, artifact history, provider adapters. |
| Bank | Multi-currency accounts, beneficiaries, transfers, cards, FX quote review, connected banks, statements, budgeting and security controls. | Payment forms, account context, approvals, ledger views, audit, notifications, adapter boundary. |
| TMS | Trade capture, pricing, positions, lifecycle events, risk, limits, collateral, settlement, reconciliation and task station. | Forms, workflow state, trade/account context, valuation grids, evidence, integration jobs. |
| LLM | Local/cloud model catalogue, chat projects, tool permissions, context inspection, usage, model comparison and research. | Provider routing, streaming messages, RAG, approvals, secrets, artifacts, metrics. |
| Exchange | Instruments, order books, matching views, market surveillance, clearing, settlement, membership and operations. | Trading grids, charts, context links, workflow, risk, audit, high-volume event adapters. |
| Music | Arrangement timeline, clip/session grid, MIDI editor, mixer, automation, generative audio, stems and mastering. | Media timeline, transport, node graph, properties, assets, undo, background rendering. |
| Media | Video timeline, storyboard, compositing, subtitles, avatars, review, versions, render queue and delivery. | Media canvas, timeline, transport, jobs, annotations, asset catalogue, multi-monitor layout. |
| Accountant | Ledger, sales, purchases, banking, reconciliation, tax, payroll, HR, reporting and period close. | Forms, grids, account context, workflow, documents, approvals, audit and reporting. |
| RAG | Collections, source connectors, ingestion, chunk inspection, hybrid retrieval, reranking, evaluation and citations. | Jobs, data sources, permissions, knowledge panels, provenance, metrics and model adapters. |
| Desktop | Application launcher, task surfaces, notifications, search, session restore, settings and user workspaces. | Shell, command palette, layout service, notifications, application bootstrap and themes. |
| OS Control Centre | System inventory, storage, network, services, processes, updates, logs and developer controls. The production distribution uses a maintained Linux LTS kernel, FreeBSD is a portability target, and original microkernel work remains independent research. | Service panels, metrics, jobs, privilege boundary, platform adapters and the versioned OS architecture-decision contract. Kernel, boot and recovery work remains external to Framework. |
| Games | Scene hierarchy, game view, assets, inspector, animation, scripting, profiler, build and packaging. | Designer canvas, media timeline, documents, debug, assets, jobs and deployment. |
| Kitchen | Floor-plan design, equipment catalogue, constraints, costing, documentation and preview. | Designer, properties, snapping, validation, documents, reports and component recipes. |
| CAD | Parametric model tree, sketches, constraints, properties, layers, assemblies, measurements and export. | High-precision designer contracts, history, properties, jobs, file adapters and undo. |
| Author | Outline, rich document editor, research sources, citations, revisions, comments, publishing and export. | Documents, editor, knowledge context, history, collaboration, search and delivery. |
| Web Studio | Responsive canvas, component palette, HTML/CSS/JS editor, properties, preview, browser diagnostics, accessibility and deployment. | Designer, documents, language services, browser adapter, diagnostics, tests and packaging. |
| Mobile Studio | Device canvas, components, navigation flows, properties, code, preview, device bridge, tests and packaging. | Designer, documents, device adapters, responsive constraints, diagnostics and delivery. |
| Database Studio | Connections, schema explorer, SQL editor, results, execution plans, data editing, migration and monitoring. | Secrets, explorer, editor, virtual grids, jobs, history, database adapters and audit. |
| Integration Studio | Connector catalogue, flow designer, mapping, transforms, testing, deployment, schedules and operations. | Node graph, forms, secrets, jobs, diagnostics, versioning and integration adapters. |
| Operations | Application health, services, jobs, queues, incidents, runbooks, logs, metrics and deployment state. | Service panels, observability, commands, permissions, jobs, evidence and notifications. |
| Security Centre | Identity posture, permissions, secrets, vulnerabilities, policies, audit, incidents and remediation. | Security contracts, evidence, grids, workflow, notifications, redaction and adapters. |
| Marketplace | Product catalogue, search, details, installation, updates, licences, reviews and publisher tools. | Package metadata, downloads, signatures, permissions, progress, rollback and audit. |
| Education | Courses, lesson authoring, resources, exercises, assessment, progress, teacher views and accessibility. | Documents, media, forms, workflow, jobs, reporting, permissions and responsive layouts. |

## How to choose the next coding task

Use this checklist:

1. Pick one user journey that can work from beginning to end.
2. List every panel, command, state and adapter used by that journey.
3. Search the Framework for existing contracts before adding files.
4. Improve the reusable contract if more than one application can use it.
5. Keep provider-specific or regulated behaviour behind an adapter.
6. Add focused tests for the contract and one reference application adoption.
7. Document the feature in plain language with a small example.
8. Run static checks first, then the user runs the complete build and test suite.

The goal is that every new file owns a
clear responsibility and reduces the amount of code future applications need
to write.

## Umicom OS delivery boundary

Umicom OS follows three deliberate tracks: a Linux LTS production distribution,
a FreeBSD portability target, and a separately governed original microkernel
research project. Umicom Framework performs the reusable work in normal user
space. The production kernel and recovery environment never depend on the full
Framework, so boot and repair remain available when application services are
not running.

The installable distribution belongs in `umicom-os`, original kernel research
belongs in `umicom-kernel`, portable user-space services belong in
`umicom-framework`, and the thin Control Centre presentation belongs in
`applications/os`. A research kernel must pass documented security,
compatibility, reliability, performance and recovery gates before it can become
a selectable product profile. Making it the default requires another explicit
architecture decision.
