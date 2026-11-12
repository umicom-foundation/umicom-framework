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

## Native tools with a separate runtime

A tool that uses the Windows API directly can receive the same approved artwork without using the GTK runtime deployment service. Add `NO_RUNTIME_DEPLOYMENT` to its call; this changes only that target. Existing applications keep the default deployment behaviour.

```cmake
include(path/to/framework/cmake/UmicomApplicationBranding.cmake)
umicom_apply_application_branding(
    TARGET my-native-tool
    PRODUCT_NAME "Umicom Native Tool"
    INTERNAL_NAME my-native-tool
    RESOURCE_ROOT "${UMICOM_FRAMEWORK_ROOT}/resources"
    NO_RUNTIME_DEPLOYMENT)
```

Create the executable target first. On Windows, enable CMake’s `RC` resource language before calling the helper. Keep the global deployment option unchanged when other targets still use GTK. The helper stages the four canonical SVG files and embeds the canonical ICO and SVG mark in the Windows executable. It does not give a Win32 static control an SVG renderer. An existing bitmap fallback remains a separate compatibility limitation until that frontend can render vectors.

Use one owner for each resource identifier. The helper owns icon 101 and SVG data 201; a product resource file can keep its manifest and other resources, but must not also define a second icon 101. Boot Media Centre follows this arrangement. Its former setup-copy icon declaration remains disabled for source review.

The `framework.boot_media.native_branding` regression builds a small native target, checks that the GTK bootstrap was not injected, and compares staged SVG bytes with the masters. Windows compilation also exercises the generated ICO/SVG resources. Run the actual product separately to inspect its taskbar icon, scaling and contrast; a resource build is not a visual result.

## Typography belongs to the shared appearance model

Use the approved artwork at its original proportions and choose the vector
variant for the surface behind it. Keep product titles as accessible text.
For interface and code fonts, use Framework appearance roles rather than copying
font rules into each application. A changed font setting must reach the rendered
control and survive the application's supported preference workflow.

Follow [Appearance profiles](ui/APPEARANCE_CENTRE.md) for a C example, native
role mapping, size limits and qualification steps. The default lists request
installed Segoe UI/Inter/Sans for interface text and Cascadia Code/JetBrains
Mono/Monospace for code. No font download or font redistribution is implied.
A successful icon-resource build, a valid profile and a correctly rendered font
are separate observations; inspect each boundary before claiming visual parity.

## Check standalone lessons against the masters

Standalone Framework lessons use the same approved SVG/ICO artwork. Run
`umicom branding audit --assets resources/brand --root docs/learning` from the
Framework root to inspect their controlled source format. The shared native
service checks decoded bytes as well as recorded identities; changing a digest
alone cannot qualify different artwork. Inert retained images do not count as
active logos. The selected asset directory remains a trust input.

Use `data-umicom-guide-surface="light"` with `umicom-logo.svg`; the default or
explicit `dark` variant uses `umicom-logo-on-dark.svg`. Preserve aspect ratio and
use the current `umicom-guide-fonts` block for standalone interface/code roles.
These roles request installed fonts and retain generic fallbacks. Source matching
does not prove visible contrast, font availability or native executable resources.
Follow [the guide identity lesson](learning/check-guide-branding.html) and inspect
the source, published and installed presentations separately.
