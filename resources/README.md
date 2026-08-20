# Umicom Framework Resources

This directory is the source of truth for immutable resources shared by more than one Umicom application.

## Ownership rule

- `brand/` contains common Umicom Foundation trademark resources used across applications.
- `icons/` maps logical application identifiers to frontend theme-icon names.
- `themes/` contains toolkit-neutral colour, density and typography tokens.
- `schemas/` validates resources, application presentation and portable semantic layouts.
- `layouts/` contains generic toolkit-neutral `.umilayout` templates.
- `windows/` contains a reusable Windows resource template; each product supplies its own name, version and executable identity at configure time.

Product-specific illustrations, screenshots, tutorials and application branding remain in the owning `*-module` repository. Boot splash screens, installer artwork, recovery imagery and other resources needed before Umicom Framework starts remain in `umicom-os`.

`application-presentations.json` mirrors the compiled presentation catalogue so build tools, packaging systems and future application installers can inspect taskbar metadata without loading a UI adapter.

Applications request logical identifiers such as:

```text
umicom.brand.logo.primary
umicom.icon.application.studio
umicom.theme.dark.tokens
umicom.layout.mosaic
```

They must not hard-code a developer checkout path.
