# Umicom: the next twenty major batches
## Batches 35–54, following Batch 34

This is the planned sequence after **Batch 34 — Native Strategy Replay and Backtest Safety**. Batch 34 is the current source delivery; the twenty entries below are future work, not completed capabilities or a delivery-date commitment. Batches 35–42 retain the subjects in the earlier roadmap. Batches 43–54 extend it with integration, scale and release qualification.

A major batch is complete only when its stated user journey works on the claimed target. A successful headless test, an enabled CMake option and an application that starts are three different results. Windows/GTK integration, real provider connections, graphical OS boot and physical-media qualification remain explicit gates; their earlier deferral must not be hidden by a new batch number. A prerequisite fix moves into the first batch that needs it. It does not have to wait for a later consolidation milestone.

## The common delivery contract

Each batch supplies full repository files, focused and integration tests, failure-path checks, a public beginner guide, a complete example, exercises with answers, and build/test/publication instructions. Guides use the Umicom logo and icon. Examples use fictional records until a separately approved provider environment is selected.

C23 is the default implementation language. Assembly serves architecture-specific needs or measured optimisation; C++ is confined to a justified library or vendor boundary. Existing script implementations remain alternatives. Reusable behaviour, widgets, tools and services belong to Framework; the Master Controller and Slave Controllers retain explicit responsibilities. Data Server remains the database authority. Superseded source and important comments remain for review, normally in documented disabled blocks. Source overlays retain the roots **Umicom-Applications** and **umicomOS**.

Publication stays on main. Only changed repositories receive source commits; Framework is published before consumers and parent pins. The exact state of an installed product is recorded independently of the most recent source commit.

## 35 — IBKR Paper Connectivity and Execution Controls

**Products:** Framework, Trader; Studio for inspecting development results.

**Planned work:** Qualify a real paper connection; normalise instruments, account identities, market subscriptions, orders, fills and positions; introduce durable request identities and reconnect reconciliation. Add visible paper/live identity, pending acknowledgement states, cancel/replace handling, order limits and a stop-new-orders control. Timeout means an unknown outcome until reconciliation establishes otherwise, not permission to submit the same order again.

**Architecture and tools:** Keep the provider-neutral public contract in C. Isolate any required vendor C++ SDK and its event-loop ownership. Use Framework risk, persistence, secrets and audit rather than a second trading engine inside a widget. Include an adapter diagnostic view and a recorded callback comparison tool.

**Learning project:** Follow one paper order from submission through partial fill and cancellation; interrupt the connection and establish the final provider state.

**Acceptance gate:** Real paper-provider evidence plus duplicate, delayed and out-of-order callback tests. Live trading is not enabled by completing a simulation or by compiling the adapter.

## 36 — Shared Finance and Open TMS Front-to-Back Workflows

**Products:** Framework, TMS; shared Bank and Accountant contracts.

**Planned work:** Extend canonical currencies, calendars, parties, books, instruments and cashflows. Complete an expanded FX-forward lifecycle, add a bounded deposit/loan workflow, and connect amendments to positions, valuations, settlement obligations and accounting events. Add cash ladders, exception queues and restartable end-of-day runs with explicit completion criteria.

**Architecture and tools:** Reuse one instrument and business-event identity across applications. Pricing and risk consume named, retained market snapshots. Engine Server orchestrates processing; Data Server owns committed state. Failed downstream processing remains an operational exception rather than silently changing the economic trade.

**Learning project:** Capture a fictional GBP/USD forward, amend it, revalue it, inspect its cashflows, settle it and reconcile the journal.

**Acceptance gate:** Independent calculation cases and cash/position/ledger reconciliation, including interruption between stages. The batch does not imply support for every asset class or production pricing model.

## 37 — Banking Products, Payments and Customer Operations

**Products:** Framework and Bank, with shared TMS/payment services.

**Planned work:** Extend account templates, multi-currency balances, holds, restrictions, fees, interest, standing instructions, beneficiary approval, payment repair queues and statements. Connect maker/checker decisions to authenticated principals and approved permissions. Expand simulated card authorisation, capture and reversal; a real network remains a separately qualified adapter.

**Architecture and tools:** Centralise money arithmetic, reservation semantics, journals and payment identifiers. Views request operations; they cannot bypass ledger authority or mutate balances. Add a balance explanation panel tracing booked, held and available amounts to their source events.

