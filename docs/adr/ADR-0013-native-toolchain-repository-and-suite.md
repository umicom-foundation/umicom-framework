# ADR-0013: Native toolchain, repository scaffolding, and suite composition

## Status

Accepted for the Framework 0.5.0 major foundation release candidate.

## Context

Windows development previously required a dot-sourced PowerShell file to place
MSYS2 UCRT64 first in `PATH`.  That approach was vulnerable to execution-policy
restrictions and could not change the environment of an already-running parent
shell from a child process.  Repository creation also depended on manual Git and
GitHub CLI sequences, encouraging inconsistent project structures.

## Decision

Implement a dependency-light C23 native `umicom` command and reusable Framework
services that:

1. discover compatible tools and libraries;
2. validate compile, link, and runtime behaviour;
3. construct isolated child-process environments;
4. generate local CMake user presets;
5. detect and recover stale CMake caches;
6. configure, build, test, run, and open a prepared child shell;
7. create complete local repositories;
8. optionally create GitHub remotes after explicit user approval;
9. scaffold console, GTK4, and web frontends from one application manifest; and
10. validate suites of independently runnable applications.

The native tool does not silently modify machine-wide environment variables and
does not silently install packages.  Potentially broad changes require an
explicit command-line request.

## Consequences

- Normal development no longer requires PowerShell execution-policy changes.
- PowerShell remains an emergency fallback during transition.
- Toolchain discovery and repository generation are reusable by Studio, Trader,
  TMS, Designer, and future applications.
- Framework must keep platform and process APIs independent of GTK4.
- Remote repository creation requires an authenticated, validated provider.
- Generated repositories inherit the same Framework architecture rather than a
  copied product implementation.
