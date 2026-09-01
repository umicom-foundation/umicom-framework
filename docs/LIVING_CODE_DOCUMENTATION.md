<!--
Umicom Framework
File: docs/LIVING_CODE_DOCUMENTATION.md

PURPOSE:
Explain how source comments become a navigable code reference and how that
reference stays connected to the code it describes.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Living Code Documentation

Umicom keeps technical explanations beside the C declarations and decisions
they describe. This reduces the chance that a separate manual says something
different from the current code.

The generated reference brings those explanations together as linked HTML
pages. It includes:

- public structures, enumerations, constants and function contracts;
- parameter, return-value, ownership and lifetime guidance;
- links between declarations and the files that implement them;
- source listings and references to callers and related symbols;
- Framework and thin-application contracts in one searchable index;
- Markdown guides that explain larger workflows and architecture.

## How comments become reference pages

A structured public comment starts with `/**` and sits immediately above its
declaration. The first sentence explains the outcome in plain language.
`@param` entries explain what each input means, `@return` explains success and
failure, and the remaining paragraphs describe ownership and relationships.

```c
/**
 * Finds one registered panel by its stable identifier.
 *
 * @param catalogue Catalogue that owns the registered panel descriptions.
 * @param panel_id Stable identifier saved in layouts and user workspaces.
 * @return A borrowed panel description, or `NULL` when no match exists.
 *
 * The caller must not free or change the returned description. It remains
 * valid for as long as `catalogue` remains alive.
 */
const UmiPanelDefinition *umi_panel_catalogue_find(
    const UmiPanelCatalogue *catalogue,
    const char *panel_id);
```

Comments inside a function explain important choices such as capacity checks,
ownership changes, state transitions and cleanup. The HTML source browser
shows those comments beside the implementation.

## Generating the HTML reference

Configure the suite normally, then build the documentation target:

```powershell
Set-Location "C:\umicom\umicom-applications"

& "C:\msys64\ucrt64\bin\cmake.exe" --preset windows-ucrt64-debug
& "C:\msys64\ucrt64\bin\cmake.exe" `
    --build `
    --preset windows-ucrt64-debug `
    --target umicom-documentation
```

The target is provided when CMake finds a compatible source-documentation
generator. If configuration reports that the generator is unavailable,
install it through the development toolchain and configure again. Normal
application builds remain available while this optional tool is missing.

The start page is generated at:

```text
build/windows-ucrt64-debug/documentation/html/index.html
```

The generated folder belongs to the build tree and is not committed. A local
browser, including an application browser panel, can open `index.html` without
an internet connection.

## Keeping the reference current

The documentation target depends on C files, headers and Markdown guides in
Framework and the application modules. After one of those files changes, the
next documentation build refreshes the HTML pages. A development workspace
may call the same target after saving a file to provide an automatically
refreshed local reference.

The existing source-comment governance and documentation-coverage checks find
contracts and decisions that do not yet have enough explanation. This makes a
file-by-file improvement practical: older code can be documented carefully
without filling the repository with generated pages or guessed descriptions.

Two complete public areas are now protected by structured-comment audits:

- `SUITE_LAYOUT_ARCHITECTURE.md` maps 52 layout functions in 14 headers;
- `APPLICATION_PRODUCTISATION_ARCHITECTURE.md` maps 95 productisation and
  controlled-execution functions in 37 headers.

Later subsystems can adopt the same audit after their public contracts have
been reviewed carefully. The audit is added only after the comments describe
real ownership and behaviour; generated volume is not treated as useful
documentation by itself.

## What belongs in a comment

A useful comment answers questions the C syntax cannot answer:

- Why does this contract exist?
- Which service or application calls it?
- Who owns the memory?
- How long does a returned pointer remain valid?
- Which state must be established first?
- Which errors should a caller handle?
- Which Framework component should a thin application reuse?

Comments must remain truthful. When behaviour changes, the nearby comment is
part of the same change. Names should remain descriptive; comments add context
instead of replacing clear code.

## Connecting documentation to source intelligence

The planned source-intelligence service will add a searchable file and symbol
map, verified relationships and a durable quality ledger around this generated
reference. It will reuse the current file index, CodeGuard, diagnostics and
data services instead of creating competing scanners. See
`SOURCE_INTELLIGENCE_AND_QUALITY_LEDGER.md` for the records, issue lifecycle,
privacy rules, reusable panels and delivery order.
