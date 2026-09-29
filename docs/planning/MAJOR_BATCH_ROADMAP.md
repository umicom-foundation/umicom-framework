# Umicom: consolidated future major-batch roadmap

## Following Batch 42 · 29 September 2026

This is the full currently defined planning horizon: **Batch 42 is the focused source delivery; Batches 43–54 are carried forward; Batches 55–88 are proposed continuation work.** There are **46 future numbered entries**, not a promise of 46 release-ready products or a delivery-date commitment. The horizon can change after engineering and product review.

A numbered delivery is not the same as completion of its broader milestone. Source-only, native-tested, target-tested, integrated and release-qualified are different states. An enabled CMake message, a passing subset and a window that starts do not establish the same capability.

Critical build, memory, concurrency, data-loss and security fixes move into the first delivery that needs them. They do not wait for their later consolidation batch. Windows/GTK acceptance is the next priority before further capability claims. Existing Paper/Live broker access remains read-only; physical-device writes remain disabled until their dedicated acceptance gates pass.

## Shared architecture and delivery rules

Framework owns reusable services, widgets, models, tools and engines. Applications remain thin product compositions. Data Server is the database authority; Master Controller and Slave Controllers retain explicit lifecycles and responsibilities. C23 is primary, with Assembly for appropriate low-level work and C++ only at a justified boundary. Existing scripts remain alternatives, not the new primary feature engine.

Preserve existing APIs, implementation logic, authorship, licence notices, comments and tests. Superseded code is normally retained in an explained disabled block; physical deletion needs an explicit later decision. Every batch supplies full files, examples, a public beginner guide, tests and failure checks, a validation record, known limits, and build/test/Git steps. Publication remains on main, Framework before consumers and parent pins. Delivery roots remain `Umicom-Applications` and `umicomOS`.

Learning is continuous: each development batch adds a realistic project, exercises and answers. The education milestones organise and verify that material rather than postponing teaching.

## Earlier scope that remains open

| Earlier scope | What remains distinct from the delivered subset | Planned continuation |
|---|---|---|
| 19–27 / OS and delivery | Host tools and selected installer/VM/media paths were delivered, but real graphical/physical boot and complete maintenance qualification remain separate. | 43, 51–53, 84–86 |
| 31–32 / Studio and designer | Read-only build review and native export do not complete editor/debugger/Git or interactive designer authoring. | 45–46, 79–80 |
| 35 / Broker | Paper/Live inspection is read-only; authenticated provider qualification and transmission/reconciliation remain open. | 43, 49–50, 64 |
| 36 / Shared finance and TMS | The delivery repaired build integration; it did not complete the expanded front-to-back TMS milestone. | 56–58, 60, 82–83 |
| 37–38 / Bank and finance | Command review, close blockers and integrity checks do not complete products, real payments, exchange operations or authenticated approvals. | 49, 59–62, 82–83 |
| 39 / AI | Selected evidence and citation inspection do not constitute qualified neural inference, scalable RAG or agent execution. | 65–67 |
| 40 / Enterprise | Dataset browsing and re-preparation do not complete a query editor, scheduler, external integrations or signed extensions. | 68–70 |
| 41 / Creative | PCM16 clip editing/export does not complete audio playback, video timelines, CAD, games or mobile builds. | 71–76 |
| 42 / Education | This batch supplies native study planning and a curated 22-guide index. Expanded courses, verified practical execution and lesson package management remain open. | 77–78, 87 |

## Future major batches

### 43 — Windows and GTK Integration Qualification

**Status:** Carry-forward plan. **Owners/consumers:** Framework; all 24 applications; installers.

**Planned work:** Compile and exercise the real GTK and Win32 adapters; clean-machine startup, private DLLs/resources, Unicode paths, unrelated working directories, DPI, accessibility and multi-window lifecycle.

**Learning project:** Install Notes on a clean Windows account, deliberately remove a required resource, diagnose it and restore correct startup.

**Acceptance gate:** Named Windows build and installed-product journeys; no missing or disabled tests hidden by a success label.

### 44 — Framework Lifetime, Cancellation and Concurrency

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Studio; Desk; Operations; all workers.

**Planned work:** Audit owner/borrower contracts, callback lifetimes, worker join and shutdown, cancellation, lock ordering, event subscriptions and completion after window closure.

**Learning project:** Cancel a Notes indexer and close the window while it runs; show that no callback touches a destroyed owner.

