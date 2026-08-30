<!--
Umicom Framework
File: docs/INTERACTION_FORMS_AND_UI_STATE.md

PURPOSE:
Explain portable commands, forms and UI state to beginning developers.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Interaction, Forms and UI State

## A beginner-friendly guide to how an Umicom application behaves

An application is more than panels arranged on a screen. A useful application
must also answer questions such as:

- What happens when the user presses a button?
- Which information does a command need?
- Is the current user allowed to run it?
- Why is a command disabled?
- How does a form explain an invalid value?
- Can a change be undone?
- What should a panel show while it is loading or has no data?
- How can GTK4, Qt6, web and headless tests behave consistently?

Umicom Framework now answers these questions through one toolkit-neutral
application experience model.

The model does not draw a button or text box. It describes what those controls
mean. A frontend adapter can turn the same description into GTK4 widgets, web
controls, Qt6 widgets, accessibility output or a headless test.

## The simple mental model

Think about a Lego model:

1. A **component** is a reusable brick, such as an explorer or order ticket.
2. A **workspace recipe** explains where the bricks are placed.
3. An **experience profile** explains how the workspace behaves.
4. A **frontend adapter** gives the model its visible shape.

The profile covers three connected areas:

| Area | Question it answers |
| --- | --- |
| Interaction | What commands exist, what do they need and when may they run? |
| Forms | What information is collected and how is it checked? |
| UI state | What should a component communicate right now? |

Every Framework workspace recipe now names one experience profile. There are
three profiles for each application:

- **Learning** explains more, validates gently and keeps recovery guidance
  visible.
- **Standard** provides balanced everyday behavior, inline validation and
  automatic draft preservation.
- **Focus** uses a compact, quieter presentation while retaining confirmation,
  undo and accessible announcements.

With 26 applications and three audiences, the catalogue contains 78 profiles.

## Typed values

A command argument or form field is represented by
UmiApplicationExperienceValue.

It supports Boolean, integer, decimal, text and explicitly empty values. The
value has a type, so Framework can reject text supplied where an integer is
required. Text is stored in a bounded buffer, preventing an unexpectedly long
value from writing beyond available memory.

~~~c
UmiApplicationExperienceValue value;

if (umi_application_experience_value_text(&value, "My project") !=
    UMI_STATUS_OK) {
  /* Explain that the value was missing or too long. */
}
~~~

## Parameterised commands

A simple command ID is enough for a toolbar button, but not enough for a
reusable application platform. UmiApplicationExperienceCommand also describes:

- A stable ID, title, description and category.
- The values required to run it.
- Whether it changes data or can be undone.
- Whether it needs a selection or online connection.
- Its risk level and required permission.

An invocation carries actual values, a correlation ID and the component that
started the operation. This makes a command traceable from a button press to
its result.

| Risk | Meaning |
| --- | --- |
| Safe | The command may run immediately. |
| Confirm | The user must confirm before it runs. |
| Restricted | Confirmation and an appropriate permission are expected. |

## Why a command is disabled

umi_application_experience_command_enablement() returns both a decision and a
plain-language reason. Examples include:

- “Select an item before running this command.”
- “Connect to a service before running this command.”
- “The current workspace is read-only.”
- “The current user does not have the required permission.”

A grey button with no explanation feels broken. A disabled command with a
reason teaches the user how to continue. The same reason can be shown in a
tooltip, command palette, status area or screen-reader description.

## Safe dispatch

The dispatcher applies these checks in order:

1. Find the command definition.
2. Validate all parameters.
3. Require confirmation for risky commands.
4. Check permissions.
5. Check selection, connection, busy and read-only conditions.
6. Find and run the registered handler.
7. Preserve the result and completion sequence.

Application logic therefore receives a request only after shared Framework
rules have passed.

## Reusable forms

A form schema describes meaning rather than widgets. It has a stable ID, title,
description, submit command and fields. Supported field kinds include text,
multiline text, Boolean, integer, decimal, choice, date, time, search and
secret input.

A field can define:

- Whether it is required or read-only.
- Minimum and maximum numbers.
- Minimum and maximum text length.
- Available choices.
- A default value and help text.

GTK4 might render a choice as a drop-down. A web frontend might use a select
element. A headless test does not render it at all. All three use the same
rules.

## Form models and validation

A schema describes the rules. A form model stores the current values, current
revision, saved revision and dirty status.

