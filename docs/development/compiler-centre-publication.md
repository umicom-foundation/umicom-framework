![Umicom](../../resources/brand/umicom-logo.svg)

# Keep compiler plans and command records consistent

The compiler centre combines provider registration, project manifests, profiles, artifacts and a compilation database. Its planning operation describes a command; it does not start a compiler.

Previously, a plan could be written to the caller before a later database insertion failed. Replanning an existing source also ignored the duplicate result, leaving an older command in the database. Framework now stages the request and complete command entry before publishing either result.

1. Initialize a centre and register a provider descriptor.
2. Add a valid compiler profile and project manifest through their shared services.
3. Call umi_compiler_centre_plan_unit_in_directory with the project identifier, unit identifier and intended working directory.
4. Check the returned status before using the output plan.
5. Obtain argument views through the plan service and consume exactly argument_count entries.
6. Qualify the described compiler, flags and environment independently before executing the command.

## Publish both owners together

The provider and policy services build into temporary heap storage. If their work fails, the complete live centre and caller output remain unchanged. The database then inserts or replaces the source record through its explicit upsert API. After that succeeds, only copies, pointer rebasing and a checked revision increment remain.

A repeated source now refreshes its command. The database's older add-only API still reports ALREADY_EXISTS; clients choose replacement explicitly. A full database permits replacement of an existing source but refuses a new source. Exhausted database or centre revision refuses the whole planning operation.

This is an operation-level guarantee for serialized callers. It is not crash recovery or a lock-free transaction. Exclude concurrent reads and writes for the complete call. Keep output plans separate from centre memory; an embedded output destination is refused. Input identifiers and working-directory text may borrow from existing centre fields.

## Admit all nested collections

Every active provider, profile, manifest, artifact, cache entry and database record is checked before traversal. A malformed late record prevents an early match from hiding corruption. The checks establish bounded storage; they do not qualify every semantic relationship between profiles, targets and projects.

Adding duplicate profiles or manifests still reports ALREADY_EXISTS before capacity or revision exhaustion. Activation accepts only enabled profiles, and planning now refuses an active profile that has since been disabled. Read-only snapshots remain available at maximum revision.

The new checked snapshot API preserves output on refusal. The older void snapshot API retains its zero-result convention for invalid input or storage. A zero legacy snapshot cannot distinguish an empty centre from refusal, so new clients should use the checked form.

## Make directory ownership explicit

The explicit-directory planning API records the supplied directory in both the returned plan and stored command. It does not resolve paths or inspect the filesystem. Prefer a project directory whose interpretation is stable in your application.

The original plan_unit entry point remains available and delegates with "." for compatibility. That relative convention still depends on the eventual executor's working directory; it is not a portable project location.

Provider fallback selection and option mappings remain existing behavior. A configured provider is not a probed SDK, and an accepted command is not proof of supported flags. The centre does not infer artifact freshness from cache metadata.

Owner-run regressions cover nested corruption, snapshots, alias refusal, capacity, revision exhaustion, receiving-owner argument pointers, explicit directories and repeated-source replacement. The prior centre fixture retains its assertions with large values moved off the process stack. These tests are authored and unexecuted; no compiler, installed consumer or GUI result is claimed.
