# Framework release qualification roadmap

Status: development candidate; no new semantic-version tag or public release is
created by this batch. This is the first consolidation slice towards the next
major product milestone, not a declaration that the complete estate is ready.

## Recorded starting point

- Framework: `120b09f63e6b3ff8a7f5f350f45f2065d9d3a000`.
- Applications: `6315cb29fe251feb69af8137729d1007eaee0ccf`.
- Studio source observed in the connected repository:
  `94ddcc301ea42a878148cffecb48836c7474386d`.

The older `FOUNDATION_RELEASE_SCOPE.md` describes its historical foundation
milestone. It is not evidence that this candidate has met today's complete
application, installation and graphical acceptance gates.

## Implemented in this candidate

| Work | Completion boundary |
|---|---|
| Capability-based input-validation test naming | Two test files move together; parent CMake, internal targets and test names follow the new path; 14 original groups retained |
| Git lesson filename | Canonical page and all supplied course links use git-first-commit; the previous URL is a compatibility link |
| CodeGuard naming | Extend the existing classifier/scanner contract; retain existing public functions and original rule/enum values |
| Lesson integrity | Real metadata, title/heading agreement, unique anchors, local links, negative cases and bounded parser inputs |
| Qualification entry point | Standalone CMake build of real naming and teacher subjects; no duplicate application runtime |
| Contributor material | Add a naming reference without adding an assessed course ID or changing learner progress |

Only the explicitly changed paths in the manifest are supplied. The previous
production input fixes and all existing exercises are prerequisites, not new
engineering counted again in this candidate.

## Next release milestones

### Resource discovery and installed learning

Use the existing Framework resource services to resolve source and installed
lesson documents from a workspace unrelated to the Applications checkout.
Preserve the established catalogue IDs and avoid a second resource locator in
Studio. Acceptance: source-tree, installed-tree, missing-resource and moved-root
tests; then actual Studio interaction on Windows and Linux.

### Persistent learning and reusable developer workflows

Connect the existing progress and assessment contracts to saved exercise results.
Keep compiler output, failed attempts, corrected submissions and recovery
observable through Framework services. Acceptance: save/reopen and failed-write
checks, no application-local progress database, and runnable C/Assembly examples.
This is outstanding work, not functionality claimed by a static lesson page.

### Workbench, SDK and application qualification

Exercise the shared editor, layouts, save/reopen, shutdown and recovery through
Studio and Trader as separate consumers. Build the GTK-enabled SDK and a separate
Framework window application from an installed prefix. Acceptance: native click,
resize, close, restart and resource-resolution evidence; full application build
and test results must not be replaced by small source fixtures.

### Packaging and release promotion

Run independent Windows UCRT64 and Linux builds, the complete relevant test suite,
clean install/uninstall and smoke checks. Check naming inventory, link integrity,
licence/author preservation, API compatibility and reproducible package hashes.
Select a release version only after the actual existing version policy and
acceptance evidence have been reviewed. Push tested child main commits, update
the native repository lock and publish the parent last. No forced pushes.

## Deliberate exclusions

The naming audit is targeted, not a claim that every path in every product or
all historical books has been examined. Product names, public C interfaces,
Master/Slave terminology, third-party API names, published book titles and
persisted identifiers are not blindly rewritten. Remaining candidates need a
path/reference inventory before any structural change.

This batch does not change broker permissions, live-order arming or risk pricing.
The existing 1.0 fallback for a request without a positive limit price is not a
market-data valuation policy and remains a separate trading qualification issue.

## Evidence to attach before promotion

Record source revision, command, compiler, operating system, exit status,
registered test count, failures/skips and known limitations. Keep baseline
reproductions distinct from corrected-build passes. A completed Git push is not
by itself evidence of a complete application build or runtime qualification.
