# Native Umicom Command Reference

The native `umicom` executable is the normal entry point for development.  It
constructs the required environment for child processes and therefore avoids a
normal dependency on unsigned PowerShell bootstrap scripts.

## Environment commands

```text
umicom check [--gtk] [--github]
umicom doctor [--gtk] [--github]
umicom env
umicom repair [--dry-run]
umicom shell
```

`check` discovers tools and libraries and performs compile-link-run probes.
`repair` generates local CMake user presets and recovers stale build caches.
`shell` starts a child interactive shell with the validated environment.

## Build commands

```text
umicom configure [--source PATH] [--preset NAME | --build PATH]
umicom build     [--source PATH] [--preset NAME | --build PATH]
umicom test      [--source PATH] [--preset NAME | --build PATH] [--tests REGEX]
umicom automate plan [PATH] [--all]
umicom automate run  [PATH] [--preset NAME | --build PATH]
                     [--jobs N] [--no-tests] [--deploy] [--prefix PATH]
umicom automate watch [PATH] [--preset NAME | --build PATH]
                      [--jobs N] [--debounce MS] [--interval MS]
                      [--no-tests] [--deploy] [--prefix PATH]
                      [--ignore-existing] [--all]
umicom make      [--source PATH] [--preset NAME | --build PATH]
umicom run studio [--source PATH] [--preset NAME]
umicom run --executable PATH [--source PATH] [--preset NAME]
```

`make` performs check, configure, build, and test as one fail-fast operation.

`automate plan` reads changed paths and prints affected product and test scopes
without compiling. `automate run` builds those scopes and their focused tests.
`automate watch` continuously discovers edits, verifies affected scopes and
records durable module-update notices. It installs only when `--deploy` is
present. The complete planning rules and
Framework API are described in [Automated Build System](automated-build-system.md).

## Governed workflow commands

```text
umicom workflow plan [options]
umicom workflow verify [options]
umicom workflow build [options]
umicom workflow test [options]
umicom workflow publish --message MESSAGE [options]
umicom workflow update [options]
umicom workflow complete --message MESSAGE [options]
```

Common options are `--source PATH`, `--preset NAME`, `--jobs N`, `--strict`,
`--dry-run`, `--remote NAME` and `--branch NAME`. Verification runs before
configure, compile and link. Strict mode requires the external dependency CVE
scanner instead of recording it as skipped.

`workflow plan` accepts `--goal verify|build|test|publish|update|complete` so a
developer can inspect the exact stages without changing local or remote state.

## Quality and dependency commands

```text
umicom quality scan [PATH] [--profile default|ci|security|architecture]
umicom security scan [PATH]
umicom architecture check [PATH]
umicom memory scan [PATH]
umicom dependencies inventory [PATH]
umicom dependencies audit [PATH] [--strict] [--dry-run]
```

CodeGuard provides local source analysis. Dependency CVE matching delegates to
a validated OSV-Scanner executable and never claims success when that scanner
is unavailable.

## Repository command

```text
umicom create repo "Umicom Designer" [options]
umicom new repo "Umicom Trader" [options]
```

Important options:

```text
--parent PATH
--repo-name NAME
--id APPLICATION_ID
--frontends console,gtk,web
--console
--gtk
--web
--owner GITHUB_OWNER
--remote
--public
--private
--push
--dry-run
--framework-url URL
--framework-branch NAME
--framework-installed
--framework-vendored
--no-git
--no-commit
```

The command creates the standard repository structure, application manifest,
CMake configuration, tests, documentation, licence, suite manifest, and selected
frontends.  Remote GitHub creation is attempted only when explicitly requested
and after `gh` has been discovered and validated.

Safe maintenance commands are:

```text
umicom repo status [PATH]
umicom repo audit [PATH] [--summary]
umicom repo verify [PATH]
umicom repo update [PATH] [--remote NAME] [--branch NAME]
umicom repo stage [PATH]
umicom repo add [PATH]        # Friendly alias for stage
umicom repo commit [PATH] (--message MESSAGE | --auto-message)
umicom repo push [PATH]
umicom repo publish [PATH] (--message MESSAGE | --auto-message)
```

`repo audit` is read-only. It uses the same Framework CodeGuard service as
Studio to report duplicate source, unsafe patterns, and version- or
batch-labelled filenames. It never renames or deletes files automatically.

Update requires a clean tracked worktree and applies only a fast-forward. It
does not rebase, reset or create an automatic merge commit.

`--auto-message` derives a conservative conventional commit message from the
complete staged path list. Generation is local, does not inspect source
contents, and rejects truncated input instead of guessing. A command accepts
either a reviewed manual message or `--auto-message`, never both.

## Catalogue and suite commands

```text
umicom capabilities
umicom suite [MANIFEST]
```

The catalogue reports implemented, foundation, and planned Framework
capabilities.  The suite command validates a suite manifest that composes
independent applications over one Framework contract set.
