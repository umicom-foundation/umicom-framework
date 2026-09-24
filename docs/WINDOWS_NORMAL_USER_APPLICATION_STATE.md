# Normal-user application state and Windows startup

Umicom desktop applications are designed to run as the signed-in user. Opening an application from Windows Explorer, a desktop shortcut or the Start menu must not require an Administrator prompt, a developer terminal or a writable installation directory.

## Where writable state belongs

The executable, DLLs, GTK resources and other packaged files belong to the installation. User settings and runtime state belong to the user.

Framework now exposes `UmiApplicationPathsConfigDefault`, `UmiApplicationPathsResolve` and `UmiApplicationPathsPrepare` for this purpose. On Windows, the normal root is below `%LOCALAPPDATA%`:

```text
%LOCALAPPDATA%\Umicom\<Application>\
    config\
    state\
        recovery\
    cache\
    data\
    logs\
```

An application can use an absolute `baseOverride` in an isolated test or controlled portable developer run. Relative overrides are rejected so a launch directory can never silently become the application-data location.

On Unix-like systems the same contract uses the XDG configuration, data, state and cache locations, with the conventional directories below `HOME` when an XDG variable is not set.

## Why this is separate from a workspace

An IDE workspace, a trading layout and a bank user's data are not installation folders. They are also not interchangeable with application settings and caches.

A product should therefore keep these concepts separate:

- installation: executable and read-only packaged resources;
- user application state: settings, saved sessions, recovery, cache and logs;
- workspace or business data: locations selected through the product's own workflow.

A product may remember the user's last workspace, but the Windows process working directory is not a workspace-selection mechanism.

## Using the Framework contract

```c
UmiApplicationPathsConfig config =
    UmiApplicationPathsConfigDefault("Studio");
UmiApplicationPaths paths;
UmiStatus status = UmiApplicationPathsResolve(&config, &paths);

if (status == UMI_STATUS_OK) {
    status = UmiApplicationPathsPrepare(&paths);
}
```

The public values are bounded UTF-8 paths. Applications keep product-specific filenames and persistence formats while Framework owns the operating-system location policy.

## Windows packaging relationship

The existing Framework Windows deployment system remains responsible for executable DLLs, GTK data, GtkSourceView data, schemas, loaders and installer payload ownership. The per-user path contract solves a different problem: where a running application may write.

Neither mechanism changes the system `PATH`, requires MSYS2 on an end-user machine or grants Administrator rights to the application.

## Release check

For a Windows release candidate:

1. Build and stage the complete private runtime.
2. Start each graphical application from Explorer as a normal user.
3. Start the installed shortcut as a normal user.
4. Confirm no project or application uses `System32`, the installer directory or another launch folder as writable state merely because it was the process working directory.
5. Confirm the application can save and reopen its intended user state.
6. Run the clean install/uninstall and portable-package checks before publishing a release.
