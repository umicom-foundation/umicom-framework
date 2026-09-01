<!--
Umicom Framework
File: docs/SOURCE_INTELLIGENCE_AND_QUALITY_LEDGER.md

PURPOSE:
Define a reusable source-intelligence service that indexes a workspace,
explains code relationships and keeps a durable history of quality findings.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Source Intelligence and Quality Ledger

## Purpose

A large codebase becomes difficult to understand when knowledge lives only in
individual files or in a contributor's memory. A source-intelligence service
can walk a workspace, build a searchable map and answer practical questions:

- Where does this file live and what responsibility does it have?
- Which declarations does it provide and which code uses them?
- Which application, panel, layout or engine depends on it?
- Does its public contract explain ownership, failure and lifetime rules?
- Has a similar defect appeared before, and was it fixed?
- Which unresolved findings should be addressed next?

This is achievable without creating a second compiler or copying existing
Framework scanners. The service should coordinate the indexing, CodeGuard,
diagnostic, documentation and data services already present in Framework.

## Reusable services that already exist

| Existing service | Responsibility in the source-intelligence system |
| --- | --- |
| `UmiFileIndex` | Discovers files and supports incremental updates when a file changes. |
| Editor and language symbol indexes | Describe functions, types, declarations and symbol relationships. |
| CodeGuard service and rule registry | Run repeatable source-quality, unsafe-API, duplication, architecture and dependency checks. |
| CodeGuard documentation coverage and inventory | Find files, functions and decisions that need more explanation. |
| Diagnostic providers and parsers | Normalise compiler, linker, test, runtime and tool findings. |
| Diagnostic baseline and problem model | Compare current findings with an earlier accepted state. |
| Data Server and its SQLite adapter | Persist scan runs and issue history without exposing database handles. |
| Developer Workbench search and navigation | Open code from a search result, relationship or issue. |
| Living documentation generator | Turn reviewed source comments and guides into linked local HTML pages. |

The new layer is an orchestrator and a durable index. It does not replace the
services in this table.

## Information recorded for each workspace

### Workspace and file records

A workspace record identifies one repository checkout. It stores a stable
identifier, root path, active revision, scan policy and the last complete and
incremental scan times. Machine-specific paths stay local.

Each indexed file records its relative path, language, broad role, content
digest, reviewed description, owning module, documentation coverage and latest
quality status. The digest avoids rescanning unchanged files. A removed file is
marked absent before its old relationships are retired, preserving useful
history without presenting it as live code.

### Symbol and relationship records

A symbol record identifies a function, type, constant or public contract. It
stores declaration and definition locations, visibility, documentation and
ownership guidance.

Relationships form the code map. Important relationship kinds include:

- file includes file;
- implementation defines declaration;
- function calls function;
- test verifies symbol;
- application adopts Framework contract;
- panel belongs to layout;
- command opens panel;
- module links to module;
- guide describes module;
- finding affects file or symbol.

Every relationship records how it was discovered. Parser or compiler evidence
has higher confidence than a text-only inference.

## The quality ledger

The quality ledger gives a finding a stable identity instead of treating every
scan as a new list. A finding records its fingerprint, category, severity,
confidence, rule, affected location, explanation, remediation guidance,
first-seen scan, most-recent scan, resolved scan, state and reviewed evidence.

The supported lifecycle is:

```text
new -> open -> fixed
        |       |
        |       +-> reopened when the fingerprint returns
        +-> accepted or deferred with a reason and review date
        +-> false positive with evidence
```

A finding becomes fixed only when a later complete scan covers the same scope
and no longer reports its fingerprint. An interrupted or partial scan cannot
prove that an issue was resolved. This makes it possible to report newly found,
open, fixed, reopened, accepted and deferred totals by module or application.

## Crawl and analysis pipeline

