<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/AI_ASSISTANT_AND_MULTI_MODEL_WORKSPACES.md

PURPOSE:
Explain the reusable chat, agent, local/remote model, RAG and multi-model
workspace foundation to new contributors and application developers.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
---------------------------------------------------------------------------- -->

# AI Assistant and Multi-Model Workspaces

An AI assistant is a helper that can answer a question, explain information or
prepare a task. A model is the engine that produces an answer. Some models run
on the user's computer. Other models run through an online service.

Umicom Framework gives every application the same safe building blocks. An
application chooses which blocks to show and which providers to configure. It
does not copy the common chat, approval, context or window-management logic.

## What Framework provides

Framework now provides reusable definitions for:

- assistant chat and conversation sessions;
- local, remote, process and test provider boundaries;
- a model/runtime catalogue with health information;
- a bounded multi-model request that can compare up to eight selected routes;
- repository-aware coding tasks and reviewable patches;
- tool registration, tool activity and explicit approvals;
- local knowledge collections, retrieval, citations and archive storage;
- privacy rules for remote sharing and conversation persistence;
- chat, agent-task, approval, tool-activity, model-comparison and research
  windows that any Framework application can add to its New Window catalogue;
- docking, floating, tab grouping, multi-monitor placement and saved layouts;
- linked context that lets an assistant understand the user's current window,
  document or selected business object without making the UI toolkit own that
  information.

## The simple mental model

```text
Application window
    -> asks Framework AI runtime
        -> applies privacy and approval policy
            -> calls one or more registered provider adapters
                -> stores separate results and statuses
                    -> projects data into reusable view models
```

The arrow order matters. A panel never contacts an online model directly. It
must use the runtime, because the runtime is where provider policy is checked.

## Asking several models

`UmiAiModelTarget` identifies a provider and a model. The target also records
whether the user approved a route that needs approval.

`umi_ai_model_ensemble_query()` copies the original request for every enabled
target, replaces only the model identifier, and then calls
`umi_ai_runtime_generate()`. This means the ordinary provider and remote-data
rules still apply.

Each attempt becomes a `UmiAiModelResult`. A blocked or unavailable provider is
kept beside successful answers. One failure therefore does not erase useful
answers from other models. The first success is selected initially, and the
user may select another successful result without running the request again.

The contract is deliberately bounded. At most eight targets and their owned
responses are retained. There are no dangling pointers to temporary prompt or
provider data.

## Local and online providers

A local provider adapter may connect to a model process, a model library or a
service running on the same machine. A remote adapter may connect to an online
model service. Both implement the same `UmiAiProvider` function table.

Studio includes a deterministic local reference provider for development and
tests. A real deployment registers the chosen local and remote adapters. A
runtime catalogue entry by itself describes a provider; it does not secretly
make a network connection.

After a provider is registered successfully, the runtime owns that adapter
instance and calls its cleanup function during shutdown. This prevents a local
model process, network client or plug-in allocation from being leaked.

Online credentials must be stored through a local secret-provider reference.
Configuration records store the reference, never the secret value. Do not put
an API key in source code, a layout, a prompt archive, a log message or Git.

## Chat and agent tasks

Chat is conversational. An agent task may also inspect context, propose a plan,
invoke allowed tools and prepare changes. Tool authority is separate from model
access. Giving a model permission to answer does not give it permission to
write files, run a command, use source control or submit a business operation.

Risky actions must remain visible and reviewable:

1. The assistant explains its plan.
2. Framework checks the tool and workspace policy.
3. The user approves an action when policy requires it.
4. Framework records the tool result and validation evidence.
5. A code patch or business operation receives its own final review.

In a trading application, an assistant may analyse data or prepare an order
idea. A live order still requires the trading risk checks, account permissions,
environment selection and user confirmation owned by the trading platform.

## Retrieval and the user's knowledge folder

Retrieval lets a model answer with selected local evidence. Documents are
ingested into a knowledge collection, split into bounded chunks and searched.
Matches keep source and citation information so the user can inspect where an
answer came from.

The application should show exactly which sources will be shared remotely.
Local-only collections stay local unless the user changes policy and approves
the route.

## Using the windows in another application

After the application has created its AI runtime and workspace customisation
model, it can register the shared window catalogue:

```c
UmiStatus status = umi_ai_ui_assistant_windows_register(
    &workspace->windows);
```

The application then maps the stable tool IDs to its composed services. It may
use all windows or only the ones appropriate for its purpose. Framework still
owns layout movement and persistence.

## Studio composition

Umicom Studio IDE is an application-development environment. Its thin product
layer composes the Framework editor, project, build, test, debug, source-control,
designer, terminal, chat, coding-agent, RAG and model-comparison services.

The current workbench adds the Umicom icon and the title **Umicom Studio IDE**
to the application bar. The menu inventory comes from Framework application
shell contributions. Users can add, hide, move between the left, right and
bottom dock regions, and save the assistant windows like other Studio panels.
The Framework workspace contracts also retain floating, grouping and
multi-monitor metadata for hosts that expose those presentation controls.

## Current boundary and next implementation work

The contracts, catalogues, local reference route, comparison service and view
models are present. Production provider adapters still need to be registered
by each deployment. Further work should add streamed comparison updates,
provider-specific configuration screens, cancellation, retry/backoff evidence,
cost and token limits, transcript branching, export, and end-to-end encrypted
secret-provider integration tests. The GTK host should also finish projecting
Framework floating/grouping metadata into native secondary windows.

These additions should extend the same contracts. They should not create a
second chat runtime inside an application.