**Acceptance gate:** Race/lifetime regression tests on the actual operating systems, alongside sanitizers and failure injection.

### 45 — Studio Editing, Debugging and Version Control

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Studio.

**Planned work:** Finish document save/reload/conflict recovery, multi-project navigation, language-server actions, debugger state, registers/memory, terminal sessions, Git staging/compare/merge and exact test provenance.

**Learning project:** Create a Notes project, fix a bounds error in the debugger, run its test, inspect and publish the intended change.

**Acceptance gate:** One complete IDE workflow from new project to installed executable, including failed build and recovery paths.

### 46 — Visual Designer Editing and Round-Trip Generation

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Studio; all application designers.

**Planned work:** Connect palette, hierarchy, typed properties, events, bindings, responsive layouts and preview to the native exporter; review regeneration without losing handwritten source.

**Learning project:** Build a Notes/account-summary window, regenerate it and preserve a handwritten command handler.

**Acceptance gate:** The generated project compiles and runs; unsupported features are rejected rather than silently omitted.

### 47 — Market Recorder, Historical Data and Feed Recovery

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Trader; Studio.

**Planned work:** Durable market capture, indexed ranges, corrections, depth deltas, separate event/arrival clocks, provider generations, missing segments and bounded backpressure.

**Learning project:** Record a permitted practice stream, inspect a gap and replay an explicitly repaired dataset.

**Acceptance gate:** Real capture adapter plus restart/replay consistency and data-rights review; reconnect is not represented as historical repair.

### 48 — Multi-Instrument Research and Reproducible Experiments

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Trader; Studio.

**Planned work:** Multi-stream ordering, partial-fill models, rejected instructions, fees, run manifests, strategy-binary identity, seek/checkpoints, parameter grids and walk-forward experiments.

**Learning project:** Compare two C strategies on frozen data, change costs, and separate training from unseen-period results.

**Acceptance gate:** No-look-ahead checks, independent accounting and retained input/configuration/binary identity; no promise of profit.

### 49 — Identity, Secrets, Approval and Package Trust

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Security Centre; all consequential-action consumers.

**Planned work:** Authenticated and revocable sessions, capability grants, secret providers, signed metadata, trust rotation, exact-input approvals, expiry and denial explanations.

**Learning project:** Revoke an approval before a fictional transfer; reject a modified signed test package.

**Acceptance gate:** Tampering, expiry, confused-deputy and key-rotation tests; required security fixes are pulled forward before any dependent action.

### 50 — Engine, Data and Risk Server Process Boundaries

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Operations; TMS; Trader; Bank.

**Planned work:** Real supervised processes, bounded messages, deadlines, fenced ownership, durable inbox/outbox, retries, dead letters and dependency-aware shutdown.

**Learning project:** Terminate an engine during a fictional trade workflow, then recover without duplicate postings.

**Acceptance gate:** Actual multi-process recovery evidence; clients never bypass Data Server to reach its database.

### 51 — Installer, Runtime Bundles and VM User Experience

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Setup Centre; VM Manager; Desk; all apps.

**Planned work:** Component registration, native update/repair/removal, shortcut ownership, signed offline releases, qualified QEMU bundle, path selection, console, graceful shutdown and disk-checkpoint inspection.

**Learning project:** Install two applications and a VM component; repair one file, boot the guest and remove one application without deleting the other.

**Acceptance gate:** Clean Windows install/maintenance and real QEMU tests; runtime redistribution is reviewed separately.

### 52 — Graphical Umicom OS and Persistent User Sessions

**Status:** Carry-forward plan. **Owners/consumers:** umicomOS; Framework; Desk; OS Control Centre.

**Planned work:** Boot a qualified graphical Linux profile using an established display stack, persistent user storage, session management, display/input configuration and an independent recovery route.

**Learning project:** Save a note in the guest, restart and recover it; reject a bad system update.

**Acceptance gate:** Actual graphical boot and persistent-volume tests per claimed architecture; the kernel/recovery remain independent of Framework.

### 53 — Physical USB and Optical-Media Qualification

**Status:** Carry-forward plan. **Owners/consumers:** Framework; Setup Centre; umicomOS.

**Planned work:** Stable device identity, system-disk exclusion, narrow elevation, explicit erase review, exclusive acquisition, flush/read-back, interruption handling and safe eject; optical burning qualified separately.

**Learning project:** Start with a disposable virtual disk, then use an explicitly identified spare device and verify its boot route.

