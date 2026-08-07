# Current State — 0.4.0

## Verified in this release

- C23 Framework builds with Clang, CMake and Ninja.
- Five CTest tests pass: Framework, scaffold, Studio, Trader and TMS.
- Memory and SQLite Data Server adapters are implemented.
- Fake/system clocks, scheduler, append-only journal and plug-in loader are implemented.
- The C project generator and GTK4 wizard source are included.
- Studio console core has Workspace, Document, Output and Build Slave Controllers.
- Trader replays 30 sample NQ bars, calculates EMA, ATR and RSI, produces an initial Logger decision, publishes events and rejects live orders.
- TMS demonstrates a clean-room trade lifecycle, reference pricing engine, Data Server persistence and event publication.
- The Java 21 Eclipse TMS training companion compiles and runs.

## Not yet complete

- Studio: full editor, GtkSourceView, LSP, DAP, terminal, docking, visual designer, AI Workbench and installer.
- Trader: tick feed, complete Pine/Logger parity, chart renderer, market radar, OMS, Risk Server, full IBKR adapter, paper/live execution and reconciliation.
- TMS: complete reference data, products, cashflows, settlement, accounting, collateral, workflow, batch, risk and reporting servers.
- Framework: dependency graph ordering, durable distributed Event Server, full ORM, security/identity, IPC, plugin signatures, packaging and native LocalLLM implementation.

## Immediate next milestone

Freeze this release in Git, reproduce the Windows UCRT64 GTK4 build, launch the Application Wizard, and migrate the first real Studio logging/configuration vertical slice into Framework.
