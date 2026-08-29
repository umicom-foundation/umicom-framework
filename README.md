# Umicom Framework

Umicom Framework is the reusable C23 application foundation for Umicom Studio
IDE, Umicom Designer, Umicom Trader, Umicom Treasury Management System, Yuma
Media Studio and future Umicom applications.

## Major foundation 0.9.0

This release candidate consolidates the reusable platform instead of publishing
one small release per helper or header.  It provides:

- stable C ABI version 2;
- Master Controller and Slave Controller lifecycle;
- capability registry and canonical capability catalogue;
- application and suite manifests;
- diagnostics, retained logging, typed settings, messaging and Data Server
  foundations;
- portable filesystem and child-process services;
- capability policy and secret-provider foundations;
- native compiler, build-tool, SDK and library discovery;
- compile-link-run validation and isolated child environments;
- CMake cache repair and local user-preset generation;
- native configure, build, test, run and prepared-shell operations;
- local Git and optional GitHub repository creation;
- complete console, GTK4 and web application repository scaffolding;
- a native `umicom` command;
- independent unit, contract, lifecycle, repository and generated-application
  tests.

## Native command

```text
umicom check
umicom check --all --project "C:/umicom/umicom-applications"
umicom env
umicom repair --dry-run
umicom shell
umicom configure
umicom build
umicom test
umicom install
umicom package
umicom make
umicom run desk
umicom run studio
umicom create repo "Umicom Designer" --console --gtk
umicom repo clone URL DESTINATION --root PATH
umicom repo init PATH
umicom repo submodule add URL PATH --root PARENT
umicom repo publish PATH --message "feat(scope): explain the change"
umicom capabilities
umicom suite
```

The native command constructs environments for its child processes.  Normal
Framework development therefore does not require an unsigned PowerShell script
to modify the current shell.

See [docs/UMICOM_COMMAND_GUIDE.md](docs/UMICOM_COMMAND_GUIDE.md) for a
beginner-friendly explanation of environment checks, repository creation,
submodules, safe publishing and the remaining role of the Windows bootstrap.

Inspect the shared panel and window plans after building Framework tools:

```text
umicom-application-presentation validate
umicom-application-presentation list "org.umicom.studio"
umicom-application-presentation show "org.umicom.workspace.studio.standard"
```

Read the [Application Presentation Platform](docs/APPLICATION_PRESENTATION_PLATFORM.md)
for a beginner-friendly explanation, or use the
[quick reference](docs/APPLICATION_PRESENTATION_QUICK_REFERENCE.md) while coding.

## Architecture

Framework public interfaces are under `include/umicom`.  Implementations are
under `src`.  Optional product policy and GTK4 widgets remain outside Framework
Core.  Generated repositories consume Framework through a submodule, installed
package, or explicitly selected bundled copy.

## Release policy

Minor internal corrections remain local.  Version, commit, push, and tag occur
only after the complete major feature train passes all acceptance gates.

## Author and organisation

- Author: Sammy Hegab
- Organisation: Umicom Foundation
- Licence: MIT
