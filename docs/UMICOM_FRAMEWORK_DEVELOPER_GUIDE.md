<!--
Umicom Framework
File: docs/UMICOM_FRAMEWORK_DEVELOPER_GUIDE.md

PURPOSE:
Introduce C and Umicom Framework to new developers, map the reusable platform,
and explain how thin applications select components, panels and layouts.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Umicom Framework Developer Guide

## Who this guide is for

This guide is written for someone who may be learning C for the first time. It
also helps experienced contributors answer three questions before writing code:

1. Does Framework already provide the feature I need?
2. Is the new code reusable, or does it belong only to one application?
3. Which public contract, component, panel or layout should the application use?

Umicom Framework is the box of reusable building blocks. An Umicom application
selects those blocks, supplies its specialist rules and arranges its workspace.
An application should not copy Framework source code.

## The current codebase

The current Applications checkout contains more than twenty thousand C source
and header files. Framework has thousands of public headers and many catalogues.
For that reason, this guide explains the map and the design rules while the
generated reference lists the individual engines, components, panels, layouts
and feature states directly from running Framework code.

Generate the detailed reference after building:

```powershell
Set-Location "C:\umicom\umicom-applications"

& ".\build\windows-ucrt64-debug\bin\umicom-developer-reference.exe" |
    Set-Content -Encoding utf8 ".\build\windows-ucrt64-debug\UMICOM_DEVELOPER_REFERENCE.md"
```

Generated output is disposable evidence. Do not edit it by hand. Change the
Framework catalogue and generate it again.

### Finding every public C function

The authoritative method index is the public header tree. Function names begin
with `umi_`, and the directory after `include/umicom` identifies the owning
module. This command lists declarations with their source line so a learner can
jump directly to the ownership and argument comments:

```powershell
Set-Location "C:\umicom\umicom-applications"

rg -n "umi_[A-Za-z0-9_]+[ ]*\(" ".\framework\include\umicom" |
    Set-Content -Encoding utf8 ".\build\windows-ucrt64-debug\UMICOM_PUBLIC_FUNCTION_INDEX.txt"
```

The text index is generated evidence and should not be committed. Public
structures and their attributes live beside those declarations in the same
headers. This arrangement keeps the function signature, field ownership and
beginner explanation together instead of maintaining a second stale copy.

## Five C ideas to learn first

### 1. A declaration tells other files that something exists

A public header contains declarations. An implementation source file contains
the executable logic.

```c
/* The header promises that this function is available to callers. */
UmiStatus umi_example_open(const UmiExampleRequest *request);
```

Applications include public headers under `include/umicom`. They never include
Framework implementation files from `src`.

### 2. A pointer refers to an object

`const UmiExample *example` means the function receives a pointer and promises
not to modify the object through that pointer. The contract must explain whether
the pointer is borrowed, copied, transferred or owned by the caller.

### 3. A status describes success or failure

Umicom functions normally return `UmiStatus`.

```c
UmiStatus status = umi_example_open(&request);

/* Stop this workflow when opening failed; the caller decides how to display it. */
if (status != UMI_STATUS_OK) {
    return status;
}
```

Do not ignore a status unless the comment explains why failure is harmless.

### 4. Capacity must be checked before writing

Fixed arrays are common in public contracts because their ownership is simple.
The implementation checks the available capacity before copying or appending.
Truncating an identifier or filesystem path is normally an error.

### 5. Cleanup belongs to the owner

If a Framework `create` function returns an owned object, its matching `destroy`
function releases it. Memory should be allocated and released by the same module
unless an allocator contract explicitly permits transfer.

## Public ABI rules

- New fields are appended to published structures; they are not inserted among
  existing fields.
- Extensible public structures carry a size or API version where appropriate.
- Public C headers do not expose toolkit, database-vendor, model-provider or C++
  implementation types.
- Callbacks state the lifetime of their context and input values.
- A borrowed catalogue descriptor is read-only and must not be freed.
- A snapshot is a copy suitable for display, logging or evidence.

## Framework module map

### Foundations

| Area | Purpose | Typical public roots |
|---|---|---|
| Base | Status values, results, memory and common primitives | `umicom/base` |
| Diagnostics | Logs, findings, stores and diagnostic hubs | `umicom/diagnostics` |
| Messaging | Commands, queries, events and journals | `umicom/messaging` |
| Platform | Files, processes, configuration, clocks and operating-system adapters | `umicom/platform` |
| Runtime | Modules, capabilities, controllers, scheduling and lifecycle | `umicom/runtime` |
| Security | Policy, permissions, secret references, redaction and evidence | `umicom/security` |
| Resilience | Retry, circuit, health and failure-recovery policy | `umicom/resilience` |
| Observability | Metrics, traces, operations and health views | `umicom/observability` |

### Application composition

