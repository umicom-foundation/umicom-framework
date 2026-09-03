<!--
Umicom Framework
File: docs/INTEGRATED_DEVELOPMENT_ENVIRONMENT_PLATFORM.md

PURPOSE:
Describe the reusable Framework capabilities required by a complete integrated
development environment and record their implementation state.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Integrated Development Environment Platform

Umicom Framework provides the reusable engines, contracts and semantic models
used to build development tools. Umicom Studio IDE composes those capabilities
into a product. A menu, panel or button in Studio should call a Framework or
Studio service; it must not exist only as decoration.

This inventory describes familiar functionality found in professional
development environments without copying another product's branding, wording
or implementation.

## Product shell and workspace

| Capability | Framework responsibility | Current state |
|---|---|---|
| Startup and welcome centre | Recent work, project creation, learning and extension entry points | Implemented and available to Studio |
| Menu and command system | Stable command identifiers, enabled state, search and keyboard routing | Implemented |
| Toolbars and quick access | Command-backed actions with compact presentation metadata | Implemented |
| Dockable windows | Open, hide, move, group, float and redock reusable surfaces | Implemented |
| Named layouts | Task-specific visible surfaces, sidebars, bottom panel and focus | Implemented |
| Layout editing | Explicit lock, reversible edit session, splitter sizes and persistence | Implemented |
| Multi-monitor windows | Native floating windows and remembered semantic placement | Partly implemented; monitor recovery remains planned |
| Appearance | Themes, scale, typography and contrast-aware identity | Implemented at platform level; individual editors need more preference pages |
| Command search | Search menus and registered commands from one catalogue | Implemented |
| Accessibility | Keyboard operation, labels, high contrast and scalable assets | Foundation exists; complete screen-reader audit remains planned |

## Project and source development

| Capability | Expected behavior | Current state |
|---|---|---|
| Project creation | Templates, language, platform and build choices | Implemented foundation; template breadth is expanding |
| Project manager | Files, resources, dependencies, targets and configurations | Implemented foundation |
| Multi-root workspace | Open and coordinate several related source roots | Implemented |
| Code editor | Tabs, dirty state, syntax services, find, replace and line navigation | Implemented foundation |
| Active-document preview | Revision-aware HTML and plain-text preview beside the editor | Implemented with provider and execution-safety contracts |
| Editor groups | Split, move, balance, pin and focus document groups | Implemented |
| Language intelligence | Completion, symbols, diagnostics, hover and navigation | Implemented service contracts; language coverage is expanding |
| Search and navigation | File, symbol, reference, bookmark and command navigation | Implemented foundation |
| Resource editing | Text and semantic document models for non-code resources | Partly implemented |
| Source control | Status, history, branch, diff, stage, commit, pull and push | Implemented service surface |

## Visual application design

The visual designer uses one authoritative declarative document. The canvas,
component catalogue, structure tree and property inspector receive the same
document revision and selection through `UmiDesignerWorkspaceModel`. This
prevents one panel from displaying stale data after another panel changes it.

The current reusable slice provides:

- a searchable, categorized component catalogue;
- safe insertion below a selected container or the design root;
- generated unique component identifiers;
- a semantic component hierarchy with shared selection;
- a typed property schema and current property values;
- validated property edits recorded by designer history;
- undo and redo;
- semantic geometry used by a toolkit canvas;
- topmost component hit-testing with visible selection handles;
- pointer-driven move and edge/corner resize previews;
- grid snapping, canvas bounds and minimum component sizes;
- stale-gesture protection and one-step undoable geometry commits;
- separate Design, Code, Mixed and Preview workbench surfaces;
- debounced source updates that reject stale renderer results;
- last-known-good preview retention when edited source is invalid;
- source-family routing for declarative UI, C, HTML, JavaScript, CSS and scripts;
- dockable, grouped and floatable Studio surfaces;
- a named Visual Application Design layout.

Declarative source now completes the first end-to-end live update path in
Studio. Other languages use the same `UmiDesignerLiveSourceWorkspace` revision,
debounce and error-retention contract, while their compiler, browser or script
runtime remains a replaceable provider. Executable source must run only through
an approved sandbox provider; editing text never grants permission to run it.

