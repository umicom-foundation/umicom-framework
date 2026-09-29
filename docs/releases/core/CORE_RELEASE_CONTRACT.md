# Umicom Framework Core release contract

Status: **proposed for owner acceptance**. Active product: **Umicom Framework**. Active major batch: **FW-01**. This document starts the release contract; it does not declare a stable release or change the existing Framework version.

## The product we are finishing

The first Core release must let a developer install the SDK, build an independent native application, open its real graphical interface, save and recover its documents, supervise background work, package it and install it on a clean supported machine. The reference application is Umicom Notes. A tutorial, simulated process, source-level check or headless library test alone cannot satisfy this outcome.

Reusable behaviour belongs to Framework. The Master Controller owns the composition and orderly lifecycle; Slave Controllers own bounded modules and operations. Public C interfaces retain their existing names. C23 remains primary, with Assembly where justified and isolated C++ only where a real adapter requires it. Data Server remains the persistence authority. There is no new application feature campaign in FW-01.

## Proposed supported profiles

1. Windows 11 x86-64, MSYS2 UCRT64 development toolchain, GTK4 native adapter, Debug and Release. Exact tool and dependency versions must be recorded from the qualifying machine. The release package must not depend on the end user's MSYS2 installation, source tree or terminal PATH.
2. Debian 13 x86-64 is the proposed named Linux profile, with GCC and Clang native checks and an actual GTK4 desktop journey. This proposal reflects the available native build host, not a completed GUI qualification. Owner acceptance of the Linux profile remains required.

No claim is made for untested Windows SDK/compiler variants, macOS, ARM, RISC-V or a bootable Umicom OS image. Existing implementations are preserved; additional platforms remain visible in the portfolio backlog rather than being deleted. A profile cannot quietly be removed to make this contract pass.

## Core scope

| Core area | Release obligation | Lead batch |
|---|---|---|
| Build and public ABI | Reproducible native builds, clean public headers, dependency ownership, independent consumers | FW-02–03 |
| Ownership and lifecycle | Allocation failure, tasks, threads, queues, cancellation, callback disposal and orderly shutdown | FW-04–05 |
| Native I/O | Unicode paths, process ownership, unrelated working directories and interrupted file operations | FW-06 |
| Data Server | Transactions, stale-writer detection, migrations, backup and demonstrated recovery | FW-07 |
| Communication | Typed commands/events, bounded queues, subscriber lifetimes and explicit backpressure | FW-08 |
| Security/configuration | Real authority boundaries, denied/revoked operations and protected secrets | FW-09 |
| GUI/component layer | Real GTK4 windows, actions, menus, focus, dialogs and an enumerated component catalogue | FW-10–11 |
| Workspaces/documents | Independent context, layout restoration, editing, save/conflict/undo/recovery | FW-12–13 |
| Presentation | Bounded grids and charts; unavailable or invalid values are not successful zeroes | FW-14 |
| Composition/providers | Declarative generation, explicit extension contracts and contained optional failures | FW-15–16 |
| Developer distribution | Installed public SDK, complete application templates, runtime, installer and maintenance | FW-17–18 |
| Acceptance | Native-host evidence, installed user journeys, documentation and owner acceptance | FW-19–20 |

## Preservation and maturity

Financial products, trading, AI, media, education, OS tooling and Umicoin are not certified by the first Core release. Their code and features remain present. Required shared dependencies discovered in those modules must be accounted for; this proposal is not approval to silently exclude them or hide failing regressions. Already existing all-module builds remain compatibility checks, not proof that every application is stable.

The current inventory distinguishes: source observed; focused execution observed; not yet assessed; and release-qualified. Nothing in checkpoint 1 is marked release-qualified. Absence from the selected dependency subset is not evidence of absence from the repository. Neither public declarations nor `-- enabled` configuration messages demonstrate user-visible behaviour.

## Mandatory acceptance record

`CORE_REQUIREMENTS.tsv` names 47 mandatory evidence slots and assigns each to its lead Framework batch. It is executable by the native release-baseline tool. `PENDING_EVIDENCE.tsv` deliberately contains no product-release receipts. The new parser's own regression tests are not substituted for the full Core suite.

All of the accepted scope must pass: actual Debug and Release builds; complete mandatory test inventories; installed Notes journeys; ownership/security/data recovery; package identity; licence review; accessibility; and truthful beginner documentation. Missing, skipped, Not Run, failed or mismatched receipts block review. Any test-set change requires explicit review, not a reduced denominator.

The initial real-use proposal is ten sessions over at least five working days, including one eight-hour workload session. These are proposed thresholds, not evidence of endurance testing. Exact performance budgets and environments must be agreed before FW-01 closes.

## Who accepts what

The automated checker validates the consistency of supplied assertions. It does not authenticate them, parse CTest/JUnit logs, verify referenced digests, check Git state or approve a release. An actor can fabricate plausible metadata; therefore the output is never authorisation to publish. Actual logs, package hashes, signatures, test inventory and user outcomes must be independently inspected.

Sammy's approval of the product-focused programme authorises beginning FW-01. Acceptance of this finite contract, the proposed Linux profile and the final release remain separate decisions. No acceptance signature or date is filled in on the owner's behalf.

## Stop condition

FW-01 stays active until its five checkpoints are integrated, its source/test inventory is complete enough to assign release-critical work, the required host-access gaps are explicit, and the owner accepts the scope. FW-02 does not begin merely because checkpoint 1 was published. OS, Studio, Trader and the other product campaigns remain queued.