**Acceptance gate:** Real supported hardware tests for wrong-device refusal, replacement/removal and incomplete writes. Device writes stay disabled until qualified.

### 54 — Integrated Release Candidate and Public Capstones

**Status:** Carry-forward plan. **Owners/consumers:** Framework; all qualified applications; qualified OS profile.

**Planned work:** Freeze a named candidate and capability matrix; package version-matched examples, dependencies, known limitations, cross-application journeys and recovery tests.

**Learning project:** Build Notes, replay research, reconcile a fictional financial workflow and boot the qualified OS.

**Acceptance gate:** Reproduce the declared subset on a clean supported host. This is not a declaration that every long-term product feature is complete.

### 55 — Desk Federation and Cross-Application Sessions

**Status:** Proposed extension. **Owners/consumers:** Framework; Desk; all applications.

**Planned work:** Complete launcher/taskbar, application groups, deep links, cross-application context, notifications, window switching, session restore and controlled application management.

**Learning project:** Resume a workspace containing Studio, Trader and a note without crossing account or project contexts.

**Acceptance gate:** Restart and failed-launch tests preserve the correct application identities and session ownership.

### 56 — Shared Financial Reference and Money Platform Completion

**Status:** Proposed extension. **Owners/consumers:** Framework; Bank; TMS; Trader; Accountant; Exchange.

**Planned work:** Complete common money/rounding, currencies, calendars, parties, books, accounts, instruments, business dates, state migrations and reference-data governance.

**Learning project:** Trace one fictional instrument and amount through all financial consumers without duplicate identity or rounding rules.

**Acceptance gate:** Contract compatibility, independent arithmetic/calendar checks and controlled migration of existing practice data.

### 57 — Open TMS Front-to-Back Trade Lifecycle

**Status:** Proposed extension. **Owners/consumers:** Framework; TMS; Accountant; Operations.

**Planned work:** Complete FX forwards, amendments, cancellations, deposits and loans, cashflows, positions, settlement obligations, accounting events and restartable end-of-day orchestration.

**Learning project:** Capture, amend, value, settle and reconcile a GBP/USD forward through each owning service.

**Acceptance gate:** Cash, positions and ledger reconcile after interruption at each lifecycle stage.

### 58 — Pricing, Portfolio Risk, Liquidity and ALM

**Status:** Proposed extension. **Owners/consumers:** Framework; TMS; Trader; Bank.

**Planned work:** Retained market snapshots, curves/calibration, sensitivities, scenarios, portfolio aggregation, cash ladders, liquidity forecasts and governed advanced-model adapters.

**Learning project:** Explain a portfolio change from market inputs through valuation and liquidity exposure.

**Acceptance gate:** Independent numerical references, deterministic populations, explicit missing data and stated model limits.

### 59 — Bank Products, Servicing and Lending

**Status:** Proposed extension. **Owners/consumers:** Framework; Bank; TMS.

**Planned work:** Account templates, deposits, interest, fees, holds, standing instructions, loan schedules, arrears, customer lifecycle and authenticated approval flows.

**Learning project:** Open practice products, accrue interest, process a repayment and explain booked/held/available balances.

**Acceptance gate:** Double-entry and restart invariants, permissions and explicit treatment of rounding and backdated changes.

### 60 — Payments, Statements and External Banking Adapters

**Status:** Proposed extension. **Owners/consumers:** Framework; Bank; TMS; Integration Studio.

**Planned work:** Payment routing, cut-offs, message generation, acknowledgements, repair/reversal, statements, nostro/vostro reconciliation and separately qualified network adapters.

**Learning project:** Follow a fictional payment through rejection, repair and matched statement confirmation.

**Acceptance gate:** Real authorised test-channel evidence; a sent message is not treated as settlement and timeouts do not trigger duplicate transfers.

### 61 — Accountant Full Close and Financial Reporting

**Status:** Proposed extension. **Owners/consumers:** Framework; Accountant; Bank; TMS.

**Planned work:** Charts and sub-ledgers, accruals, deferrals, depreciation, revaluation, budgets, consolidation, eliminations, period controls and reproducible statements.

**Learning project:** Close a multi-currency practice group and explain each consolidation adjustment.

**Acceptance gate:** Independent trial-balance/report checks; corrections preserve the original journals and approval history.

### 62 — Exchange Matching and Venue Operations

