<!--
Umicom Framework
File: docs/APPLICATION_PRODUCTISATION_ARCHITECTURE.md

PURPOSE:
Explain how canonical application definitions become inventory, coverage,
readiness, guidance and controlled completion work.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Application Productisation Architecture

Productisation answers a practical question: can a declared application be
presented, launched, tested and accepted using real Framework components?

An application repo contributes only its identity and evidence that its
manifest, composition, executable and tests exist. Framework already owns the
application's features, panels, layouts, capabilities and reusable components.
Keeping those responsibilities separate prevents each application from
inventing a different definition of readiness.

## Main data flow

```text
Canonical application experiences
        |
        +----> portfolio inventory
        |
        +----> capability matrix ----> missing capability/component report
        |
        +----> surface projection ----> panel-to-component coverage
        |
Application contribution
        |
        +----> adoption snapshot ----> acceptance readiness
        |
        +----> workspace guide ------> layouts and next feature
        |
        +----> product session ------> live layout, panel and context commands
```

The experience catalogue is the source of truth. Tests and user interfaces
must derive counts from that catalogue instead of copying totals such as “six
layouts” or “175 features.” A copied total becomes wrong as soon as a valid
feature or layout is added. A catalogue-derived comparison still detects a
projection defect while allowing planned growth.

## Inventory and capability closure

The inventory walks every experience and counts its features, panels, layouts,
maturity states and owners. The capability matrix then records which
applications use each capability and whether Framework has registered it.

A panel needs two related registrations:

1. A capability explains what service or behaviour the panel requires.
2. A reusable component explains how that capability can be presented.

The capability matrix reports a missing capability when an experience names a
capability that is absent from the Framework catalogue. It reports a missing
component when a panel's capability exists but no reusable component presents
it. Both are real architecture gaps; a test must not hide them by accepting a
non-zero missing count.

## Surface projection

Surface projection resolves every application panel to a reusable component.
The result keeps the canonical panel title, starting region, context group,
flags, component role and component maturity. Thin applications use this
projection instead of maintaining private panel registries.

A complete projection means every panel is covered. It does not mean every
feature is finished. Feature maturity remains explicit in the experience and
may truthfully be planned or at foundation level.

## Adoption readiness

An adoption snapshot is accepted only when all of these statements are true:

- the application contribution is valid;
- its canonical experience exists;
- every canonical layout can be projected by the shared runtime;
- every panel resolves to a reusable component;
- the application manifest, composition and executable are present;
- application tests are present;
- the module is runnable.

These checks are combined with logical AND. A working executable does not
cancel out a missing component or missing tests.

## Product sessions

A product session joins the adoption snapshot to a Framework thin client. It
accepts a small set of commands for selecting layouts, showing or hiding
panels, changing shared context, locking layouts, refreshing readiness and
synchronising a workbench.

The session stores the application contribution as a borrowed pointer. The
caller must keep that contribution alive for the complete session. Snapshots
copy their text and counters into value storage, so panels and diagnostics can
read them without taking ownership of session internals.

## Controlled completion execution

The `execution` submodule turns a completion plan into bounded work items. Its
parts have distinct jobs:

- the work queue owns items created from completion-plan steps;
- the dependency graph prevents later stages from running too early;
- the scheduler chooses the next ready item deterministically;
- policy checks ownership, mutation and retry limits;
- host adapters perform owner-specific work outside the portable model;
- cancellation stops work cooperatively at a safe boundary;
- history records ordered state changes and evidence;
- checkpoints retain recovery positions;
- rollback plans reverse successful mutations after a failure;
- metrics and acceptance describe progress without changing it.

Execution success and product acceptance are deliberately separate. A command
may run successfully but still leave a blocker, missing delivery result or
unfinished item.

## Writing durable tests

Use `UMI_TEST_REQUIRE` in native tests. It returns a normal failure code and
prints the failed file, line and expression. A C `assert` may terminate through
a platform fast-fail code, which hides useful context in large test runs.

Compare derived data with its authoritative source:

```c
const UmiApplicationExperienceDefinition *experience =
    umi_application_experience_catalogue_find("org.umicom.studio");

UMI_TEST_REQUIRE(experience != NULL);
UMI_TEST_REQUIRE(snapshot.layout_count == experience->layout_count);
```

Keep fixed literals for true contract rules, such as maximum capacity or the
number of contributions created inside a small test fixture. Do not use fixed
literals for catalogue totals that are expected to grow.

## Documentation protection

Every public productisation function has a structured comment that appears in
the generated HTML reference. This source-only target verifies that coverage:

```powershell
& "C:\msys64\ucrt64\bin\cmake.exe" `
    --build `
    --preset windows-ucrt64-debug `
    --target umicom-productisation-documentation-audit
```

CTest also exposes the check as
`framework.application_productisation.documentation`. The audit reads headers
only and does not launch an application or change source files.
