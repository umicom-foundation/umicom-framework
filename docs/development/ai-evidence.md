# Selected AI evidence and immutable inspection

Author: Sammy Hegab, Umicom Foundation. Implementation: C23. Licence: MIT.

## Canonical owner

The capability extends `Umicom::ai_workspace`; it creates no alternative corpus, provider
registry, persistence engine or job lifecycle. `UmiAiWorkspaceSearch` remains the existing
lexical/vector/reranker boundary. `UmiAiWorkspacePrepare` retains its automatic lexical path.
`UmiAiWorkspacePrepareEvidence` is the additive bridge for explicitly inspected ordered results.

The Master Controller owns application composition and service lifetime. The AI workspace
Slave Controller owns preparation and job transitions. Toolkit adapters display copied data;
they do not author a second corpus or decide that cited prose is financially authoritative.

## Preparation contract

1. Capture the current corpus revision on the workspace owner thread.
2. Obtain canonical retrieval results, or deliberately select current copied sources.
3. Pass one to four sources, in the desired numbering order, to PrepareEvidence.
4. The service validates identifiers, finite nonnegative scores, uniqueness, collection
   membership, exact text/title/line range, source revision and expected corpus revision.
5. Preparation publishes through the existing all-or-nothing Data Server operation.
6. Approval and execution remain separate explicit calls. Existing provider policy and
   corpus-change checks still apply before a request may run.

Scores are transient, not persisted in the existing schema or included in request identity.
The input source order and exact source fields are included. Ranking model/version and binary
identity are not recorded by this extension. Repeating an identical job with an identical valid
selection is idempotent; reusing its ID with changed sources or order is a conflict. After a
corpus change the caller must reload/inspect rather than claim an old selection is current.

The GUI's Inspect retrieval still performs its existing lexical search. Prepare inspected
results freezes the first four displayed passages (or fewer when fewer exist), in that order.
The public API additionally accepts hybrid/reranked results. This batch adds no learned encoder
or graphical hybrid-configuration form. Tests using synthetic vectors are not embedding inference.

## Inspection contract

`UmiAiEvidenceCapture` copies one saved job plus current source comparisons from the *loaded*
workspace. It neither reloads the database nor changes a job, approval or source. A captured
review is immutable and can outlive its workspace, runtime and Data Server. Its destroy operation
must not race readers. Capturing and workspace mutation use the existing single-owner rules.

The report retains the frozen prompt, provider/model labels, job state, loaded/prepared corpus
revisions and exact source text. It reports unchanged, changed and removed sources separately.
A changed source is shown as a comparison, never substituted as the original citation target.
An unrelated source change can change the corpus without changing the cited sources.

Reference scanning is literal: it recognises every `[S` prefix, including one inside quoted text
or a code block. It is not a Markdown parser. Canonical `[S1]` through `[S4]` are preferred.
Existing accepted `[S01]` spelling remains resolvable and is flagged as noncanonical. Unknown
or malformed references are report data. The scanner does not assess entailment or truth.
A response is reported only for a succeeded job with nonempty retained output. Length-limited
output remains visible as such. Pending/failed/cancelled work is not a successful answer.

`UmiAiEvidenceCopyLines` returns an inclusive line range from the frozen passage. Returned bytes,
including available line endings, are unchanged; offsets are UTF-8 byte offsets, not screen columns.
The original line labels were supplied during ingestion. No original file is opened or authenticated.

## Failure atomicity and memory

The public request builder now allocates a private candidate and publishes it to caller storage
only after every passage and frame has validated. The superseded implementation is preserved
in an explained `#if 0` block. Invalid late input and allocation failures leave caller storage
unchanged. There is one extra bounded temporary allocation in this path.

The review renderer similarly writes into an owned temporary buffer, then copies on success.
Capture sets its output handle to NULL on error. Copy/format/getter errors leave caller records
unchanged unless their individual public contract explicitly says otherwise. Every new owned
allocation is released in the native fault-injection and lifecycle tests.

## Integration and compatibility

New code is linked into the existing workspace library. Its public header is installed by the
existing directory installation. The optional GTK adapter adds controls/pages without removing
previous controls. Existing worker cancellation/join handling is retained; new inspection is
synchronous and bounded. The CMake extension includes test definitions, not a deferred
`add_subdirectory`, preserving the earlier build-integration fix. New validation targets are
registered when the parent supplies its validation registry.

No financial, broker, Winsock, installer or OS implementation is modified. No schema migration,
credential storage, provider download or autonomous tool execution is introduced. Existing
Python HTTP tests remain independent alternatives. The new HTTP fixture is native C.

## Limits and validation boundary

Existing workspace limits: 16 collections, 64 sources, 32 jobs, four passages per grounded job,
1535 UTF-8 bytes per passage/prompt, 2047 bytes per response, 128 literal references per inspection.
The report buffer limit is 32768 bytes including the terminator. Overflow is an explicit error,
not truncation. Corpus/source comparisons are optimistic observations, not locks against external edits.

The focused host builds actual canonical sources but is not the complete Framework SDK. Windows
and GTK execution require separate qualification. The native loopback tests prove transport and
saved-job handling against an inert peer, not model inference. Consult Validation.txt for exact runs.
