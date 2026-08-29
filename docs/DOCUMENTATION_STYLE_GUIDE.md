<!-- --------------------------------------------------------------------------
Umicom Framework
File: docs/DOCUMENTATION_STYLE_GUIDE.md

PURPOSE:
Help every contributor write clear Umicom documentation for readers of all
ages, including beginners with no previous software-development experience.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
----------------------------------------------------------------------------- -->

# Documentation Style Guide

Umicom documentation is written for human beings. A reader should not need to
know the project history, internal planning codes or advanced computer science
before they can understand a feature.

Imagine that the reader is 16 years old, curious and using the project for the
first time. Do not make the writing childish. Make it patient, direct and easy
to follow.

## Use meaningful names

Give every document a title that says what the document teaches.

Good titles include:

- `Component Model and Composition`
- `Creating a New Application`
- `How Workspace Layouts Are Saved`
- `Troubleshooting the Windows Build`

Do not put batch numbers, milestone codes or internal planning labels in a
document title or filename. Those codes do not explain the subject and become
confusing when plans change.

## Begin with the problem

Before describing structures or functions, answer three questions:

1. What problem does this feature solve?
2. Who uses it?
3. What should happen when it works?

A short everyday comparison can help. For example, reusable Framework
components can be explained as Lego bricks: applications choose and arrange
the bricks instead of manufacturing a new copy of every brick.

## Explain new words

Define a technical word the first time it appears. Expand an abbreviation
before using its short form. A small `Start here` or `Glossary` section is
useful when a document introduces several terms.

For example:

- A **component** is one reusable part of an application.
- A **contract** is a set of rules that code promises to follow.
- A **registry** is a searchable collection of known items.
- A **frontend** is the part that draws the user interface on the screen.

Do not assume that words such as lifecycle, adapter, semantic version,
transaction or conformance are already understood.

## Show how the pieces connect

Describe a feature in the order a person will use it. Prefer a small numbered
flow over a list of unrelated type names:

1. Register the reusable component.
2. Select it by its stable ID.
3. Add it to a layout.
4. Ask a frontend factory to create it.
5. Close it when the workspace shuts down.

When several types are involved, state which type owns the data and which type
is allowed to change it.

## Include a small example

Examples should be short enough to understand without scrolling through a
large program. Use real Framework names and commands. Explain what the reader
should see after running the example and what a common failure means.

Never label a design as implemented unless the code exists. Clearly separate:

- what works now;
- what is planned;
- what is optional; and
- what is not yet supported.

## Keep instructions safe and copyable

- Use the correct shell language on code fences, such as `powershell`, `c` or
  `cmake`.
- Show paths relative to the repository unless an absolute path is required.
- State the folder where a command must be run.
- Do not hide destructive steps inside a long command block.
- Explain how to verify success.

## Review checklist

Before accepting a document, check that:

- the title explains the subject without an internal code;
- the introduction explains the problem and outcome;
- new technical terms are defined;
- the main workflow is shown in order;
- at least one small example is provided when useful;
- ownership, errors and cleanup are explained;
- current behaviour is separated from future plans; and
- a beginner can identify the next action without asking what a term means.