**Learning project:** Open fictional accounts, place a hold, approve a transfer, process a reversal and reconcile the statement without editing the database directly.

**Acceptance gate:** Concurrency, insufficient funds, expired approval, duplicate callback and restart tests preserve balanced journals. No regulatory readiness or real banking connectivity is inferred from local examples.

## 38 — Accountant, Exchange and Post-Trade Control

**Products:** Framework, Accountant, Exchange, TMS and Bank.

**Planned work:** Improve charts of accounts, sub-ledgers, accruals, revaluation, trial balances and period controls. Expand exchange price/time rules, partial matching, cancellation and bounded auction experiments. Connect executions to clearing obligations, settlement exceptions, custody movements and collateral-reservation foundations.

**Architecture and tools:** Matching, settlement and accounting remain separate authorities joined by recorded events. Add reconciliation views that show the original event, derived state and unresolved break. Corrections reverse or supersede recorded business effects; they do not erase the original trade.

**Learning project:** Match fictional orders, settle cash and holdings, investigate a deliberate break and close the period only when required controls pass.

**Acceptance gate:** Cash, holdings and journal totals agree across products. Failure after matching but before settlement is recoverable and visible. Complete central-counterparty default management remains additional scope.

## 39 — AI Runtime, RAG, Creator and Reviewed Agent Work

**Products:** Framework, LLM, RAG, Creator and Studio.

**Planned work:** Qualify real local-model execution with resource limits and cancellable generation. Expand ingestion, revision-aware chunks, embeddings, hybrid retrieval, reranking and navigable citations. Add resumable text and supported media jobs. Agents can prepare source changes in separate candidate directories, show their assumptions and run approved tests before proposing a merge.

**Architecture and tools:** Share model-provider, retrieval, job and tool-permission contracts. Retain the extractive preview under its existing honest label. Keep the model outside payment/trading authority; a generated sentence is not an approved command. Candidate generation does not mean automatic repository push.

**Learning project:** Index the Notes manual, inspect the passage supporting an answer, then review and test a suggested code change.

**Acceptance gate:** Real-model evidence is separate from fixtures. Provider loss, cancellation, malicious retrieved instructions and denied tools do not cause unauthorised effects.

## 40 — Enterprise Data, Integration and Operations

**Products:** Framework, Database Studio, Integration Studio, Operations, Security Centre and Marketplace.

**Planned work:** Add controlled schema/query sessions and paged results through Data Server. Extend mappings, validation, scheduled flows, retries, dead-letter queues and recorded replay. Connect operational dashboards to actual service health and incidents. Marketplace gains package verification, activation review and a visible permission request.

**Architecture and tools:** Reuse credential, workflow, package and audit services. Database drivers stay behind Data Server; a query panel is not a new direct connection authority. Keep existing import recipes as supported introductory profiles and preserve their saved jobs.

**Learning project:** Move a fictional stock list through a scheduled mapping, introduce a bad row, inspect the failed step and recover without duplicating accepted records.

**Acceptance gate:** At least one real database adapter and one real integration adapter work end to end. Revocation, retry and replay produce traceable results. A manifest alone is not proof that a plug-in is trusted.

## 41 — Creative and Engineering Application Workflows

**Products:** Framework, Media, Music, CAD, Kitchen, Games, Web Studio and Mobile Studio.

**Planned work:** Extend assets, scenes, timelines and export jobs. Deliver a bounded video import/trim/export journey and actual music playback/export. Add measured 2D constraints, drawing interchange, parametric cabinets and bills of materials. Improve game input/scene handling and connect web/mobile prototypes to explicitly supported build hosts.

**Architecture and tools:** Share asset identities, scene models, rendering, undo and background jobs while retaining domain-specific rules. Audio callbacks cannot perform blocking database or UI work. Existing SVG, HTML and WAVE paths remain available.

**Learning projects:** Make a captioned short clip and tune, then plan a room from measured cabinet parts and compare the bill of materials with the scene.

**Acceptance gate:** Every advertised workflow has a working import/edit/export path, format limits and failure tests. This is not a promise of complete 3D manufacturing CAD or all mobile-store deployment targets.

