# Umicom Architecture

## Product family

Umicom Framework is the shared C23 technical foundation. Umicom Studio IDE, Umicom Trader and Umicom TMS are independent applications assembled from Framework blocks. Compatible applications may also be composed into a suite and share one Framework runtime and authorised servers.

## Core decisions

- C23 is the primary language.
- Cross-module binary boundaries use a stable C ABI.
- The Master Controller supervises startup, lifecycle, health and shutdown.
- Slave Controllers own bounded product areas.
- Commands request state changes, queries request information and events record facts.
- The Data Server is the sole authority permitted to communicate with databases.
- GTK4 is the first desktop adapter; toolkit types do not cross domain boundaries.
- Core modules are statically registered first. Dynamic plug-ins are optional and validated before activation.
- Studio remains buildable while reusable functions are migrated into Framework one vertical slice at a time.
- Trader begins with deterministic journal, replay and strategy/logging parity
  before broker execution.
- Umicom TMS is an original clean-room platform that teaches familiar
  enterprise treasury-management concepts through Umicom contracts and APIs.

## Current 0.4.0 source

Framework includes status/result, memory, diagnostics, command/event/query buses, journal, memory/SQLite Data Server, filesystem, configuration, process, workspace, document, clock, scheduler, module registry, Master Controller, plug-in loader, scaffolding CLI and optional GTK4 wizard.

The source also includes runnable Studio, Trader and TMS foundations with automated tests.
