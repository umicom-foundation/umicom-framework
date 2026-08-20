# Umicom Semantic Layout Templates

These portable `.umilayout` files are immutable Framework defaults. They describe logical windows, splits, tab groups and component identifiers; they do not serialise GTK widgets or memory addresses.

Ownership is divided deliberately:

- Framework supplies generic layout schemas and generic templates here.
- Each `*-module` repository supplies its product-specific default layouts.
- The Framework Data Server stores user layouts, revisions, permissions, active sessions and crash-recovery state.
- Portable `.umilayout` files support import, export, source control, comparison and sharing.

The templates in this directory are safe starting points for GTK4, Qt, Wt, native web and headless renderers.
