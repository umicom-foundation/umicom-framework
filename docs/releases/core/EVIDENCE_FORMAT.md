# Release evidence: format and trust boundary

The public entry point is `umicom/distribution/runtime/evidence.h`; implementation is attached to the existing `Umicom::distribution` owner. It uses the existing `umi_dr_release_gate_pass` function. It neither replaces the live capability registry nor registers unqualified services as available.

## Contract grammar

ASCII TSV only, up to 512 KiB, 128 requirements and 2,047 bytes per logical line. LF and CRLF are accepted; a final LF is optional. Blank lines and lines beginning with `#` are ignored. Embedded NUL, other control characters, non-ASCII bytes and malformed rows are refused. Identifiers use letters, digits, dot, underscore or hyphen; they are case-sensitive.

The first three non-comment records must be:

```
UMICOM-RELEASE-CONTRACT<TAB>1
candidate<TAB>label
source<TAB>40-lowercase-hex-baseline-revision
```

A requirement has eight tab-separated fields:

```
require  id  category  profile  configuration  kind  ownerBatch  title
```

Categories are `signature`, `checksum`, `compatibility`, `tests`, `frontend`, `other`. At least one requirement in each of the first five categories is mandatory. Every declared row is required; there is no automatic waiver or optional-skip flag. Kinds are `native`, `installed`, `analysis`, `review`.

The candidate label and expected source revision are metadata supplied by the author. They are not a content hash of the checkout or package. Before recording a new release candidate, create and inspect its exact source/artefact manifest and update the label, revision and contract together. Dirty overlays and different builds of one Git revision need distinct candidate labels plus independently inspected manifests.

## Evidence grammar

The first non-comment record is `UMICOM-RELEASE-EVIDENCE<TAB>1`. Zero results are valid input and correctly leave every requirement missing.

A result has sixteen fields:

```
result id candidate source profile configuration kind outcome total passed failed skipped notRun asserted digest reference
```

`outcome` is exactly `passed`, `failed`, `skipped` or `not_run`. Counts are unsigned decimal 64-bit integers. Their sum must equal total without overflow. For a reported-complete row, total must be nonzero, passed must equal total, and every failure/skip/not-run count must be zero. For a manual review or installed journey, these are named checklist items, not an invented CTest total.

The literal field `asserted` is compulsory: these are claims supplied by a producer. `digest` is 64 lowercase hexadecimal characters or `-`; `reference` is a relative slash-delimited label using the identifier alphabet or `-`. Parent segments, absolute paths and empty path segments are refused. The checker does not open that reference. Missing digest/reference blocks completion of a reported passed row.

Evidence IDs must exist in the contract, and duplicates are refused. Candidate, source, profile, configuration and kind must match their requirement exactly. Different platform/build-stage evidence is not interchangeable. Record provenance, exact test names and native logs separately: matching counts cannot detect one missing case replaced with another.

## Results

The evaluator publishes a complete result only on success. Failed parse/assessment leaves the caller's previous output untouched. Parsed objects own their data and can outlive input buffers. Concurrent immutable reads are allowed with separate output objects; destruction must be serialised with readers. The API does not retain outside files, controllers, services or user data.

`show CONTRACT` prints the requirements and deliberately reports missing evidence; exit zero means the catalogue was displayed. `check CONTRACT EVIDENCE` returns 1 for blocked evidence, 2 for malformed or unreadable inputs, and 0 only for structurally complete assertions. Even zero is not an approved release.

Do not automatically wire this early metadata check to package promotion. Native CTest inventory/JUnit ingestion, actual artefact verification and authenticated owner acceptance remain later checkpoint work.