Validation creates a structured report. Each issue contains a field ID,
severity and human-readable message. A frontend may show the message beside a
field and also include it in a validation summary.

~~~c
UmiApplicationExperienceValidationReport report;

if (umi_application_experience_form_validate(&model, &report) ==
        UMI_STATUS_OK &&
    !umi_application_experience_validation_report_passed(&report)) {
  /* Present report.issues to the user. */
}
~~~

## Undo and redo

Framework compares two form models to create a typed changeset. It records the
field, old and new values, whether each value existed and the source/target
revisions.

Form history stores recent changesets. Undo applies a changeset in reverse.
Redo applies it forwards. Revision checks prevent an old changeset from
silently overwriting newer work.

This is safer than remembering individual key presses because the history
describes meaningful data changes.

## Standard UI states

Every professional component should support these states:

| State | What it communicates |
| --- | --- |
| Ready | The component can be used. |
| Empty | Nothing exists yet and a helpful next action is needed. |
| Loading | Information is being prepared. |
| Busy | An operation is running. |
| Success | An operation completed. |
| Warning | Attention is needed but work may continue. |
| Error | Something failed and recovery guidance is required. |
| Offline | A service-dependent feature has no connection. |
| Permission required | Access is needed before continuing. |

An empty state should not be a blank rectangle. It should explain what is
missing and offer an action such as “Create project”. An error should explain
what failed and, when possible, provide a retry command.

## State transitions

UmiApplicationExperienceStateStore keeps the current state and a bounded
history. Each transition records the old state, new state, reason and sequence
number.

A test can prove that a component changed from Loading to Ready because project
loading completed. Diagnostics can explain the same sequence without reading a
frontend event log.

## Portfolio profiles

Profiles live under:

~~~text
src/application/experience/profiles/
~~~

Each application owns three declarative records. Examples:

- Studio uses project run and project settings.
- Trader uses order submission and an order ticket.
- TMS uses trade saving and trade capture.
- Bank uses payment submission and payment creation.
- Accountant uses journal posting and journal entry.
- Education uses lesson publishing and lesson settings.

These IDs are contracts. Later updates can register complete domain command and
form schemas without changing the workspace/profile relationship.

## Inspecting the catalogue

After building Framework tools:

~~~powershell
& ".\build\windows-ucrt64-debug\bin\umicom-experience-profiles.exe" list

& ".\build\windows-ucrt64-debug\bin\umicom-experience-profiles.exe" show "org.umicom.experience.studio.learning"

& ".\build\windows-ucrt64-debug\bin\umicom-experience-profiles.exe" validate
~~~

Validation checks that profile and recipe counts match, IDs are unique, each
application exists, and every profile and recipe agree on application and
audience.

## Adding a reusable command

1. Choose a descriptive ID, such as umicom.command.document.publish.
2. Describe its parameters.
3. Set risk, flags and permission.
4. Register it in the shared command registry.
5. Register a handler with the dispatcher.
6. Add headless tests for missing input, denied permission and success.
7. Let frontend adapters present the same command where appropriate.

Do not create separate GTK4, web and Qt6 definitions for the same behavior.

## Adding a reusable form

1. Choose a stable form ID.
2. Define fields by business meaning, not widget names.
3. Add bounds, required values, choices and help.
4. Connect the schema to a parameterised submit command.
5. Test valid, invalid, undo and redo behavior headlessly.
6. Add frontend rendering only after the toolkit-neutral contract works.

## Adding an application profile

1. Add Learning, Standard and Focus workspace recipes.
2. Add matching experience profiles.
3. Give each recipe the matching experience_profile_id.
4. Select a meaningful primary command and form.
5. Run the catalogue and bridge tests.

The profile and recipe must agree. A layout without behavior is incomplete, and
behavior without a layout has nowhere to appear.

## Current boundaries

This update provides the toolkit-neutral behavior model and portfolio data. It
does not yet draw GTK4 controls, replace every specialist command system,
implement every product command, connect a real identity provider or persist
drafts to a database.

Those integrations should adapt existing Framework services to this common
contract. Existing features remain available while duplicate public behavior
is gradually consolidated.

## Why this matters

An application can now be assembled from reusable components, portable
workspace recipes, interaction rules, form schemas, predictable UI states and
audience-appropriate policies.

That is another step toward building Umicom applications like Lego: choose
well-defined parts, connect them through stable contracts and avoid copying the
same behavior into every product.