| Area | Purpose | Typical public roots |
|---|---|---|
| Application | Experiences, components, recipes, journeys and runtime catalogues | `umicom/application` |
| Application shell | Shared startup and application shell behaviour | `umicom/application_shell` |
| Application UI | Toolkit-neutral application views | `umicom/application_ui` |
| Desktop | Windows, workspaces, monitors and shell coordination | `umicom/desktop` |
| Product | Product metadata, editions and capability selection | `umicom/product` |
| Distribution | Installation, packages, updates and delivery evidence | `umicom/distribution` |

### Workspaces and user interfaces

| Area | Purpose | Typical public roots |
|---|---|---|
| UI | Views, actions, values, layout models and adapters | `umicom/ui` |
| Workbench layout | Docking, tabs, splits, floating windows and restoration | `umicom/workbench_layout` |
| Workbench designer | Palette, canvas, hierarchy and properties | `umicom/workbench_designer` |
| Context channels | Typed links between panels and applications | `umicom/context_channel` |
| Selection providers | Publish active selections without coupling panels | `umicom/workbench_selection_provider` |
| Cross-application panels | Share one panel model between running applications | `umicom/cross_application_panel` |

### Developer platform

| Area | Purpose | Typical public roots |
|---|---|---|
| Documents | Identity, loading, encoding, editing and generation | `umicom/document` |
| Editor | Buffers, selections, navigation and editing services | `umicom/editor` |
| Language | Tokens, diagnostics, completion, navigation and refactoring | `umicom/language` |
| Project | Project models, files, dependencies and workspaces | `umicom/project` |
| Build and compiler | Build graphs, providers, tasks and diagnostics | `umicom/build`, `umicom/compiler` |
| Debug | Sessions, breakpoints, stacks, variables and adapters | `umicom/debug` |
| Test platform | Discovery, execution, results, coverage and history | `umicom/test_platform` |
| Terminal | Processes, sessions, buffers and terminal views | `umicom/terminal` |
| Source control | Status, staging, commits, history, branches and diffs | `umicom/source_control` |
| Plug-ins and SDK | Extensions, capabilities, packaging and compatibility | `umicom/plugin`, `umicom/sdk` |

### Data, AI and reusable engines

| Area | Purpose | Typical public roots |
|---|---|---|
| Data | Repositories, transactions, migrations and connector adapters | `umicom/data` |
| Integration and web | External services, protocols and provider adapters | `umicom/integration`, `umicom/web` |
| AI | Model runtimes, conversations, tools, approvals and local/remote providers | `umicom/ai` |
| Knowledge | Sources, ingestion, retrieval, provenance and citations | `umicom/knowledge` |
| Chart and analytics | Series, indicators, drawings, views and analysis | `umicom/chart` |
| Finance and trading | Money, portfolios, orders, risk, matching and execution | `umicom/finance`, `umicom/trading` |
| Games | Stable-ID worlds and deterministic sessions | `umicom/game` |
| 3D | Scene nodes, transforms and renderer-neutral resources | `umicom/render3d` |
| Media | Images, animation and future audio/video adapters | `umicom/media` |
| Social | Communities, membership, posts and moderation | `umicom/social` |

## Engines, components, panels and layouts

An engine owns domain behaviour. A component presents or edits part of that
behaviour. A panel places a component in an application workspace. A layout is
only a starting arrangement of panels.

```text
engine → component/view model → panel → customisable layout → UI adapter
```

Users may dock, split, tab, float, auto-hide and move compatible panels across
monitors. The application experience records which panels are available and
which layout is selected initially. It must not prevent the user from creating
another arrangement.

### Reading the engine catalogue

```c
size_t index;

/* Borrow each descriptor; Framework owns it for the life of the process. */
for (index = 0U; index < umi_engine_catalogue_count(); ++index) {
    const UmiEngineDescriptor *engine = umi_engine_catalogue_at(index);

    /* A missing descriptor means the catalogue cannot provide this position. */
    if (engine == NULL) {
        continue;
    }

    printf("%s: %s\n", engine->display_name,
           umi_engine_maturity_text(engine->maturity));
}
```

### Finding an application experience

```c
const UmiApplicationExperienceDefinition *experience =
    umi_application_experience_catalogue_find("org.umicom.studio");

/* Do not read panels or layouts when the requested application is unknown. */
if (experience == NULL) {
    return UMI_STATUS_NOT_FOUND;
}
```

## Application catalogue and remaining work

The feature state in the generated reference is authoritative. The table below
explains the main unfinished user journeys; it must not be read as a claim that
every listed foundation is production-ready.

