# Umicom Brand Resources

This directory is the source of truth for the visual identity shared by all
applications built with Umicom Framework. An application should request a
logical resource from the Framework resource catalogue instead of copying a
file or writing a repository path into its source code.

## Which file should an application use?

- `umicom-icon.svg` is the `<>` mark for light or neutral surfaces.
- `umicom-icon-on-dark.svg` uses lighter colours on dark surfaces.
- `umicom-logo.svg` combines the mark and dark text for light surfaces.
- `umicom-logo-on-dark.svg` combines the mark and light text for dark surfaces.
- `umicom-icon.png` is the transparent raster mark used by native resources.
- `umicom-logo.png` is a transparent raster fallback for systems that cannot
  display SVG files.
- `umicom.ico` contains the common Windows icon at several sizes.

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

The suite build may stage copies under the outer repository's `assets` and
`win` directories for modules that have not yet moved to direct resource
resolution. Those staged files are generated compatibility inputs. Update the
canonical files in this directory and let CMake refresh the copies.