**Status:** Proposed extension. **Owners/consumers:** Framework; Exchange; Trader; Operations.

**Planned work:** Expanded order types, partial matching, cancel/replace, auctions, session states, self-trade controls, surveillance, fee schedules and recovery of venue state.

**Learning project:** Match a practice order book through a session transition and a restart.

**Acceptance gate:** Deterministic price/time rules, independent book invariants and separation of matching from settlement.

### 63 — Trader Professional Workstation Completion

**Status:** Proposed extension. **Owners/consumers:** Framework; Trader; Studio.

**Planned work:** Integrated watchlists, richer charts/studies, Time & Sales, DOM/price ladder, positions/P&L, alerts, scanners, economic calendar and correct linked account/market contexts.

**Learning project:** Investigate a market event across all linked panels while retaining source age and account identity.

**Acceptance gate:** Actual provider and graphical journeys; no fabricated live values or empty panels presented as completed features.

### 64 — Broker Paper and Deliberate Live Execution

**Status:** Proposed extension. **Owners/consumers:** Framework; Trader; Risk; Operations.

**Planned work:** Qualify the broker adapter, market subscriptions, orders/fills, cancel/replace, reconnect reconciliation, durable request identity, pre-trade risk, kill controls and execution audit.

**Learning project:** Complete paper-order partial-fill/cancel recovery before a separately authorised restricted live acceptance procedure.

**Acceptance gate:** Live execution needs explicit environment/account selection plus identity, risk, reconciliation and provider qualification. Existing Live access remains read-only until then.

### 65 — Local AI Runtime and Model Operations

**Status:** Proposed extension. **Owners/consumers:** Framework; LLM; Studio; Operations.

**Planned work:** Real local inference, model/provider compatibility, resource budgets, streaming cancellation, supervised workers, permitted accelerator paths and model inventory.

**Learning project:** Run a small supported model, cancel a request and recover from provider loss without leaking resources.

**Acceptance gate:** Actual inference evidence kept separate from inert HTTP fixtures and extractive previews.

### 66 — RAG Ingestion and Scalable Retrieval

**Status:** Proposed extension. **Owners/consumers:** Framework; RAG; Studio; Database Studio.

**Planned work:** Versioned ingestion, chunking, embeddings, durable indexes, hybrid retrieval, reranking, source permissions, deletion propagation and navigable citations.

**Learning project:** Index a changing Notes handbook and show which answer used which immutable passage.

**Acceptance gate:** Scale and source-revision tests; citations are checked structurally without claiming automatic truth verification.

### 67 — AI Creator and Governed Helix Workflows

**Status:** Proposed extension. **Owners/consumers:** Framework; Creator; Studio; LLM; Operations.

**Planned work:** Resumable text/media jobs and reviewed tools; candidate source directories, bounded specialist workers, approved test execution and controlled promotion.

**Learning project:** Generate a candidate Notes improvement, review its source diff and test results, then explicitly accept it.

**Acceptance gate:** Models cannot bypass command authority, edit running production state, or push repositories automatically.

### 68 — Database Studio Operational Completion

**Status:** Proposed extension. **Owners/consumers:** Framework Data Server; Database Studio; Operations.

**Planned work:** Schema browsing, controlled query sessions, explain plans, paged results, parameter binding, migration review, backup/restore and supported database adapters.

**Learning project:** Inspect a practice schema, run a bounded query and recover a failed migration through Data Server.

**Acceptance gate:** Real database adapter tests, resource limits and preserved database authority; no application-local SQL engine.

### 69 — Integration Studio and Operations Execution

**Status:** Proposed extension. **Owners/consumers:** Framework; Integration Studio; Operations.

**Planned work:** Mapping/flow designer, real connectors, scheduler, bounded retries, dead letters, replay, correlated service health, incidents and operational runbooks.

**Learning project:** Recover a failed stock-list flow without duplicating its accepted records.

**Acceptance gate:** At least one authorised real connector, crash/retry tests and business-level completion indicators.

### 70 — Security Centre and Marketplace Operations

**Status:** Proposed extension. **Owners/consumers:** Framework; Security Centre; Marketplace; Setup Centre.

**Planned work:** Permission inspection, session revocation, trust-store management, signed package provenance, extension isolation, activation review and rollback.

**Learning project:** Inspect requested permissions, reject a tampered extension and revoke its active capability.

**Acceptance gate:** Actual isolated-host and update paths with auditable outcomes; a manifest is not treated as a trusted signature.

