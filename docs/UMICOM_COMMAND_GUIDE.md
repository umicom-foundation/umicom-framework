<!--
Umicom Framework
File: docs/UMICOM_COMMAND_GUIDE.md

PURPOSE:
Teach developers how to use the native Umicom command safely.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Umicom Command Guide

Author: Sammy Hegab

Organisation: Umicom Foundation

Licence: MIT

## What the Umicom command does

The `umicom` command is a beginner-friendly doorway into Umicom Framework.
Instead of remembering long PowerShell, Git and CMake command sequences, a
developer can ask one command to validate the environment and perform one
clearly named operation.

The command owns the Umicom development workflow. Git, CMake, CTest and optional
analysis tools remain the trusted engines underneath it. `umicom` calls those
engines with explicit argument lists, checks their exit codes and explains
failures in Umicom language. Developers normally use `umicom`; direct engine
commands are recovery and diagnosis tools.

## The one-time Windows bootstrap still has a job

A new computer cannot run `umicom.exe` before Umicom has been downloaded and
built. The Windows bootstrap therefore remains responsible for the first step:

- install Git, GitHub CLI and MSYS2 when the developer asks it to;
- install the canonical UCRT64 packages through `pacman`;
- add an idempotent helper block to the developer's PowerShell profile;
- optionally authenticate GitHub in a browser; and
- optionally clone and build the first workspace.

The native command takes over after that first build. It deliberately does not
run `winget`, update all MSYS2 packages, edit PowerShell profiles or start an
authentication browser. Those are operating-system setup decisions and should
remain visible to the developer.

## Make the command discoverable on Windows

PowerShell reports that `umicom` is not recognised when the directory that
contains `umicom.exe` is absent from `PATH`. A successful source build creates
the program but does not change the terminal's search path.

The command can always be called by its explicit path:

```powershell
& "C:\umicom\umicom-applications\build\windows-ucrt64-debug\bin\umicom.exe" `
    version
```

For the current PowerShell window only, prepend the build directory and then
confirm which executable will run:

```powershell
$env:Path = "C:\umicom\umicom-applications\build\windows-ucrt64-debug\bin;$env:Path"
Get-Command umicom
```

Closing that PowerShell window restores its earlier environment. The Windows
installer provides an explicit option to add the installed `bin` directory to
`PATH`; a newly opened terminal then finds the installed command. This option
is also removed by the uninstaller.

## Check the complete Windows development environment

From the Umicom Applications directory:

```powershell
& ".\build\windows-ucrt64-debug\bin\umicom.exe" check `
    --all `
    --project "C:\umicom\umicom-applications"
```

This reports:

- the selected toolchain and its root directory;
- the path and version of every discovered tool;
- GCC or Clang compile, link and runtime probes;
- MSYS2 Bash on Windows;
- the project's `CMakePresets.json` file;
- GTK4, GLib, GtkSourceView, JSON-GLib, LibSoup, Curl and SQLite versions; and
- whether GTK4 satisfies the Framework minimum of version 4.10.

Use a smaller check for headless Framework work:

```powershell
umicom check --project "C:\umicom\umicom-applications"
```

Useful variations:

```powershell
umicom check --gtk
umicom check --github
umicom check --all
```

`--github` verifies that the GitHub CLI executable is available. It does not
open a browser or change authentication settings.

## Developer lifecycle commands

The short commands remain available:

```powershell
umicom configure --preset windows-ucrt64-debug
umicom build --preset windows-ucrt64-debug --jobs 2
umicom test --preset windows-ucrt64-debug
umicom install --preset windows-ucrt64-debug `
    --prefix "C:\umicom\install\umicom-applications"
umicom package --preset windows-ucrt64-debug --jobs 2
umicom run desk --preset windows-ucrt64-debug
umicom run studio --preset windows-ucrt64-debug
```

The same operations can be grouped under `dev`, which can be easier to
discover:

```powershell
umicom dev doctor --all --project "C:\umicom\umicom-applications"
umicom dev configure --preset windows-ucrt64-debug
umicom dev build --preset windows-ucrt64-debug --jobs 2
umicom dev test --preset windows-ucrt64-debug
umicom dev clean --preset windows-ucrt64-debug
umicom dev install --preset windows-ucrt64-debug
umicom dev package --preset windows-ucrt64-debug
umicom dev run studio --preset windows-ucrt64-debug
```

