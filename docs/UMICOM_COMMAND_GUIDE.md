# Umicom Command Guide

Author: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT

## What the Umicom command does

The `umicom` command is a beginner-friendly doorway into Umicom Framework.
Instead of remembering long PowerShell, Git and CMake command sequences, a
developer can ask one command to validate the environment and perform one
clearly named operation.

The command does not replace Git or CMake. It calls their real executables with
safe argument lists, checks their exit codes and explains failures in Umicom
language.

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

`clean` removes generated content from the selected build directory. It does
not remove source files, but you should still confirm the selected preset or
`--build` path before running it.

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
umicom repo stage "C:\umicom\my-project"

umicom repo commit "C:\umicom\my-project" `
    --message "feat(project): describe the completed change"

umicom repo push "C:\umicom\my-project" `
    --remote origin `
    --branch main
```

`stage` uses `git add -A`. `commit` first runs `git diff --cached --check` and
skips an empty commit. `push` performs a normal push and never force-pushes.

## Publish one repository safely

`publish` is the native equivalent of the reusable PowerShell
`Publish-Repository` function:

```powershell
umicom repo publish "C:\umicom\umicom-applications\framework" `
    --message "feat(framework): add developer environment and repository workflows" `
    --remote origin `
    --branch main
```

It performs these steps in order:

1. `git add -A`
2. `git status --short --branch`
3. `git diff --cached --quiet`
4. `git diff --cached --check` when a commit is needed
5. `git commit -m MESSAGE` when staged content exists
6. `git push REMOTE BRANCH`

If no new content exists, the empty commit is skipped but already-created local
commits can still be pushed.

Always supply a meaningful message. A message should explain the capability,
fix or user-visible result rather than a release number.

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
- pull an existing repository automatically;
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
