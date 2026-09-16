# Source Naming and Duplicate Governance

Every implementation needs one clear home. If two files contain the same code,
a new developer cannot easily tell which file should be changed. The problem
becomes worse when filenames contain labels such as `v2`, `v3`, or a development
batch number.

Git already keeps the history of every committed file. A second file is not
needed merely to preserve an earlier version.

## The simple rule

Use one descriptive filename without a version or development batch number.

Good examples:

- `workspace_layout.c`
- `security_secret_inventory.c`
- `source_naming.c`

Avoid examples such as:

- `workspace_layout_v3.c`
- `security_secret_inventory_v2.c`
- `BATCH_24_WORKSPACE_LAYOUT.md`
- `command_reference_0.5.0.md`

If a newer file contains useful improvements, merge those improvements into the
canonical file. Review the result, update its tests, and only then remove the
parallel copy. Never delete unique behaviour merely to make filenames tidy.

## How CodeGuard helps

CodeGuard now checks filenames while it walks a repository. It reports:

- `CODEGUARD-NAME-VERSION-001` for version-labelled filenames;
- `CODEGUARD-NAME-BATCH-001` for batch-labelled filenames;
- existing duplication findings when two source files contain the same logic.

The check does not rename or delete anything. It provides evidence so a human
can decide how to consolidate the files safely.

## Intentional workspace templates

The complete workspace template contains standalone Framework files because the
scaffolding command copies that template to create a usable new project. These
files are template payloads, not competing runtime implementations. They remain
separate so a generated workspace does not depend on the source repository.

## Name the responsibility, not its first consumer

`trading` is a reusable domain; `Trader` is a product. Shared order validation
belongs in Framework trading code. A shared regression suite is
`tests/input_validation`, not a suite named after whichever applications first
reported its defects. Product profiles, application composition, vendor adapters
and genuinely product-specific integration tests may keep their real names.
Do not mechanically replace every occurrence of Studio, Trader, GTK4 or IBKR.

The retired `tests/studio_trader_contracts` suite is now `tests/input_validation`.
The target is `umicom-input-validation-tests` and its CTest prefix is
`framework.input_validation.`. All 14 existing regression groups remain. Update
source paths, standalone build commands and CTest selectors together. The
Applications `tests/CMakeLists.txt` registers the new path. No public C API is
renamed by this migration.

## Additional CodeGuard findings

The existing source-name classifier and scanner now share these additional
rules. No second naming implementation is installed in an application.

| Rule | Meaning | Deliberate limit |
|---|---|---|
| `CODEGUARD-NAME-CONSUMER-001` | A whole path component uses the specifically retired `studio_trader_contracts` name | Not a blanket ban on application names; semantic ownership still needs review |
| `CODEGUARD-NAME-COPY-001` | The filename ends in a numbered copy marker before its final extension, such as `guide(1).md` | Not evidence that the contents are identical; inspect provenance and unique material |
| `CODEGUARD-NAME-PORTABILITY-001` | The final filename contains selected ASCII Windows conflicts, reserved device names, or a trailing dot/space | Not a complete filesystem validator; Unicode normalisation, case collisions and whole-path length are not checked |

The original two rule IDs, public functions and existing enum values remain.
New enum values are appended. An empty audit path is invalid input. A finding
whose path cannot fit the existing diagnostic record returns
`UMI_STATUS_CAPACITY_EXCEEDED` instead of truncating the evidence. Classification
of a null or empty string retains its original no-finding behaviour.

These rules are read-only. They neither move files nor infer authorisation to
remove them. Versioned protocol names, release artefacts, immutable source
archives and externally maintained material need scope-aware human review.
Numbered educational volumes are not development-version duplicates.

## Lesson identity is different from its filename

The `foundations.git` lesson now uses `docs/learning/git-first-commit.html`.
Its title, position, exercise and saved identifier stay unchanged. The old
`git-first-comment.html` is a small compatibility link, not a second lesson.
Current navigation and the curriculum point at the canonical page.

Likewise `foundations.branch` and `foundations.pull-request` remain stable keys
for existing progress. Their visible titles teach working on main and publishing
a reviewed change. Do not reset progress to make an internal key prettier.

The native resource test checks real head metadata, the catalogue title against
both the HTML title and h1, unique local anchors, and same-directory local
navigation. Comments and raw script/style contents cannot supply a missing
identity. This is the authored lesson subset, not general HTML5 validation.
External links are not fetched, and scripts are not executed.

## Change and publication sequence

1. Record the repository revision and inspect the current source and callers.
2. Classify the name as a capability, product identity, persisted key, external
   interface, compatibility name or historical record.
3. Move files deliberately; update every in-scope reference in the same change.
4. Preserve features, tests, comments, author credits and unique lesson content.
5. Run focused tests and then the affected real application builds.
6. Publish changed child repositories on main, update the native repository lock,
   and publish the parent last. Do not create empty application commits.

For the known migration and build commands, see the Applications guide
`docs/getting-started/FRAMEWORK_QUALIFICATION.html`. Release gates are tracked in
`docs/RELEASE_QUALIFICATION_ROADMAP.md` in Framework.

Reference for Windows filename restrictions:
https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file
