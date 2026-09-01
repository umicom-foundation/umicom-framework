# Umicom Brand Resources

This directory contains the visual identity shared by all applications built
with Umicom Framework. The SVG files are the editable source of truth. An
application should request a logical resource from the Framework resource
catalogue instead of copying a file or writing a repository path into its
source code.

## Which file should an application use?

- `umicom-icon.svg` is the `<>` mark for light or neutral surfaces.
- `umicom-icon-on-dark.svg` uses lighter colours on dark surfaces.
- `umicom-logo.svg` combines the mark and dark text for light surfaces.
- `umicom-logo-on-dark.svg` combines the mark and light text for dark surfaces.
- `umicom-icon.png` and `umicom-logo.png` are optional raster fallbacks derived
  from the SVG masters for an older image loader.
- `umicom.ico` is derived from the icon SVG because Windows executable and
  taskbar integration requires an icon container with several image sizes.

Application menus and title bars should normally compose the SVG icon beside a
real text label such as **Umicom Studio IDE** or **Umicom Trader**. Text remains
readable, localisable and accessible when it is rendered by the application.
The complete wordmark is reserved for larger presentation areas.

## Contrast and backgrounds

All artwork has a transparent background. The open spaces inside the letters
and symbol show the surface underneath; they must not be painted white. Choose
the light-surface or dark-surface variant that gives the clearest contrast.
Avoid placing a dark wordmark on a saturated blue panel because the similar
colours make the name difficult to read.

Leave clear space around the mark and keep its proportions unchanged. If an
application needs a smaller identity, use the icon by itself rather than
squeezing or cropping the complete wordmark.

## Generated compatibility copies

Do not edit PNG or ICO files to change the Umicom identity. Make the change in
the appropriate SVG master, review its light and dark variants, and regenerate
the required platform output. This prevents several almost-identical logos
from slowly becoming inconsistent.

The shared CMake helper always stages the four SVG masters. A client must ask
for `RASTER_FALLBACKS` explicitly when it still uses an older raster-only image
path. New applications should not enable that option.
