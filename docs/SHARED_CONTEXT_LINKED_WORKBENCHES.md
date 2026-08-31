<!--
Umicom Framework
File: docs/SHARED_CONTEXT_LINKED_WORKBENCHES.md

PURPOSE:
Explain how canonical application experiences become reusable context-linked
workbenches and how junior developers can extend the system safely.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Shared Context-Linked Workbenches

## Why this exists

An Umicom application is meant to feel like it was assembled from reliable
building blocks. A developer should be able to describe panels, layouts and
capabilities, then let Umicom Framework connect the reusable parts.

This feature turns every canonical application experience into a workbench
context profile. In simple terms, a panel can now say:

> “I belong to this colour group, I understand these kinds of selection, and
> I am allowed to publish or only follow changes.”

The application does not need its own router, selection bus or copied panel
wiring. The Framework remains the owner of those services.

## The mental model

Imagine a workbench containing an explorer, editor, properties panel and
preview. When the user selects an item in the explorer, the other panels in
the same colour group can follow that selection.

The information moves through five reusable definitions:

| Definition | Plain-English meaning |
| --- | --- |
| Application experience | The official list of an application's panels, layouts and features. |
| Panel capability | What a panel is for, such as editing, banking, media or market data. |
| Context kind | The stable shape of the shared selection, such as project, account, instrument or media. |
| Colour group | The set of panels that are allowed to follow one another. |
| Endpoint policy | Whether a panel publishes selections, follows selections, or does both. |

This is the “Lego” rule: applications choose and configure Framework-owned
pieces. They do not rebuild the studs that join those pieces together.

## What the Framework now provides

The shared implementation contains three layers:

1. `experience_policy` reads a panel's existing capability and flags. It
   produces a panel role, accepted context kinds, published context kinds and
   link mode.
2. `experience_profile` groups panels by their existing `context_group_id`
   and builds a valid `UmiWorkbenchContextHostProfile`.
3. `experience_profile_catalogue` builds and registers profiles for the whole
   canonical application catalogue.

The default registration covers all current application experiences. It does
not replace the specialist Trader/TMS profile. A specialist profile can still
be selected when it provides tighter rules than the catalogue default.

## Active and passive panels

There are two useful endpoint behaviours:

- An **active panel** can publish its selection and follow another panel.
- A **passive panel** follows compatible context but does not publish it.

A panel marked `UMI_EXPERIENCE_PANEL_CONTEXT_LINKED` is active by default,
unless it is read-only, an order-entry surface or a context inspector. Those
surfaces follow context because silently changing the current account, trade
or payment from them could be surprising or unsafe.

Panels that have a colour group but are not marked context-linked are still
included as passive endpoints by default. This is important for applications
such as Umicom OS: its current panels can receive shared workspace context
without pretending that every system-information view is already an active
publisher.

## How product families use the same stable context kinds

The existing `UmiContextKind` values are deliberately preserved. Hundreds of
records validate those stable values, so changing their numbers or adding an
uncoordinated new upper bound would cause compatibility problems.

The policy combines the current kinds in different ways:

| Application family | Shared context kinds used today |
| --- | --- |
| Studio, Web Studio, Mobile Studio, Database Studio | project, workspace, source location, selection |
| Trader, TMS, Exchange | instrument, account, trade, selection |
| Bank and Accountant | account, trade, selection |
| LLM, RAG and AI Creator | project, workspace, generic, selection |
| Music, Media and Games | media, project, selection |
| Desktop, OS, Operations and Security Centre | workspace, generic, selection |
| CAD, Kitchen and Author | project, workspace, media or selection according to capability |

`generic` does not mean “unknown”. It is the safe envelope for a
product-specific identifier whose full schema has not yet been promoted into
the stable cross-application ABI.

## Build a profile for one application

```c
UmiWorkbenchContextHostProfile profile;
UmiStatus status;

status = umi_workbench_context_host_experience_profile_build_for_application(
    "org.umicom.web-studio",
    NULL,
    &profile);
```