### 71 — Media Studio Timeline and Video Workflow

**Status:** Proposed extension. **Owners/consumers:** Framework media services; Media Studio; Creator.

**Planned work:** Supported video/audio import, timeline trim, caption/title tracks, preview, render jobs, codec adapters, cancellation and new-file export.

**Learning project:** Create and export a short captioned workshop clip while keeping the original assets.

**Acceptance gate:** Actual decodes/renders and independently inspected output; storyboard HTML is not labelled encoded video.

### 72 — Music Studio Playback, Mixing and Recording

**Status:** Proposed extension. **Owners/consumers:** Framework audio services; Music Studio.

**Planned work:** Real audio-device playback, multi-track arrangement, mixing, metering, automation, supported recording, device changes and render/export.

**Learning project:** Play a workshop tune, mix two tracks and export a checked result without blocking the audio callback.

**Acceptance gate:** Real-device tests, callback lifetime/resource budgets and independent audio-format checks.

### 73 — CAD and Kitchen Designer Parametric Workflows

**Status:** Proposed extension. **Owners/consumers:** Framework designer/geometry services; CAD; Kitchen Designer.

**Planned work:** Measured 2D/3D geometry, constraints, snapping, units, drawing interchange, parametric cabinets, materials, dimensions and bills of materials.

**Learning project:** Design a measured room and cabinetry, change one dimension and reconcile the material list.

**Acceptance gate:** Independent geometric/unit checks, undo/redo and round-trip file tests; visual placement alone is not dimensional proof.

### 74 — Games Runtime and Scene Editor

**Status:** Proposed extension. **Owners/consumers:** Framework scene/input/runtime services; Games; Studio.

**Planned work:** Scene/entity lifecycle, input actions, animation, collision/physics boundaries, assets, save state, debugging and reproducible packaging.

**Learning project:** Build a small workshop exploration scene with keyboard/controller input and saved state.

**Acceptance gate:** Real frame/input tests and asset lifetime checks on declared platforms.

### 75 — Web Studio Native Runtime and Deployment

**Status:** Proposed extension. **Owners/consumers:** Framework native web services; Web Studio; Studio.

**Planned work:** Server-driven UI, routing, forms, sessions, accessibility, static assets, realtime channels, API contracts and deployable host profiles.

**Learning project:** Build and deploy a Notes web application backed by the same service contracts.

**Acceptance gate:** Actual HTTP/browser and permission/session tests; a visual HTML prototype is not a deployed application.

### 76 — Mobile Studio and Portable Application Profiles

**Status:** Proposed extension. **Owners/consumers:** Framework; Mobile Studio; Studio; Bank/Notes profiles.

**Planned work:** Touch/responsive components, lifecycle, offline state, mobile build hosts, packaging, device permissions and supported native adapter boundaries.

**Learning project:** Build a Notes profile for a named mobile target and recover it after suspension.

**Acceptance gate:** Actual supported-device/emulator journeys; required platform-language glue is isolated and justified.

### 77 — Education Studio Courses and Verified Practical Work

**Status:** Proposed extension. **Owners/consumers:** Framework education/teacher services; Education; Studio.

**Planned work:** Expand the full beginner-to-advanced curriculum, prerequisites, versioned lesson packages, controlled practical runners, retained test provenance and instructor support.

**Learning project:** Progress from a first C record to a tested, packaged Notes application and a financial capstone.

**Acceptance gate:** Quiz credit, self-reported study and verified executable results remain distinct; no untrusted code is run without a qualified boundary.

### 78 — AuthorEngine, Bits to Banking and Public Documentation

**Status:** Proposed extension. **Owners/consumers:** Framework publishing services; AuthorEngine; Education; Studio; project websites.

**Planned work:** Single-source manuals, public API/help generation, searchable offline books, examples tied to source versions, accessible assets and release-linked publishing.

**Learning project:** Publish a chapter from the same tested source example used in Education Studio.

**Acceptance gate:** Broken-link/example checks and explicit source/version/licence attribution; existing volumes retained rather than silently replaced.

### 79 — Native Compiler, SDK and Toolchain Productisation

**Status:** Proposed extension. **Owners/consumers:** Framework compiler/build/package services; Studio; Education.

**Planned work:** Qualify native compiler/IR tools, toolchain discovery, installed C ABI SDKs, source-built dependency profiles, cross compilation and reproducible artifacts.