## 42 — Education Studio and the Published Learning Library

**Products:** Framework, Education, Studio and AuthorEngine documentation services.

**Planned work:** Assemble progressive C, Assembly, Framework, GUI, Git, debugging, finance and OS courses. Add prerequisites, lesson search, hints, self-checks, milestones and versioned course packages. Extend native documentation extraction, example discovery and broken-link checking.

**Architecture and tools:** One maintained example source feeds the lesson, Studio help and published guide. Retain historical editions with compatibility labels. Quiz completion, compiler success, test execution and independent assessment are separate observations.

**Learning capstone:** Build Notes, add an account-summary component, test the result, package it and explain its memory and data ownership.

**Acceptance gate:** Each exercise builds against its declared SDK; expected failures fail; repaired versions pass; offline navigation works. Education remains part of every preceding batch, not work postponed until this milestone.

## 43 — Windows and GTK Integration Qualification

**Products:** Framework, Studio, Trader, Desk, Bank, Setup Centre and affected modules.

**Planned work:** Close the target-platform gaps carried by earlier focused deliveries. Compile the real GTK and Win32 adapters, execute lifecycle tests, exercise installed shortcuts, Unicode and long paths, unrelated working directories, clean user profiles, display scaling and multi-window operation. Establish a release acceptance matrix rather than relying on aggregate source-test counts.

**Architecture and tools:** Add a native acceptance recorder that records build identity, enabled capability, test outcome and platform. Do not convert unexecuted cases into passes. Correct shared lifetime and resource-path defects in Framework; keep application adapters thin.

**Learning project:** Take one application from a fresh Windows checkout to an installation on a machine without developer PATH entries, then deliberately remove a resource and identify the failure.

**Acceptance gate:** Real installed-product journeys pass on the supported Windows baseline. A failed platform prerequisite is fixed earlier whenever another batch needs it.

## 44 — Framework Lifetime, Cancellation and Concurrency

**Products:** Framework and all service consumers.

**Planned work:** Audit worker start/join/detach semantics, cancellation propagation, callback re-entry, borrowed snapshots, server shutdown, lock ordering and asynchronous completion after a view closes. Add deterministic stress cases and documented ownership for each affected service.

**Architecture and tools:** Preserve stable public names while adding checked lifetime contracts where the earlier API cannot report a failure. Master Controller shutdown stops requests, drains or cancels workers, joins them and only then destroys dependent Slave Controllers. Timeouts do not imply that a worker was destroyed safely.

**Learning project:** Index Notes documents in a background worker, cancel halfway, close the window and prove that every callback either completes against a live owner or is safely discarded.

**Acceptance gate:** Repeated construction/destruction, owner termination, error injection and available race/leak tooling pass for the declared configurations. Document unsupported concurrent operations explicitly.

## 45 — Studio Editing, Debugging and Version-Control Completion

**Products:** Framework and Studio.

**Planned work:** Deepen editor groups, undo/redo, autosave, recovery, source navigation and language-server actions. Qualify debugger breakpoints, call stacks, variables, memory, registers and disassembly. Connect two-way/three-way comparison, staging and history to actual Git results. Give every build/test record its own working directory, configuration and source identity.

**Architecture and tools:** Consolidate generic document, process, diagnostic and VCS behaviour in Framework. Preserve the earlier build review and exact test-selection work. Never use a current profile to invent provenance for an older result.

**Learning project:** Reproduce a Notes bounds error, stop in the debugger, repair it, run the exact failing test, compare the changes and commit on main.

**Acceptance gate:** Edit/build/test/debug/recover works through Studio, including cancellation and unsaved data. A parsed success string is not a substituted test result.

## 46 — Visual Designer Editing and Round-Trip Generation

**Products:** Framework, Studio and Education.

**Planned work:** Connect the component palette, tree, property editor, semantic layout, command bindings and isolated preview to the native exporter. Add reusable fragments, validation diagnostics with source locations, preview refresh and reviewed regeneration into separate candidate directories.

**Architecture and tools:** Keep the canonical declarative document authoritative. Do not create a parallel designer language to avoid fixing its typed-property or quotation rules. Stable identifiers link generated objects to edits. Handwritten C and comments are never silently overwritten by generation.