Passing `NULL` options selects the safe defaults and includes passive panels.

## Register every application profile

```c
UmiWorkbenchContextHostProfileCatalogue catalogue;
UmiWorkbenchContextHostExperienceProfileSummary summary;

umi_workbench_context_host_profile_catalogue_init(&catalogue);

if (umi_workbench_context_host_experience_profiles_register(
        &catalogue, NULL, &summary) != UMI_STATUS_OK) {
    /* Report the failure and do not start the workbench. */
}

/* Use the catalogue, then release its owned memory. */
umi_workbench_context_host_profile_catalogue_destroy(&catalogue);
```

The summary reports application, group, active endpoint and passive endpoint
counts. It is useful for startup diagnostics and release evidence.

## Override a policy without forking the Framework

An application may need a stricter rule. For example, a payment approval panel
might accept account context but never publish it. Initialise the options and
provide a resolver callback:

```c
static UmiStatus resolve_payment_policy(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    const UmiWorkbenchContextHostExperiencePanelPolicy *default_policy,
    void *user_data,
    UmiWorkbenchContextHostExperiencePanelPolicy *out_policy)
{
    (void)experience;
    (void)user_data;
    *out_policy = *default_policy;

    if (strcmp(panel->panel_id, "payment-approval") == 0) {
        out_policy->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW;
        out_policy->published_kinds_mask = 0U;
    }
    return UMI_STATUS_OK;
}
```

```c
UmiWorkbenchContextHostExperienceProfileOptions options;

umi_workbench_context_host_experience_profile_options_init(&options);
options.policy_resolver = resolve_payment_policy;
```

The Framework validates the result after the callback. A callback cannot
accidentally describe a follow-only panel that still publishes context.

## Add a new application or panel

For a new panel, define these fields in the application's canonical experience:

1. Give the panel a stable `panel_id`.
2. Choose an existing Framework capability, such as `umicom.editor`,
   `umicom.payments`, `umicom.market-data` or `umicom.media`.
3. Set a `context_group_id`, such as `development.blue` or
   `operations.green`.
4. Add `UMI_EXPERIENCE_PANEL_CONTEXT_LINKED` only when the panel really emits
   stable selections.
5. Add `UMI_EXPERIENCE_PANEL_READ_ONLY` when the panel must never initiate a
   change.
6. Build the profile and inspect its endpoint policy in a focused test.

Do not create a second message bus or a product-local copy of Context Host.
If a reusable capability is missing, define it once in Framework and let all
applications adopt it.

## Design lessons used in this implementation

The implementation uses familiar professional software concepts while keeping
the design and terminology entirely Umicom-owned:

- panel identity stays separate from layout placement, allowing one tool to
  appear in different saved workspaces;
- specialised panels share one host and communicate through typed contracts;
- policy is injected through callbacks instead of being hard-coded into a
  component;
- linked panels use typed colour groups and endpoints to share a subject;
- treasury and lifecycle workflows remain application responsibilities while
  their workbench behaviour stays reusable;
- AI and retrieval services remain separate from their workbench presentation;
- operating-system services remain portable and explicit behind adapters.

## Recommended next implementation steps

The next work should deepen definitions instead of creating application-local
copies:

1. Add schema-backed context payload extensions for banking customers,
   payments, AI knowledge sources and OS resources while preserving the
   current context-kind ABI.
2. Bind the generated profile catalogue into the shared application bootstrap
   so GTK4, headless and future frontends receive the same profile.
3. Add product-specific policy packs for Studio, Bank, LLM/RAG, Media and OS.
4. Add a Context Inspector projection that explains why an endpoint accepted,
   rejected or followed a selection.
5. Persist user colour-group assignments through the existing context-host
   session service.
6. Add release evidence proving every context-linked panel has a valid group,
   capability policy and test.

These steps keep the framework stable while making each new application
quicker to assemble.