`deliver` composes configure, build, test, install and package. Use it only when
you intend to run the complete delivery sequence.

## Build only what changed

The Automated Build System removes the need to name a product or test group.
First inspect its read-only plan:

```powershell
umicom automate plan "C:\umicom\umicom-applications"
```

Then ask it to build affected products and run their focused tests:

```powershell
umicom automate run "C:\umicom\umicom-applications" `
    --preset windows-ucrt64-debug `
    --jobs 2
```

To keep discovering and verifying saved changes without naming modules, start
one continuous workspace controller:

```powershell
umicom automate watch "C:\umicom\umicom-applications" `
    --preset windows-ucrt64-debug `
    --jobs 2
```

The controller builds and tests only affected scopes, then writes a durable
versioned update notice that running Umicom applications can discover. Press
Ctrl+C when you want it to stop after the current safe operation.

By default, changed source is checked after ten quiet minutes. An unchanged,
approved revision builds twenty minutes later, while a sixty-minute watchdog
prevents ordinary pending work from being forgotten. View the effective values:

```powershell
umicom automate settings "C:\umicom\umicom-applications"
```

Developers can change the ignored local
`.umicom/automation.conf` file. For example,
`build_interval_minutes=480` holds approved changes for an eight-hour
controller interval. `automatic_deploy=true` permits installation only after
the build and focused executable tests pass.

To finish sooner, wake the running controller:

```powershell
umicom automate trigger "C:\umicom\umicom-applications"
```

This cancels the waiting periods but still runs source verification,
incremental compilation and executable tests.

Application manifests supply the product catalogue. A source change normally
selects one application. A shared Framework change selects its application
consumers, and a root build-definition change selects the configured workspace.
Documentation-only changes do not start compilation.

Use `--no-tests` only for a temporary compilation diagnosis. Use `--all` when a
complete configured-product and test pass is intentional. Installation is never
implicit; add `--deploy` and an optional `--prefix` only when build and test
success should be followed by installation.

`clean` removes generated content from the selected build directory. It does
not remove source files, but you should still confirm the selected preset or
`--build` path before running it.

## Use the governed workflow

The workflow command puts checks before compilation and publication:

```powershell
umicom workflow plan `
    --source "C:\umicom\umicom-applications" `
    --goal complete `
    --preset windows-ucrt64-debug `
    --strict

umicom workflow verify `
    --source "C:\umicom\umicom-applications" `
    --strict

umicom workflow build `
    --source "C:\umicom\umicom-applications" `
    --preset windows-ucrt64-debug `
    --jobs 2

umicom workflow test `
    --source "C:\umicom\umicom-applications" `
    --preset windows-ucrt64-debug `
    --jobs 2
```

`workflow build` runs environment, native dependency, source-quality, security,
architecture, static memory-risk and dependency-vulnerability gates before it
configures, compiles and links. `workflow test` adds CTest after the build.

`--strict` requires the optional external dependency-vulnerability scanner. A
normal run clearly reports that this stage was skipped when no scanner exists;
it never labels an unperformed CVE audit as passing.

Use `workflow complete` only when one repository is ready to be verified,
built, tested, committed and pushed:

```powershell
umicom workflow complete `
    --source "C:\umicom\umicom-applications\framework" `
    --preset windows-ucrt64-debug `
    --jobs 2 `
    --message "feat(framework): add governed developer lifecycle"
```

The plan is a Framework contract, so Studio can later display the same stages
in its task and release panels.

## Scan code and dependencies

CodeGuard is built into Framework:

```powershell
umicom quality scan . --profile ci
umicom security scan . --format sarif --output code-security.sarif
umicom architecture check .
umicom memory scan .
```