**Learning project:** Design Notes with a navigation panel and account-summary tab, export and run it, edit the design and merge the next generated candidate without losing a handwritten validation function.

**Acceptance gate:** Actual GUI design-to-C-to-running-application round trips pass. Invalid properties and missing actions are rejected visibly; preview reloading does not clear unrelated edits.

## 47 — Market Recorder, Historical Data and Feed Recovery

**Products:** Framework, Trader and research tools.

**Planned work:** Add durable, provenance-bearing market capture, indexed history queries, bounded ingestion queues and recorded provider generation boundaries. Expand correction/cancellation events, depth deltas, schema migration and gap recovery from an explicitly identified source. Retain both source and arrival timestamps.

**Architecture and tools:** Extend the existing market tape vocabulary and Data Server storage contracts rather than building a recorder inside charts. A restored transport connection does not by itself restore the missing history. Add a dataset inspector for sequence, duplicates, corrections and retention limits.

**Learning project:** Record a permitted practice stream, interrupt capture, inspect the gap, import the missing authorised segment and replay the exact selected dataset.

**Acceptance gate:** Recovered sequence identity, correction handling, crash recovery and bounded memory are tested. Licensing and publication rights for external data are reviewed separately from file readability.

## 48 — Multi-Instrument Research and Reproducible Experiments

**Products:** Framework, Trader and Studio.

**Planned work:** Extend the current single-instrument replay with an explicit multi-stream event order, richer fill-model interfaces, partial fills, rejected orders, retained run manifests and checkpoint/seek rules. Add parameter grids, train/validation/test partitions, walk-forward scheduling and run comparisons that include unfinished positions and costs.

**Architecture and tools:** Reuse canonical replay, market and backtest services. Strategy binaries, configuration and dataset hashes belong in the run record. Untrusted strategy execution moves behind a separately qualified process boundary; a native callback must never be described as sandboxed.

**Learning project:** Compare two C strategies on the same frozen data, change fees and latency, then demonstrate why a favourable training result need not survive an unseen interval.

**Acceptance gate:** Repeatability, prefix/no-look-ahead tests, resource budgets and independently checked accounting hold. Optimisation output is research evidence, not a prediction of profit.

## 49 — Identity, Secrets, Approval and Package Trust

**Products:** Framework, Security Centre, Setup Centre and consequential-action consumers.

**Planned work:** Extend authenticated sessions, revocation, capability grants, secret-provider integration, signed release metadata, trust-store rotation and approval expiry. Bind high-impact approvals to exact reviewed inputs and principal identity, including file installation, provider execution and candidate-code activation.

**Architecture and tools:** Keep cryptographic/provider details behind supported C interfaces. Audit records identify the actor, operation, inputs, decision and outcome. No model, widget or plug-in gains authority merely because it can call a function. Add a read-only explanation view for denied actions.

**Learning project:** Approve a fictional transfer, revoke the approver session before execution and observe the refusal; verify a signed test package and reject a modified payload.

**Acceptance gate:** Tampering, expired sessions, confused-deputy cases and key rollover are tested. Required controls for Batches 35–40 are delivered there first; this is consolidation and expansion, not a deferred safety permission.

## 50 — Engine, Data and Risk Server Process Boundaries

**Products:** Framework, Operations, TMS, Trader and Bank.

**Planned work:** Qualify selected services out of process: discovery, transport, request identities, bounded messages, deadlines, health, reconnection and fenced ownership. Add durable inbox/outbox delivery and inspectable retry/dead-letter handling for a complete trade-processing flow.

**Architecture and tools:** One logical authority remains responsible for each mutable state. Moving Data Server to another process must not give clients direct database access. Master Controller supervision starts dependencies in order, distinguishes unavailable services from rejected business requests, and shuts down deliberately.

**Learning project:** Run separate Data, Engine and Risk processes, submit a fictional trade, terminate an engine and recover processing without duplicate settlement or accounting.

**Acceptance gate:** Actual multi-process tests establish recovery and idempotency. In-process mocks are retained for unit tests but do not count as distributed-system qualification.

## 51 — Installer, Runtime Bundles and VM User Experience

**Products:** Framework, Setup Centre, VM Manager and Desk.

