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