**Learning project:** Build a small library, consume it from a clean SDK installation and inspect generated machine code.

**Acceptance gate:** Language conformance and ABI/link tests; an experimental compiler is not claimed to replace a mature compiler before qualification.

### 80 — Multi-Frontend Component Conformance

**Status:** Proposed extension. **Owners/consumers:** Framework GUI contracts; GTK4 primary; optional adapters.

**Planned work:** Finish shared widget catalogue, layouts, accessibility and semantic conformance across selected GTK4, native web and justified Qt/wxWidgets/Wt adapter profiles.

**Learning project:** Render the same Notes command/view model on two supported frontends without duplicate domain logic.

**Acceptance gate:** Per-adapter compilation, lifecycle, input and accessibility evidence; no promise that every toolkit is already implemented.

### 81 — Digital Assets, Crypto Exchange and UmiCOIN Research

**Status:** Proposed extension. **Owners/consumers:** Framework finance/digital-asset services; proposed digital-asset product profiles.

**Planned work:** Networks, custody boundaries, deposits/withdrawals, finality/reorganisation handling, token supply, proof-of-liabilities and exchange/accounting integration.

**Learning project:** Use an isolated test network to reconcile token movements and a deliberate reorganisation.

**Acceptance gate:** No production keys or real funds by default; supported network/security and legal requirements reviewed before deployment.

### 82 — Clearing, Margin, Collateral and Custody

**Status:** Proposed extension. **Owners/consumers:** Framework; Exchange; TMS; Bank; proposed CCP/custody profiles.

**Planned work:** Clearing obligations, netting, margin, collateral eligibility/haircuts, custody movements, repo/securities lending and bounded default-management experiments.

**Learning project:** Trace a practice cleared trade through collateral allocation and reconciled cash/securities settlement.

**Acceptance gate:** Independent financial calculations and failure recovery; institutional CCP qualification remains a separate claim.

### 83 — Regulatory Reporting and Data Governance

**Status:** Proposed extension. **Owners/consumers:** Framework; TMS; Bank; Accountant; Exchange; Operations.

**Planned work:** Lineage, data-quality rules, immutable report populations, maker/checker submissions, acknowledgements, restatements, retention and jurisdiction adapters.

**Learning project:** Produce a versioned practice report, reject a missing source and reconcile a restatement.

**Acceptance gate:** Implement any real obligation from current authoritative rules at delivery time; no generic compliance certification.

### 84 — Umicom OS System Administration and Recovery

**Status:** Proposed extension. **Owners/consumers:** umicomOS; Framework user space; Desk; OS Control Centre.

**Planned work:** Processes, storage, networking, service control, user management, privileged-operation review, system installer, updates and rollback/recovery architecture.

**Learning project:** Diagnose a failed service and select a known system generation without destroying personal data.

**Acceptance gate:** Actual VM/system tests with narrow privilege boundaries and independently bootable recovery.

### 85 — Umicom OS RISC-V and Further Hardware Ports

**Status:** Proposed extension. **Owners/consumers:** umicomOS; Framework portability; Studio toolchains.

**Planned work:** Qualify RISC-V boot/userland, board support, device drivers, cross builds and selected ARM/other platform experiments.

**Learning project:** Cross-build, boot and run a native Umicom lesson on a named supported board or emulator.

**Acceptance gate:** Per-platform real boot evidence, ABI and device tests; portability headers alone do not count.

### 86 — Backup, Disaster Recovery, Scale and Resilience

**Status:** Proposed extension. **Owners/consumers:** Framework; Operations; Data/Engine/Risk services; all persistent apps.

**Planned work:** Consistent backup/restore, migration recovery, retention, failover/fencing, stress/load baselines, resource limits and long-running failure campaigns.

**Learning project:** Restore a practice estate and reconcile authoritative business and learning state.

**Acceptance gate:** Measured recovery objectives, integrity checks and actual restore drills; sanitizer success is not universal race/leak proof.

### 87 — Accessibility, Localisation and Learning Usability

**Status:** Proposed extension. **Owners/consumers:** Framework; all applications; Education; documentation.

**Planned work:** Keyboard/screen-reader journeys, high contrast, international text/input, locale-neutral storage, explicit display formatting and reviewed translations.

**Learning project:** Complete a Notes and learning workflow using keyboard and assistive technology in two supported locales.

**Acceptance gate:** Human and automated accessibility tests plus unchanged canonical financial/storage semantics.