`memory scan` is static analysis. It finds suspicious ownership, allocation,
buffer, string and resource-lifetime patterns without running the application.
Dynamic memory proof still requires a sanitizer-instrumented build and tests.
AddressSanitizer must be enabled during both compilation and linking; it is a
test tool and must not be shipped inside production executables. See the
[official Clang AddressSanitizer guide](https://clang.llvm.org/docs/AddressSanitizer.html).

Inventory native libraries and their selected versions:

```powershell
umicom dependencies inventory "C:\umicom\umicom-applications"
```

Audit dependency manifests and C/C++ submodules with OSV-Scanner:

```powershell
umicom dependencies audit "C:\umicom\umicom-applications" --strict
```

OSV-Scanner supports recursive source scanning and can inspect C/C++ submodule
and vendored commit information. See the
[official OSV-Scanner source-scanning guide](https://google.github.io/osv-scanner/usage/)
and [supported manifests](https://google.github.io/osv-scanner/supported-languages-and-lockfiles/).

The CLI delegates this work to CodeGuard's reusable dependency-audit contract.
Studio's future Security Centre can therefore show the same scanner path,
version, output and honest `planned`, `skipped`, `passed` or `failed` outcome
without reimplementing process execution.

## Clone a repository

Cloning includes submodules by default because Umicom Applications is a
superproject:

```powershell
umicom repo clone `
    "https://github.com/umicom-foundation/umicom-applications.git" `
    "umicom-applications" `
    --root "C:\umicom"
```

Plan it first without creating files:

```powershell
umicom repo clone `
    "https://github.com/umicom-foundation/umicom-applications.git" `
    "umicom-applications" `
    --root "C:\umicom" `
    --dry-run
```

Optional clone controls are `--branch NAME`, `--depth NUMBER` and
`--no-recursive`.

## Create a new Umicom application repository

The established scaffold command creates the application structure, manifests,
tests, CMake files and Framework link:

```powershell
umicom repo create "Umicom Designer" `
    --parent "C:\umicom" `
    --repo-name "umicom-designer" `
    --id "org.umicom.designer" `
    --frontends console,gtk `
    --dry-run
```

Remove `--dry-run` only after reviewing the plan. The older spelling remains
available for compatibility:

```powershell
umicom create repo "Umicom Designer" [options]
```

## Initialise a plain Git repository

Use this when the files already exist and only Git ownership is missing:

```powershell
umicom repo init "C:\umicom\my-project" --branch main --dry-run
umicom repo init "C:\umicom\my-project" --branch main
```

An optional remote can be added during initialisation:

```powershell
umicom repo init "C:\umicom\my-project" `
    --branch main `
    --remote origin `
    --remote-url "https://github.com/example/my-project.git"
```

This creates a local Git repository. It does not create a GitHub repository.
Use the established application scaffold `--remote` option when GitHub CLI
remote creation is required.

## Add a submodule

```powershell
umicom repo submodule add `
    "https://github.com/umicom-foundation/umicom-framework.git" `
    "framework" `
    --root "C:\umicom\my-project" `
    --branch main `
    --dry-run
```

After reviewing the plan, repeat without `--dry-run`. Git updates both the
submodule directory and `.gitmodules`.

## Stage, commit and push separately

These commands mirror familiar Git operations:

```powershell
Set-Location "C:\umicom\umicom-applications\framework"

umicom add -A
umicom status
umicom commit -m "feat(framework): describe the completed change"
umicom push
```

The short forms above operate on the current directory, just like Git. They
are friendly aliases for the governed `umicom repo` commands, so their safety
checks are not bypassed. A developer may still supply a repository path and
the longer option names when scripting:

```powershell
umicom repo stage "C:\umicom\my-project"
# `repo add` is a friendly alias for `repo stage`.

umicom repo commit "C:\umicom\my-project" `
    --message "feat(project): describe the completed change"

umicom repo commit "C:\umicom\my-project" --auto-message

umicom repo push "C:\umicom\my-project" `
    --remote origin `
    --branch main
```

`stage` uses `git add -A`. `commit` first runs `git diff --cached --check` and
skips an empty commit. `push` performs a normal push and never force-pushes.

If PowerShell says that `umicom` itself is not recognised, use the explicit
path or current-session `PATH` instructions above. If `umicom` runs but reports
that `add`, `commit` or `--auto-message` is unknown, the terminal has found an
older executable; `Get-Command umicom` shows which file needs to be replaced.

## Publish one repository safely

`publish` is the native equivalent of the reusable PowerShell
`Publish-Repository` function:

```powershell
umicom repo publish "C:\umicom\umicom-applications\framework" `
    --message "feat(framework): add developer environment and repository workflows" `
    --remote origin `
    --branch main
```

When a reviewed message has not been supplied, the command can derive a
conservative message locally from the staged path names:

```powershell
umicom repo publish "C:\umicom\umicom-applications\framework" `
    --auto-message `
    --remote origin `
    --branch main
```

Automatic generation does not read file contents or contact an online service.
It selects a `docs`, `test`, `build` or `feat` prefix only when the complete
staged path list supports that classification, and it rejects a partial path
list instead of guessing. Supply either `--message` or `--auto-message`, never
both.

It performs these steps in order:

1. `git add -A`
2. `git status --short --branch`
3. `git diff --cached --quiet`
4. `git diff --cached --check` when a commit is needed
5. `git commit -m MESSAGE` when staged content exists
6. `git push REMOTE BRANCH`

If no new content exists, the empty commit is skipped but already-created local
commits can still be pushed.

Prefer a reviewed manual message when it can explain a specific capability,
fix or user-visible result. Use `--auto-message` for a safe, general message
when the staged paths are the only available evidence. Neither form uses a
release or batch number as a substitute for purpose.

## Publish submodules before the parent repository

Git stores each submodule as an exact commit pointer. Publish the repository
that owns the source code first. Update the parent pointer last.

Example for a Framework change:

```powershell
umicom repo publish "C:\umicom\umicom-applications\framework" `
    --message "feat(framework): describe the completed Framework change"

umicom repo lock "C:\umicom\umicom-applications"

umicom repo publish "C:\umicom\umicom-applications" `
    --message "chore(applications): lock updated Framework revision"
```

Use the same rule for Studio, Trader and every other application module.

## Update safely

Use the Umicom update workflow instead of a manual pull:

```powershell
umicom repo update "C:\umicom\umicom-applications\framework"

umicom workflow update `
    --source "C:\umicom\umicom-applications" `
    --remote origin `
    --branch main
```

Update refuses to continue when tracked files are staged or modified. It
fetches the selected remote branch, applies only a fast-forward update, then
synchronises and initializes submodules. It never rebases, force-resets or
creates an automatic merge commit. Add `--no-submodules` only when the caller
intentionally wants to leave child repositories untouched.

## What happens when `index.lock` exists

Git creates `.git/index.lock` while changing its index. The file protects the
repository from two writers changing it at the same time.

The Umicom workflow reports the lock and stops. It never deletes the file.

1. Wait for a normal Git operation to finish.
2. Close a stuck Git operation in another terminal or IDE.
3. Confirm that no Git process is running.
4. Only then remove a stale lock and repeat the command.

This is safer than automatically deleting a lock that another process still
owns.

## Safety promises

The repository workflow does not:

- reset, clean, rebase or discard source files;
- create an automatic merge commit while updating;
- create empty commits;
- delete Git locks;
- force-push;
- hide a failing child-process exit code; or
- place credentials inside a shell command string.

Start with `--dry-run` whenever you are learning a command or are unsure which
directory it will change.

## Where the supplied Windows material fits

The Windows setup guide, bootstrap script and current-session repair script are
the source references for this command expansion. Their responsibilities are
divided as follows:

| Capability | Native `umicom` command | Windows bootstrap or repair script |
|---|---:|---:|
| Tool paths and versions | Yes | Yes, before the first build |
| Library versions | Yes | Yes, before the first build |
| CMake preset validation | Yes | Yes, before the first build |
| Configure, build and test | Yes | Optional first build |
| Install and package | Yes | PowerShell aliases may delegate |
| Clone and submodules | Yes | Optional first clone |
| Stage, commit and push | Yes | No longer needs a custom function |
| Install Git, GitHub CLI and MSYS2 | No | Yes |
| Run `pacman` updates | No | Yes |
| Edit PowerShell profiles | No | Yes |
| Open GitHub browser authentication | No | Yes |

This boundary keeps the Framework reusable on Windows, Linux and future
platforms while preserving a simple one-time Windows entry point.
