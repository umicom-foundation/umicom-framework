# AI evidence: focused risk review

This review covers the Batch 39 changes, not every Umicom application or all AI behaviour.

## Reproduced defect

The previous public `UmiAiWorkspaceBuildRequest` cleared and filled caller storage before validating
all evidence. A deliberately invalid second passage returned INVALID_ARGUMENT while leaving a
partial request. The exact original implementation produced `caller output unchanged=no`; the
replacement produced `caller output unchanged=yes` for the same input. The evidence archive contains
the complete reproduction and outputs. No network request was made by that reproduction.

The corrected builder validates a private candidate before publication. Its original code is
preserved for review. Existing framing, roles, reference hints and citation validation remain.

## Risks addressed in this change

- Inspected hybrid or reranked results could previously only be passed indirectly through a fresh
  lexical preparation. The additive selected-evidence path retains source order without repeating
  retrieval, checks exact copied source fields and refuses stale corpus revisions.
- Forged, duplicate, cross-collection or changed selections are rejected before job publication.
  Non-finite scores cannot enter this path. Score metadata is not claimed to be durable provenance.
- A historical citation must not silently display current replacement text. The immutable inspection
  keeps frozen text separate from current comparisons and remains readable after source services close.
- Reference locations use checked bounded scanning. Allocation/capacity failures do not publish
  partial output. Literal references are explicitly not semantic verification.
- New native HTTP tests exercise the real local client with fragmented responses, Unicode, bad JSON,
  model mismatch, tool calls, bad citations, HTTP errors, redirects, disconnection, timeout and cancellation.
  Fixture responses are not portrayed as learned model output.

## Remaining limitations

1. Writers/reviewers remain local labels, not authenticated identities. The feature is not a signed
   approval, cryptographic provenance record, tamper-proof journal or security boundary against a
   malicious process able to modify the Data Server storage.
2. A selected passage can itself contain misleading statements or adversarial instructions. The
   existing prompt and no-tool request reduce accidental authority mixing but are not a proof of
   prompt-injection resistance. A real model can still produce unsupported prose with valid markers.
3. Inspection reads the current *loaded* workspace. It does not detect an external database change
   until the owner reloads successfully. Original files, true source line locations, retrieval-model
   identity and provider/model binary identities are not verified by these records.
4. The existing fixed limits remain. There is no large-file chunker, persistent vector index,
   learned embedding runtime, background corpus refresh or full-text document import in this batch.
5. The GTK additions retain existing single-worker rules and signal cleanup. GTK development files
   were unavailable here, so the six new and two retained graphical cases were not compiled or run.
   Windows/Winsock and actual installed application journeys also require local validation.
6. New immutable snapshots are safe to read only while their owner preserves their lifetime.
   Concurrent destruction, workspace use from another owner thread or callbacks that violate the
   existing contract remain caller errors. No repository-wide race-freedom claim is made.
7. Plain-text reports can contain private prompts, sources and output. Review/redact before sharing.
   No automatic telemetry is introduced. The new CLI reads no personal documents and uses fictional
   built-in text, but the graphical workspace continues to persist its selected database as before.
8. The optional local command sends one actual request to an explicitly selected loopback port.
   A local process may be untrusted. No server/model is bundled, downloaded or automatically started.
   The CLI times out after ten seconds; the existing GUI provider uses thirty seconds. Unavailable,
   cancelled or malformed responses are not substituted with a successful offline answer.
9. Literal scanning includes quotes and code blocks. `[S01]` remains a legacy resolvable marker;
   noncanonical spelling is flagged rather than invalidating old stored jobs. More than 128 markers
   causes a capacity error. A successful binding check says nothing about statement-level support.
10. The full Framework/Applications build, real neural-model inference, power-loss persistence and
    end-user deployment are outside the executed evidence. Earlier financial and broker fixes are
    untouched, but their whole application suites were not rerun as part of this focused delivery.

## Authoring runs and final runs

Two development-only failures were corrected before validation: a misleading-indentation warning
in a new test entry point and an overly broad case-sensitive report assertion. They were test
implementation mistakes, not defects attributed to the original repository. Preliminary logs remain
separate. The final GCC and Clang suites each passed 153 cases; a Python-discovery-disabled run
passed 143 native cases; disabling HTTP and SQLite yielded 103 passes and 50 explicit skips.

No real broker, financial action, model server, external network endpoint or git remote was invoked.
The loopback fixture tests opened temporary local sockets. Build/test/package tooling wrote only
its disposable workspaces and delivery files.