### 88 — Whole-Portfolio Release Qualification

**Status:** Proposed extension. **Owners/consumers:** Framework; all release-scoped apps; qualified Umicom OS.

**Planned work:** Close remaining gates, publish a supported-capability matrix and packages, freeze matched manuals and run installation, migration, interoperability and recovery capstones.

**Learning project:** Reproduce the declared product suite from checkout through clean-host installation and published user journeys.

**Acceptance gate:** Only evidenced capabilities enter the release claim. Unfinished domains remain visible in the next planning revision.

## Coverage of every current application

The current Applications repository declares 24 application submodules, plus Framework. This is an inventory of declared products, not proof of their readiness. OS Control Centre is an application module; `umicomOS` is the separate operating-system repository. All applications also participate in the relevant shared 43, 44, 49, 51, 86–88 acceptance work.

| Product | Principal batches |
|---|---|
| Umicom Framework | 43, 44, 49, 50, 56, 79, 80, 86, 87, 88 |
| Umicom Studio IDE | 45, 46, 48, 67, 76, 77, 78, 79, 80 |
| Umicom Trader | 47, 48, 58, 62, 63, 64, 81, 82 |
| Umicom Open TMS | 50, 56, 57, 58, 60, 82, 83 |
| Umicom Bank | 49, 56, 58, 59, 60, 82, 83 |
| Umicom Accountant | 56, 61, 82, 83 |
| Umicom Exchange | 56, 62, 81, 82, 83 |
| Umicom Desk | 43, 51, 52, 55, 84, 87 |
| Umicom OS Control Centre | 52, 53, 84, 85 |
| Umicom LLM | 65, 67, 86 |
| Umicom RAG | 66, 67, 86 |
| Umicom AI Creator | 65, 66, 67, 71 |
| Umicom Music Studio | 72, 86, 87 |
| Umicom Media Studio | 67, 71, 86, 87 |
| Umicom CAD | 46, 73, 80, 87 |
| Umicom Kitchen Designer | 46, 73, 80, 87 |
| Umicom Games | 74, 79, 80 |
| Umicom Web Studio | 75, 80, 87 |
| Umicom Mobile Studio | 76, 80, 87 |
| Umicom Database Studio | 50, 68, 86 |
| Umicom Integration Studio | 50, 60, 69, 83 |
| Umicom Operations | 44, 50, 69, 84, 86 |
| Umicom Security Centre | 49, 70, 86 |
| Umicom Marketplace | 49, 51, 70 |
| Umicom Education Studio | 42, 77, 78, 87 |
| umicomOS repository | 51, 52, 53, 84, 85, 86, 88 |

## Named longer-term directions and scope boundaries

AuthorEngine, Helix, the native compiler/toolchain and alternative GUI backends are treated as shared platform capabilities with the consumers above, not invented new current submodules. Digital-asset/Crypto Exchange/UmiCOIN and CCP/custody product profiles are proposed in 81–82; they are not claimed to exist as registered top-level application modules today. The historical office and medical application directions remain unsequenced: no new product names, deadlines or implementations are invented for them here.

The eight original computing/finance study tracks and later domain manuals are reference material, not evidence that a present application implements every chapter. Licensed external platforms and jurisdiction-specific requirements must be checked at implementation time rather than copied into the C core as assumptions.

## Managing dependencies

The practical chains are developer reliability **43 → 44 → 45 → 46**, recorded research **47 → 48**, trust and service deployment **49 → 50 → 51**, and qualified OS delivery **52 → 53**. Finance completion builds on identity, durable services and shared financial contracts; live broker transmission must not precede those controls. Creative and enterprise products share the same ownership, cancellation, storage and packaging infrastructure.

54 qualifies a declared subset; it does not close the specialised 55–88 backlog. 88 qualifies a declared whole-portfolio release scope, not every imaginable future application. Split a large milestone into coherent source deliveries when necessary, but retain its unfinished acceptance criteria explicitly.

## Source and decision record

Planning basis: the current September batch sequence (including the previously supplied Batches 35–54 roadmap), the visible delivery records, the live Applications submodule inventory at 6478c2c5e0be7220aa06b290f4ce5874cad6b7bf, and the historical Umicom architecture/product documents. Batches 43–54 retain the earlier plan. Batches 55–88 are a proposed extension introduced by this consolidation, not previously completed work or a claim of prior approval. Historical batch/volume numbering is not silently renumbered into this series.
