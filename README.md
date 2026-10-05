# Umicom Framework

Umicom Framework is the reusable C23 application foundation for Umicom Studio
IDE, Umicom Designer, Umicom Trader, Umicom Treasury Management System, Umicom
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

## Guides for learners and contributors

[Working copies, search and safe document changes](docs/guides/WORKING_WITH_DOCUMENTS.html)
explains how applications use the shared document and editor services. Its
complete Notes search example builds against the installed Framework SDK.
[Writing useful documentation](docs/WRITING_DOCUMENTATION.md) gives contributors
practical guidance for public, beginner-friendly lessons and reference pages.

## Copy reports for review

Learn how to create bounded, owned tables and use the domain exporters in [Building an owned CSV report](docs/CSV_REPORTS.md).

- [Capture, persist and review chart documents](docs/CHART_CHECKPOINTS.md).

## Follow test failures into source

[Capture and open test source evidence](docs/TEST_SOURCE_LOCATIONS.md) explains source locations, retained runs and safe navigation.

## Review local practice charges

[Build a reviewed practice charge workflow](docs/learning/practice-charges.md) explains fixed amounts, separate approval, protected funds and retained reversals.

## Review retained trading sessions

[Capture and check a local trading session](docs/TRADING_SESSION_REVIEW.md) explains immutable evidence, canonical fill replay and separate currency totals.

## Calculate explicit payment fees

Learn [checked payment fee quotes](docs/PAYMENT_FEE_QUOTES.md), including exact rounding, caps, copied assumptions and native review.

- [Chart drawing undo and redo](docs/CHART_DRAWING_HISTORY.md)

- [Investigate accepted banking commands](docs/BANK_AUDIT_INVESTIGATION.md) links captured command history to its exact posting journals.

For debugger groups that should be loaded only on request, follow the
[scope inspection lesson](docs/learning/scope-inspection.md). It explains owned
captures, expensive scopes and recovery when the stopped context changes.

[Review and publish state safely](docs/REVIEWED_STATE_UPDATES.html) explains
independent captures, stale-edit refusal, complete collection replacement and
text edits that preserve the previous value when an update cannot be accepted.

[Review linked context changes](docs/guides/REVIEWING_LINKED_CONTEXTS.html) explains
copied proposals, stale-state checks and atomic publication across application
and UI context stores, with product adapter names and recovery guidance.

Workspace recovery: [Review and restore saved workspaces](docs/learning/restore-saved-workspaces.html). The guide explains the complete comparison, explicit confirmation and recovery limits.

For an explicit runtime edit while debugging, see [assigning captured variables](docs/learning/assigning-debugger-variables.html). The guide explains confirmation, adapter support and recovery after an uncertain reply.

[Read live build output](docs/learning/live-build-output.html) explains progress, paused inspection, retained history and output limits.

[Save complete emitted build output](docs/learning/saving-build-logs.html) explains file selection, capture status, retention and recovery.

## Keep provider credentials local

[Store provider keys on this computer](docs/learning/storing-provider-keys.html)
explains the shared native secret provider, local database references, ownership,
failure recovery and the distinction between saved credentials and remote requests.

[Save provider connection settings](docs/learning/saving-provider-connections.html)
explains durable metadata, conflict review and explicit credential acquisition.

[Edit provider connections in the native settings window](docs/learning/editing-provider-connections.html)
explains draft preservation, conflict review and the shared Studio entry point.

[Manage local provider keys](docs/learning/managing-local-provider-keys.html)
explains the profile password check, native key panel, local availability checks
and the separate steps for storing a key and saving its connection reference.

[Check a saved provider connection](docs/learning/checking-provider-connections.html)
explains the separate **Check saved connection** action in Connections. After
review and approval, it requests a model catalogue from the official OpenAI
endpoint or a supported local loopback server. Remote checks freshly verify
the local profile password before reading the stored key. A listed model does
not prove that chat or inference works, and this action does not change the
active AI provider. Editing and saving metadata still performs no sign-in.

[Chat with a saved connection](docs/learning/chat-with-saved-connections.html)
explains **Connections → Chat with selected saved connection…**. Enter a prompt
and optional context, review the exact outgoing message, then approve one send.
The shared Framework workflow uses the selected OpenAI or local connection and
displays plain text. It does not change the provider used by the existing chat,
agent or patch controls, and it cannot run tools or apply code changes.

For explicit editor context, see [Chat about selected code](docs/learning/chat-about-selected-code.html). Framework captures only the selected draft bytes and hands them to a reviewed request without retaining the editor.

For a longer discussion, see [Choose context for a chat follow-up](docs/learning/reviewed-chat-follow-ups.html). The shared window keeps up to eight exchanges locally in memory and copies only the excerpt you explicitly select into a newly reviewed request.

To turn a suggestion into a deliberate source edit, follow [Review a replacement for selected code](docs/learning/review-selected-code-replacements.html). The shared review captures the target, previews complete drafts and applies approved text through the document Undo owner; saving remains separate.

[Import a document for local source retrieval](docs/learning/import-searchable-documents.html) explains the shared UTF-8 file/text import, complete passage preview, atomic save and grounded-question workflow. Imports remain local until you separately approve a model request.

[Review changes to your source library](docs/learning/review-source-library-changes.html) explains explicit passage selection, complete replacement/removal previews, atomic application and retained evidence for earlier answers. RAG, LLM and Creator share this native workspace page.

[Choose project presets and a program working folder](docs/learning/project-stage-presets.html) explains separate configure, build and test selections, saved launch folders shared by Run and native Debug, and recovery from conflicting settings.

[Choose and review a program launch](docs/learning/review-launch-settings.html) explains selecting a local executable and working folder, reviewing exact argument values, preserving manual edits during a chooser, and making Run and native Debug executable lookup explicit. Review does not start a process or save settings.

[Discover configured build targets](docs/learning/discover-configured-targets.html) explains requesting CMake target metadata, reading the configured target list, selecting a build target or executable, and recovering from stale or mismatched build folders. Selection edits the form; applying settings and execution remain separate.