**Planned work:** Expand native release planning, component ownership, installation maintenance, registration, shortcut repair/removal and signed offline releases. Qualify a selected QEMU runtime bundle with its supporting files and distribution materials. Improve path selection, profile discovery, console presentation, graceful shutdown and disk-checkpoint inspection.

**Architecture and tools:** Keep installation transactions distinct from personal-data migrations. Disk checkpoints are not full machine snapshots. Use private VM control channels and explicit host-sharing opt-ins. Package-inspection results remain separate from actual application/guest startup evidence.

**Learning project:** Install two apps and a qualified VM component, repair one runtime file, boot the selected guest, shut down and selectively remove an app without deleting the other app's shared files.

**Acceptance gate:** Clean Windows install/update/repair/removal and real QEMU workflows pass. An unqualified redistributable is not silently bundled.

## 52 — Graphical Umicom OS and Persistent User Sessions

**Products:** umicomOS, Framework, Desk and OS Control Centre.

**Planned work:** Build and boot a qualified graphical Linux profile with Desk inside an established display stack, persistent user storage, session handling, display/input configuration and controlled system-status tools. Add a VM-owned installation experiment, recovery entry point and image-based update/previous-generation selection.

**Architecture and tools:** Kernel and minimal recovery stay independent of Framework. Desk remains the shell, not a new compositor or unrestricted root service. Privileged operations pass through narrow reviewed OS adapters. Filesystem persistence, application checkpoints and OS rollback remain different mechanisms.

**Learning project:** Boot the guest, save a Notes document, restart, recover the document and practise recovering from a deliberately rejected system update.

**Acceptance gate:** Actual graphical guest and persistent-volume tests pass for each claimed architecture. RISC-V graphical support and physical-machine installation require their own qualification.

## 53 — Physical USB and Optical-Media Qualification

**Products:** Framework boot-media services, Setup Centre and umicomOS.

**Planned work:** Complete graphical discovery, stable device identity, system-disk exclusion, capacity/media checks, narrow privilege elevation, explicit erase review, exclusive acquisition, write/flush progress, read-back verification and safe eject. Qualify CD/DVD preparation and burning separately from USB imaging.

**Architecture and tools:** Separate non-destructive inspection from the privileged writer. Bind approval to the image and device, then recheck both immediately before writing. Refuse ambiguity, replacement devices and unknown ownership. Do not disguise a failed query as permission to write.

**Learning project:** Begin with a disposable virtual disk, then an explicitly identified spare physical device; inspect the resulting image and test the supported firmware boot route.

**Acceptance gate:** Wrong-device refusal, surprise removal, capacity failure, interruption and read-back checks pass on real supported hardware. An interrupted raw write can leave a device unbootable; no automatic reversibility is promised.

## 54 — Integrated Release Candidate and Public Capstones

**Products:** Framework, Studio, Trader, Desk, Bank, TMS, Setup Centre, Education and the qualified OS profile.

**Planned work:** Freeze a named release candidate, publish its capability matrix, dependency inventory, build identities, known limitations and user journeys. Run upgrade/recovery and cross-application context tests. Package the offline learning library with version-matched complete examples and accessible troubleshooting.

**Architecture and tools:** This is an integration and qualification milestone, not another unrelated feature engine. Remove no obsolete source merely to reduce the release diff. Classify every capability as source-only, target-tested, integrated or release-qualified with the evidence that supports the label.

**Learning capstones:** Build and package Notes in Studio; replay a research dataset in Trader; complete a fictional TMS/Bank/ledger reconciliation; and boot the qualified OS without touching a host disk.

**Acceptance gate:** Reproduce the release on a clean supported host and complete the published journeys. Remaining risks are explicit release limitations, not hidden behind a large unit-test count.

## Managing the sequence

The practical chains are **35 → 36 → 37 → 38** for financial workflows; **39 → 40 → 42** for governed information and learning; **43 → 44 → 45 → 46** for integrated developer tooling; **47 → 48** for recorded research; and **49 → 50 → 51 → 52 → 53 → 54** for trust, deployment and qualification. These are dependency guides, not permission to delay a required security or ownership fix.

A public guide is revised when its executable example changes. A roadmap entry is not marked complete because a focused subset passed. Target compilation, actual interface use, failure recovery and installed-product behaviour remain separate acceptance steps throughout.
