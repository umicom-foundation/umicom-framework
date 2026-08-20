# Application Runtime, Launcher and Umicom Desk

The application runtime connects the declarative application portfolio to a
user-facing Umicom Desk.

## Authority

The Master Controller owns:

- application installation and compatibility state;
- enabled and visible state;
- pinning;
- process identity;
- running, failed and attention state;
- active application;
- launch, activate, restart and stop requests.

A frontend adapter renders copied snapshots. It does not scan folders or execute
an arbitrary file because it appears below `applications/`.

## Flow

```text
Validated application identity
        ↓
Runtime catalogue
        ↓
Taskbar strip
        ↓
Semantic launch request
        ↓
Framework launcher
        ↓
Platform launch adapter
        ↓
Supervised application process
```

## Repository boundary

`umicom-desktop-module` is a thin product composition. It owns the Umicom Desk
entry point and its default layout. The taskbar model, launcher contracts,
semantic layout runtime, shared resources and GTK4 rendering adapter remain in
Umicom Framework.

`umicom-os-module` is a thin user-space Control Centre. Linux kernel source,
bootloaders, drivers, root-filesystem construction, recovery and image
generation remain in the full `umicom-os` repository.

## Threading

The initial catalogue and launcher are serialised by the owning Master
Controller. Frontends should marshal requests onto that controller's thread.
A later out-of-process launcher may retain the same public contract.