Ordinary editor documents also use the separate
`UmiBrowserSourcePreviewService`. This service supplies readable HTML and
plain-text fallbacks, rejects stale provider results and requires both
workspace trust and explicit permission before an executable provider can be
selected. The Studio Live Preview panel and Code + Preview layout present this
Framework service without creating another rendering model.

Studio now performs direct manipulation and palette placement through a
toolkit-neutral Framework interaction session. A pointer press selects the
topmost semantic component,
movement changes only a preview rectangle, and release records one undoable
operation. Edge and corner handles resize safely, the component cannot shrink
below its minimum size, and grid snapping can be disabled when precise placement
is needed. A palette drop creates its semantic node and bounded geometry in one
history operation, so an unsuccessful drop cannot leave a partial component. If
another operation changes the same component during a gesture, the stale
gesture is rejected instead of overwriting newer work.

The next visual-design increments are peer alignment guides, multi-selection
alignment commands, event handler wiring,
data bindings, device-size preview, resource editors and form inheritance.
These must extend the same Framework document and history services rather than
introduce frontend-owned models.

## Build, test and debugging

| Capability | Expected behavior | Current state |
|---|---|---|
| Configure and build | Profiles, targets, compiler diagnostics and cancellation | Implemented foundation |
| Incremental automation | Discover changed modules, quality gate, test and build affected targets | Implemented foundation and configurable scheduler |
| Unit and user-interface tests | Discover, filter, run, stop, rerun and inspect output | Implemented foundation |
| Debug sessions | Launch or attach, continue, pause, stop and restart | Implemented foundation |
| Debug inspection | Threads, frames, variables, watches and breakpoints | Implemented surfaces |
| Low-level inspection | Registers, memory, disassembly and instruction stepping | Contracts exist; complete Studio presentation remains planned |
| Profiling | CPU, allocation, leak and performance analysis | Planned product integration |

## Data, services and deployment

The platform should let an application connect to relational and document data
stores, inspect schemas, design queries, consume structured web services and
generate typed access code. Connection secrets belong in secure storage and
must never be written into layouts or generated source by default.

Deployment must model platforms, architectures, artifacts, signing,
dependencies, remote hosts and packaging as explicit plans. The current
Framework includes database, web, integration, build, distribution and delivery
modules. Studio still needs a unified data explorer and a visual deployment
manager that compose those modules.

## Extensions and intelligent assistance

Extensions require discoverable manifests, compatibility checks, permissions,
lifecycle isolation, package installation and safe removal. Intelligent
assistance requires selectable local or online providers, explicit context,
reviewable edits, approval gates, tool permissions and traceable sessions.
Provider choice is a user setting; Studio must not hard-code one service.

The Framework already provides reflection, component catalogues, command
registries, tool discovery, multi-model assistance, review and approval
contracts. Remaining work includes a polished extension manager, stronger
sandboxing and richer visual authoring extensions.

## Recommended menu structure

Studio should keep a stable, predictable top-level structure:

- **File** — create, open, recent work, save, close, import, export and exit.
- **Edit** — undo, redo, clipboard, find, replace and preferences.
- **Search** — files, symbols, references, commands and navigation history.
- **View** — panels, editor groups, appearance, full screen and layouts.
- **Project** — files, dependencies, configuration, targets and properties.
- **Build** — configure, build, rebuild, clean, install and artifacts.
- **Run** — run, debug, attach, profiles and launch configuration.
- **Test** — discover, run, rerun, stop, coverage and results.
- **Source Control** — status, diff, stage, commit, branch, pull and push.
- **Design** — canvas, components, structure, properties, preview and bindings.
- **Tools** — command search, extensions, external tools and options.
- **Window** — new window, dock, float, group, lock, save and restore layout.
- **Help** — documentation, lessons, diagnostics, updates and product details.

Every visible action should have one stable command identifier so menus,
toolbars, key bindings, automation and assistants invoke the same behavior.

## Completion rule

A capability is complete only when its semantic service, command, presentation,
error handling, persistence and automated tests agree. A placeholder panel or
an enabled button without an operation does not count as implementation.