| Application | Existing direction | Major completion work |
|---|---|---|
| Studio IDE | Editor, project, build, test, debug, source control, designer, AI, source-guidance review and workbench foundations | Finish native docking/floating, workspace guidance dashboard, remote workflows, profiler capture, database tools, notebooks, collaboration, localisation, hot reload and accessibility evidence |
| Trader | Market data, orders, matching, risk, portfolio, charts, layouts and simulation | Complete broker sessions/reconciliation, time and sales, scanners, alerts, options, ladder, strategy analysis, order flow and performance reports |
| Bank | Account, wallet and payment workspace foundations | Complete identity, onboarding, beneficiaries, transfers, cards, FX, statements, disputes, limits, notifications and regulated audit workflows |
| TMS | Treasury application surfaces and Framework finance/trading services | Complete trade capture, lifecycle events, pricing, positions, limits, collateral, settlement, reconciliation and operations queues |
| LLM | Model workspace and provider-neutral AI foundations | Complete model installation, runtime supervision, evaluation, comparison, usage, fine-tuning adapters and safe tool execution |
| RAG | Knowledge and ingestion workspace foundations | Complete connector ingestion, chunk inspection, hybrid search, reranking, evaluation, citations and collection permissions |
| Desktop | Launcher and desktop shell foundations | Complete sessions, task surfaces, notifications, search, settings, recovery, multiple monitors and application communication |
| OS Control Centre | User-space system control surfaces | Complete process, service, storage, network, update, log and privilege adapters without moving kernel logic into applications |
| Accountant | Accounting workspace foundations | Complete ledgers, invoices, purchases, reconciliation, tax, payroll, HR, reporting and period close |
| Exchange | Trading and marketplace foundations | Complete membership, instruments, matching operations, surveillance, clearing, settlement and market administration |
| Music | Media workspace foundations | Complete arrangement, clips, MIDI, mixer, automation, instruments, recording, stems, mastering and render jobs |
| Media | Media production foundations | Complete timeline, storyboard, compositing, subtitles, review, versions, render queue and delivery |
| Games | Game world, session, 3D and media foundations | Complete components, input, collision, spatial queries, navigation, audio, assets, scripting, profiling and packaging |
| CAD | Designer and 3D foundations | Complete sketches, constraints, parametric history, layers, assemblies, precision tools and interchange adapters |
| Kitchen | Designer and 3D foundations | Complete floor plans, catalogues, constraints, costing, documentation and previews |
| Creator | AI and media creation foundations | Complete generation graphs, variants, review, provenance, assets and export |
| Education | Lesson and resource foundations | Complete course authoring, exercises, assessment, learner progress, teacher views and accessibility |
| Database Studio | Data and editor foundations | Complete drivers, schema browsing, query results, safe data editing, plans, migrations and monitoring |
| Integration Studio | Integration and designer foundations | Complete connectors, mappings, transformations, testing, schedules, deployment and operations |
| Security Centre | Security and evidence foundations | Complete identity posture, secrets, vulnerabilities, policy, incidents and remediation workflows |
| Operations | Observability and operations foundations | Complete services, jobs, queues, incidents, runbooks, logs, metrics and deployment state |
| Marketplace | Product and distribution foundations | Complete discovery, signatures, permissions, installation, updates, rollback, reviews and publisher tools |
| Web Studio | Designer, browser and language foundations | Complete responsive design, component editing, preview, diagnostics, accessibility and deployment |
| Mobile Studio | Designer and device foundations | Complete navigation flows, device previews, bridges, tests, packaging and store delivery adapters |
| Author | Documents, generation, knowledge and AI foundations | Complete outlines, chapter manifests, citations, revisions, comments, previews and publication jobs |

## Adding a reusable feature

1. Describe one complete user journey.
2. Search public headers and catalogues for an existing contract.
3. Put reusable state and policy in Framework.
4. Keep vendor, broker, model, device and regulated behaviour behind adapters.
5. Add a toolkit-neutral view model before native widgets.
6. Add the panel and command to the application experience.
7. Add a focused Framework test and one thin-application adoption test.
8. Explain ownership, failure and important decisions in comments.
9. Generate the developer reference and review the reported maturity.
10. Build and run the complete test suite before release.

## Documentation coverage

`umicom/codeguard/documentation_coverage.h` scans one C or header file without
modifying it. It reports:

- whether the standard file header exists;
- how many Umicom function candidates have nearby guidance;
- how many conditions, loops and switches have nearby explanations;
- separate function and decision percentages;
- whether the selected policy passes.

This is a review assistant, not a replacement for understanding C. Generated,
imported or compatibility code may use a reviewed policy exception. New Umicom
code should reach full function guidance and explain every meaningful decision.

## Learning path

1. Learn values, variables, conditions, loops and functions in C.
2. Learn pointers, `const`, arrays and structures.
3. Learn ownership and cleanup using one small Framework module.
4. Read `base/status.h` and follow one `UmiStatus` through a test.
5. Read an engine catalogue descriptor and print it in a console program.
6. Find one application experience and list its panels.
7. Add a focused test before changing behaviour.
8. Add one reusable capability and consume it from a thin application.

The aim is not to memorise thousands of function names. Learn how to find the
right module, read its ownership contract, check its maturity and use the
catalogue. That skill continues to work as Umicom Framework grows.
