# Native Umicom Command Reference — Framework 0.5.0

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
umicom test      [--source PATH] [--preset NAME | --build PATH]
umicom make      [--source PATH] [--preset NAME | --build PATH]
umicom run studio [--source PATH] [--preset NAME]
umicom run --executable PATH [--source PATH] [--preset NAME]
```

`make` performs check, configure, build, and test as one fail-fast operation.

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

## Catalogue and suite commands

```text
umicom capabilities
umicom suite [MANIFEST]
```

The catalogue reports implemented, foundation, and planned Framework
capabilities.  The suite command validates a suite manifest that composes
independent applications over one Framework contract set.
