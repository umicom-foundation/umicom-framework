# Umicom Framework

Umicom Framework is the reusable C23 application foundation for Umicom Studio
IDE, Umicom Designer, Umicom Trader, Umicom Treasury Management System, Yuma
Media Studio and future Umicom applications.

## Major foundation 0.9.0

This release candidate consolidates the reusable platform instead of publishing
one small release per helper or header.  It provides:

- stable C ABI version 2;
- Master Controller and Slave Controller lifecycle;
- capability registry and canonical capability catalogue;
- application and suite manifests;
- diagnostics, retained logging, typed settings, messaging and Data Server
  foundations;
- portable filesystem and child-process services;
- capability policy and secret-provider foundations;
- native compiler, build-tool, SDK and library discovery;
- compile-link-run validation and isolated child environments;
- CMake cache repair and local user-preset generation;
- native configure, build, test, run and prepared-shell operations;
- local Git and optional GitHub repository creation;
- complete console, GTK4 and web application repository scaffolding;
- a native `umicom` command;
- independent unit, contract, lifecycle, repository and generated-application
  tests.

## Native command

```text
umicom check
umicom check --all --project "C:/umicom/umicom-applications"
umicom env
umicom repair --dry-run
umicom shell
umicom configure
umicom build
umicom test
umicom install
umicom package
umicom make
umicom automate plan "C:/umicom/umicom-applications"
umicom automate run "C:/umicom/umicom-applications" --preset windows-ucrt64-debug
umicom automate watch "C:/umicom/umicom-applications" --preset windows-ucrt64-debug
umicom automate settings "C:/umicom/umicom-applications"
umicom automate trigger "C:/umicom/umicom-applications"
umicom run desk
umicom run studio
umicom create repo "Umicom Designer" --console --gtk
umicom repo clone URL DESTINATION --root PATH
umicom repo init PATH
umicom repo submodule add URL PATH --root PARENT
umicom repo update PATH
umicom repo publish PATH --message "feat(scope): explain the change"
umicom quality scan PATH --profile ci
umicom memory scan PATH
umicom dependencies inventory PATH
umicom dependencies audit PATH --strict
umicom workflow plan --source PATH --preset windows-ucrt64-debug
umicom workflow build --source PATH --preset windows-ucrt64-debug --jobs 2
umicom capabilities
umicom suite
```

The native command constructs environments for its child processes.  Normal
Framework development therefore does not require an unsigned PowerShell script
to modify the current shell.

See [docs/UMICOM_COMMAND_GUIDE.md](docs/UMICOM_COMMAND_GUIDE.md) for a
beginner-friendly explanation of environment checks, repository creation,
submodules, safe publishing and the remaining role of the Windows bootstrap.

Inspect the shared panel and window plans after building Framework tools:

```text
umicom-application-presentation validate
umicom-application-presentation list "org.umicom.studio"
umicom-application-presentation show "org.umicom.workspace.studio.standard"
umicom-application-surface "org.umicom.workspace.studio.standard"
umicom-application-surface "org.umicom.workspace.trader.standard"
umicom-application-runtime-policy "umicom.development.editor"
umicom-application-runtime-policy "org.umicom.workspace.trader.standard"
```

Read the [Application Presentation Platform](docs/APPLICATION_PRESENTATION_PLATFORM.md)
for a beginner-friendly explanation, or use the
[quick reference](docs/APPLICATION_PRESENTATION_QUICK_REFERENCE.md) while coding.
The [Application Surface Runtime](docs/APPLICATION_SURFACE_RUNTIME.md) explains
how a validated recipe becomes a live, testable panel session.
The [shared product surface guide](docs/APPLICATION_PRODUCT_SURFACE.md) explains
how every product selects learning, standard or focus layouts without copying
the host and lifecycle code.
The [Runtime Behavior and Workspace Policies](docs/APPLICATION_RUNTIME_BEHAVIOR_AND_WORKSPACE_POLICIES.md)
guide explains refresh timing, safe commands, shared context, background work
and checkpoints in beginner-friendly terms.

Public SDK headers are also checked as one governed contract surface. The
[Public Header Governance](docs/PUBLIC_HEADER_GOVERNANCE.md) guide explains
include guards, include-order independence and the required human-readable
file comment.
The [source file governance guide](docs/SOURCE_FILE_GOVERNANCE.md) explains the
same human-readable convention for implementation files and the static check
for missing catalogue declarations.
The [workspace panel composition guide](docs/WORKSPACE_PANEL_COMPOSITION.md)
explains placement, tab stacks, linked contexts, reusable panel actions and
saved-layout compatibility in beginner-friendly language.
The [AI assistant and multi-model workspace guide](docs/AI_ASSISTANT_AND_MULTI_MODEL_WORKSPACES.md)
explains chat, approved agent tasks, local and online provider boundaries,
retrieval, reusable windows and safe side-by-side model comparison.
The [application identity and chrome guide](docs/APPLICATION_IDENTITY_AND_CHROME.md)
shows how a thin application receives accessible native text, a contrast-aware
SVG mark and an active-layout subtitle from shared Framework components.
The [responsive command centre guide](docs/RESPONSIVE_COMMAND_CENTRE.md)
explains how applications publish searchable commands, windows and layouts
through one portable model and a compact native renderer.

## Architecture

Framework public interfaces are under `include/umicom`.  Implementations are
under `src`.  Optional product policy and GTK4 widgets remain outside Framework
Core.  Generated repositories consume Framework through a submodule, installed
package, or explicitly selected bundled copy.

## Release policy

Minor internal corrections remain local.  Version, commit, push, and tag occur
only after the complete major feature train passes all acceptance gates.

## Author and organisation

- Author: Sammy Hegab
- Organisation: Umicom Foundation
- Licence: MIT
