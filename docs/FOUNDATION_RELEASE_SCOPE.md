# Umicom Framework 0.5.0 — Major Foundation Release Scope

## Release purpose

Umicom Framework 0.5.0 is the first consolidated native platform milestone after
the 0.4.x diagnostic and settings slices.  It provides the reusable mechanisms
required to discover a development environment, construct a safe child-process
environment, build and test applications, create complete Umicom repositories,
compose independent applications into suites, and enforce explicit capability
and permission boundaries.

The release is deliberately Framework-first.  Umicom Studio IDE is the reference
consumer, but no public Framework contract is allowed to depend on Studio, GTK4,
a trading product, a treasury product, or another product-specific type.

## Implemented capability families

1. **Base and ABI foundation**
   - Framework semantic version 0.5.0.
   - Framework ABI version 2.
   - Status, result, memory, identifiers, versions, and diagnostics.

2. **Runtime and application composition**
   - Master Controller and Slave Controller lifecycle.
   - Capability registry and canonical capability catalogue.
   - Application manifests and suite manifests.
   - Dependency validation, deterministic startup, and reverse-order shutdown.

3. **Diagnostics and observability foundation**
   - Diagnostic records, fan-out hub, retained store, correlation identifiers,
     and structured console output.

4. **Configuration and settings**
   - Raw key/value configuration.
   - Schema-driven typed settings with validation, defaults, reset, load, and
     save operations.

5. **Platform services**
   - Portable paths, files, directories, recursive removal, copy, rename, and
     text operations.
   - Argument-based child processes, isolated environment blocks, output
     capture, and compatibility wrapper.
   - Wall, monotonic, and deterministic fake clocks.

6. **Security foundation**
   - Capability-based policy rules.
   - Provider-neutral secret lookup with environment-backed reference provider.

7. **Native toolchain and build management**
   - Compiler, CMake, Ninja, pkg-config, Git, GitHub CLI, Java, GTK4, and SQLite
     discovery.
   - Compile-link-run probes.
   - Child-process environment generation.
   - CMake user preset generation.
   - Stale CMake cache detection and recoverable repair.
   - Configure, build, test, make, run, and prepared-shell workflows.

8. **Repository and application generation**
   - Native local Git repository creation.
   - Optional GitHub remote creation through a validated `gh` executable.
   - Complete Framework application scaffolding for console, GTK4, and web
     frontends.
   - Generated tests, manifests, documentation, CMake presets, licence, and
     suite metadata.

9. **Native `umicom` command**
   - `umicom check`
   - `umicom env`
   - `umicom repair`
   - `umicom shell`
   - `umicom configure`
   - `umicom build`
   - `umicom test`
   - `umicom make`
   - `umicom run`
   - `umicom create repo`
   - `umicom new repo`
   - `umicom capabilities`
   - `umicom suite`

## Capability catalogue policy

The catalogue records implemented, foundation, and planned capabilities.  A
planned catalogue entry is not presented as a finished API.  Public headers are
added only when a contract has an implementation, ownership rules, independent
tests, and a realistic consumer.  This prevents empty placeholder interfaces
from becoming accidental ABI commitments.

## Product ownership rule

Framework owns reusable mechanisms.  Products own domain policy, branding,
composition, and product-specific presentation.  GTK4 remains an adapter or
product composition dependency; GTK types do not cross Framework domain
contracts.

## Release acceptance gates

- Framework configures and builds independently with Clang and GCC.
- All Framework tests pass.
- The native `umicom` environment probe passes.
- A complete generated repository configures, builds, and tests.
- Studio builds against the same Framework source.
- Studio Doctor reports no failures.
- Studio GTK4 launches on Windows UCRT64.
- No product-private header is included by another product through Framework.
- No remote push or tag is created until all gates pass.
