# SVG-First Application Branding

Umicom Framework keeps one dependable visual identity for every application.
The icon shown in a menu, a splash screen and a high-density display should all
come from the same artwork instead of separate hand-edited pictures.

## Source artwork and generated output

The four SVG files in `resources/brand` are the source artwork:

- `umicom-icon.svg` is the mark for a light surface;
- `umicom-icon-on-dark.svg` is the mark for a dark surface;
- `umicom-logo.svg` is the larger wordmark for a light surface; and
- `umicom-logo-on-dark.svg` is the larger wordmark for a dark surface.

These files are transparent. The surface behind the image remains visible
through the open parts of the mark and letters. A white rectangle or a white
centre inside a letter is not part of the logo.

PNG and ICO files are derived outputs. A derived file is made from a source
file for a specific technical boundary. It is not a second design and should
never be edited independently.

## Why the Windows icon remains

Windows uses an ICO container for the executable, taskbar and installer. The
container holds several raster sizes so the operating system can choose a
suitable one. The shared CMake helper embeds this file only on Windows. The
application interface still uses SVG.

## How an application receives branding

The application creates its executable and calls
`umicom_apply_application_branding`. It supplies its real product name,
application identifier and version. Framework then:

1. checks the shared SVG masters;
2. copies the light and dark vectors beside the executable;
3. embeds the Windows icon and version details when building for Windows;
4. creates a desktop entry when building for a supported desktop system; and
5. installs the shared vector resources with the application.

Product names remain normal interface text. For example, a title bar combines
the `<>` mark with text supplied by the application. This keeps the title
sharp, readable by accessibility software and suitable for translation.

The [Application Identity and Chrome](APPLICATION_IDENTITY_AND_CHROME.md)
guide explains the shared native header, appearance-change callback, resource
lookup rules and controller lifetime used by application workstations.

## Supporting an older image loader

An older frontend may be unable to display SVG. Its CMake call can add the
`RASTER_FALLBACKS` option. Framework then checks and stages the derived PNG
files for that target. This option should be removed when the frontend learns
to consume SVG.

Do not enable raster fallbacks for a new application. Start with the vector
path and keep any format conversion at the packaging boundary.

## Updating the identity safely

Change the SVG source, inspect it on both light and dark surfaces, and confirm
that it remains clear at small and large sizes. Regenerate the platform assets
after the vector review. This order ensures that every derived file represents
the approved source instead of an older copy.