```text
workspace discovery
        |
        v
file classification and content digest
        |
        +---- unchanged file ----> reuse reviewed index records
        |
        v
symbol and relationship analysis
        |
        +----> documentation coverage
        +----> CodeGuard quality and security rules
        +----> dependency and architecture checks
        +----> build, test and runtime diagnostics
        |
        v
finding normalisation and fingerprint matching
        |
        v
quality ledger transaction
        |
        +----> search and local HTML reference
        +----> Studio panels and progress reports
        +----> optional intelligence recommendations
```

The ledger update is transactional. If a scan stops halfway through, partial
observations may help diagnosis, but they do not replace the last successful
complete result.

## Quality and security analysis

Repeatable checks run before any optional model analysis. The combined service
can coordinate checks for:

- missing file headers, public API guidance and decision comments;
- unsafe string, memory and path operations;
- unchecked sizes, suspicious conversions and buffer-boundary mistakes;
- ownership mistakes, double cleanup, use-after-free and leaked resources;
- nullability and state-transition mistakes;
- duplicate files and copied implementations;
- public declaration and dependency visibility;
- thin-application and other architecture rules;
- vulnerable or unapproved dependencies;
- exposed credentials, tokens or private machine data;
- compiler, linker, test, runtime and packaging regressions;
- changes to public API and binary compatibility baselines.

Static analysis reports risk; it does not prove that every reported path is a
real defect. Findings carry confidence and evidence for human review.

## Documentation and optional intelligence

The index improves living documentation without inventing descriptions.
Reviewed file headers and structured API comments remain the source of truth.
The generator adds navigation derived from verified relationships: module and
file maps, declarations, definitions, callers, tests, application adoption,
panel and layout connections, documentation gaps and issue history.

An optional provider-neutral intelligence layer may use the verified index as
retrieval context. It can explain code, summarise changes, propose tests, find
related components and suggest future features. Suggestions remain separate
from confirmed diagnostics and cite the files, symbols and guides used as
evidence. Draft descriptions require review before they become documentation.

The user chooses a local or remote model provider. Before remote use, privacy
filters remove secrets, credentials and unrelated private content. Local
credentials are stored through the Framework secret service and never written
to source files, generated documentation or the quality ledger.

## Reusable application panels

Framework should provide toolkit-neutral models for these shared panels:

| Panel | Purpose |
| --- | --- |
| Source Map | Browse modules, folders, files and reviewed descriptions. |
| Relationship Explorer | Move between declarations, definitions, callers, tests, panels and layouts. |
| Quality Ledger | Filter open, fixed, reopened, accepted and deferred findings. |
| Documentation Gaps | Find files, functions and decisions needing explanation. |
| Scan Runs | Inspect scan scope, duration, completion and analyzer results. |
| Recommendations | Review evidence-backed suggestions separately from confirmed findings. |
| Progress Dashboard | Show issue, documentation and quality trends. |

Studio can arrange all seven panels in a development workspace. Other
applications can open a smaller subset. Docking and saved arrangements remain
owned by the shared workspace services.

## Proposed command-line journeys

These names describe the intended user experience. Each command is a thin
client of the same Framework service used by Studio:

```text
umicom intelligence scan PATH
umicom intelligence status PATH
umicom intelligence find PATH QUERY
umicom intelligence explain PATH FILE
umicom intelligence issues PATH
umicom intelligence docs PATH
umicom intelligence recommend PATH
umicom intelligence export PATH --format sarif
```

`scan` is incremental by default and offers an explicit full-scan mode.
`status` never changes source files. Exports omit credentials and local paths
unless the user deliberately includes them.

## Delivery order

1. Create the workspace, file and scan-run index over `UmiFileIndex`.
2. Store symbols and relationships from existing editor and language services.
3. Add the quality-ledger schema and stable finding fingerprints.
4. Import CodeGuard, diagnostic, build and test results through adapters.
5. Generate relationship-aware local HTML documentation.
6. Add the shared panel models and adopt them in Studio.
7. Add evidence-backed optional recommendations and privacy controls.
8. Connect unresolved issue and documentation totals to release governance.

Each stage is useful on its own. The first release should favour accurate,
searchable facts and durable issue history. Suggestions become valuable only
after that foundation is trustworthy.
