# Brand and Responsive Workbench

This guide explains how Umicom applications present a consistent identity and
how their work areas stay usable on different screen sizes. The rules belong to
Umicom Framework so a client application does not invent another title bar,
palette or panel system.

## Application identity

An application menu uses two separate parts:

1. the Umicom `<>` icon from the Framework resource catalogue; and
2. a real text label, such as **Umicom Studio IDE** or **Umicom Trader**.

The product name is text rather than a picture. This lets the operating system
scale it clearly, lets assistive technology read it and allows future
translations without editing an image.

Use `umicom-icon.svg` on light surfaces and `umicom-icon-on-dark.svg` on dark
surfaces. Complete wordmark images are intended for larger presentation areas,
not compact menu bars. All variants have transparent backgrounds, including
the open parts inside the icon and letters.

SVG is the editable master format. It scales without becoming blurred and lets
one asset serve different screen densities. PNG and ICO files are derived only
for a platform boundary that cannot consume SVG. An application must never
choose a raster copy merely because it is convenient to hard-code.

The executable and the window must use the same mark. A graphical application
calls `umicom_apply_application_branding` for its executable target. The helper
embeds the multi-size Windows icon, creates the installed desktop identity and
places contrast-aware vectors beside the executable. This prevents a toolkit
or operating-system fallback icon from replacing Umicom branding.

The helper accepts `RASTER_FALLBACKS` for an older frontend that genuinely
cannot load SVG. This option is deliberately explicit. Current Umicom GTK and
web surfaces use the vector files and do not request it.

See [SVG-First Application Branding](SVG_FIRST_APPLICATION_BRANDING.md) for the
asset ownership rules and the steps used when adding another application.

## Semantic colours

`UmiUiBrandPalette` gives each colour a purpose:

- `background` is the area behind the complete workbench;
- `surface` holds menus, panels and cards;
- `surface_raised` separates toolbars and active containers;
- `editor` keeps the main document area calm;
- `foreground` and `foreground_muted` provide readable text levels;
- `accent` identifies selection, focus and active controls;
- `success`, `warning` and `danger` explain state without changing layout.

Applications request the light, dark or high-contrast palette. They should not
copy hexadecimal values into application-specific business logic. A frontend
may translate the semantic values into its own toolkit's styling language.

The shared appearance catalogue also provides retro and neo presets plus an
editable custom profile. The Appearance control lets a user choose interface
and content fonts, text scale, compact or spacious density, and every semantic
colour. These are presentation preferences only. They never change orders,
documents, projects, account data or other application state.

## A workbench that fits

A saved layout may have been created on a large monitor and later opened on a
small laptop. The frontend must keep a useful central editor before assigning
space to side and bottom panels. Stored user sizes remain unchanged; the
visible splitter positions are bounded to the space available now.

Side panels may shrink to their readable minimum. The welcome page uses a flow
layout, so three cards become two or one column as the window narrows. Long menu
strips stay inside a horizontal viewport instead of increasing the minimum
window width.

## Layout editing

The normal workspace is locked. Locking prevents an accidental drag or click
from changing a carefully arranged screen. The user can enter layout editing
mode to:

- resize a panel with a splitter;
- move it to another dock region;
- combine related panels in a tab stack;
- open or close panels from the window catalogue;
- float a panel into its own native window;
- assign a linked context group and colour;
- cancel the current edit or save it as a named layout.

The Framework owns these operations and their persistent state. Studio, Trader
and other applications provide panel content and suitable default layouts, but
they do not duplicate docking rules.

## Colour-linked groups

A narrow coloured stripe in a panel header shows which linked context group it
belongs to. For example, panels in one group can follow the same project,
instrument, customer or document selection. Colour is an extra visual clue;
the group name and accessible descriptions remain the source of meaning.

Only approved semantic colour tokens become CSS classes. Arbitrary text loaded
from a layout cannot be inserted into a stylesheet. This keeps custom layouts
flexible without turning stored names into executable presentation rules.

## Adding a new application

A new application should:

1. register its product name and logical icon resource, then apply the shared
   executable-branding helper;
2. choose a Framework appearance profile;
3. declare its panel catalogue and default placements;
4. reuse the shared panel frame, window catalogue and layout editor;
5. store business state inside its own services, not inside GTK widgets;
6. test that its smallest supported window still keeps the central work area
   reachable.

This separation keeps applications thin. Improving a shared menu, palette,
panel action or layout rule in Framework improves every application that uses
the contract.
