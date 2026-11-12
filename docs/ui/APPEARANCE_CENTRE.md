Created by: Sammy Hegab  
Organisation: Umicom Foundation  
Licence: MIT
# Appearance profiles

The Umicom Framework appearance model is a toolkit-neutral catalogue of
semantic colours, typography, density and accessibility preferences. Product
frontends render the active profile without putting GTK, Qt or web types in the
public model.

## Responsibilities

- Framework owns profile validation, activation, cloning, protection and the
  versioned persistence codec.
<!-- Earlier ownership wording is retained for review; approved shared artwork now has an explicit Framework source.
- Each application owns its branding assets and chooses the built-in profiles
  it presents.
-->
- Framework supplies approved SVG and ICO artwork. Each application selects its product identity, stages those resources through the shared branding helper and chooses the profiles it presents.
- Frontend adapters read the active snapshot and map semantic tokens to native
  controls.

Built-in profiles are protected from replacement or removal. A user or product
extension can clone a built-in, edit the clone, lock it and persist it through
`umi_ui_appearance_profile_encode`. Exactly one profile can be active at a
time.

Brand artwork is referenced by a runtime resource path; image bytes and private
developer paths are never stored in the Framework model. This lets installed
applications place the official artwork next to their executable while future
frontends use the same appearance data.

## Accessibility and portability

Profiles expose comfortable, compact and spacious density modes, bounded font
scaling and a reduced-motion preference. All colours use semantic names such as
`surface`, `foreground`, `accent`, `warning` and `danger`. The high-contrast
profile is suitable for a frontend that needs strong focus separation and
reduced decorative motion.

The persistence record starts with `appearance-v1`. Decoders validate every
field before publishing a profile, allowing the format to evolve without
silently accepting malformed settings.

## Follow a font setting from C to a native control

A font family is a requested typeface name, such as Segoe UI. A fallback list
tries the declared families in order using the fonts installed on the computer.
Framework's default interface list is Segoe UI, Inter, Sans; its code list is
Cascadia Code, JetBrains Mono, Monospace. These are preferences, not bundled font
files. The final available font can differ between systems.

1. Create a complete profile before changing fields. A profile owns its text;
   do not replace its arrays with borrowed pointers.
2. Change the interface or editor role and keep its size within the documented
   range. Interface size accepts 8–32 points, editor size 8–40 points and scale
   0.75–2.0. The renderer multiplies the size by the scale.
3. Validate the complete value. Each inline field must end inside its array.
   NaN and infinity are rejected, as are empty font-list entries and unsupported
   characters. A rejected replacement leaves the accepted catalogue unchanged.
4. Publish the profile through the model or the native Appearance editor.
   Built-in profiles remain protected. Use the existing custom-profile workflow
   for a user's editable preferences.
5. Inspect both ordinary text and code text in the actual frontend. A saved
   editor-font field alone does not establish that a widget used it.

The following fragment belongs inside a function returning UmiStatus. Include
umicom/ui/appearance.h and stdio.h. It prepares a value; it does not create a
window or change a running application.

```c
UmiUiAppearanceProfile profile;
UmiStatus status = umi_ui_appearance_profile_init(&profile,
    "reading-profile", "Reading profile", UMI_UI_THEME_MODE_DARK,
    UMI_UI_DENSITY_COMFORTABLE);
if (status != UMI_STATUS_OK) return status;
snprintf(profile.editor_font, sizeof(profile.editor_font),
    "%s", "Cascadia Code, JetBrains Mono, Monospace");
profile.editor_font_size = 12.0;
profile.font_scale = 1.25;
char reason[192];
status = umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason));
if (status != UMI_STATUS_OK) return status;
/* An accepted 12-point code role requests 15 points after scaling. */
```

## Native GTK font roles

The workbench and workstation renderers use one shared typography projection.
Ordinary controls inherit the interface role. Code controls use the editor role:
GtkTextView widgets with the umicom-editor or monospace class, explicitly marked
umicom-code-font widgets, and shared view-model rows. Use a semantic role instead
of adding a product-specific hard-coded font. Keep normal prose out of a code
role merely to make its spacing look different.

The projector quotes concrete family names and preserves generic fallbacks.
It formats point sizes with a decimal period regardless of the user's numeric
locale. Changing the process locale during rendering would interfere with other
threads, so rendering does not do that. A user can select another installed font
without replacing the approved logo or changing the product's resource identity.

This correction applies to the GTK adapters. Qt, web, chart labels and exported
media have their own rendering boundaries and need separate qualification.
A missing font falls back; it does not authorize downloading or redistributing a
font. The source assets and typography policy are described in the
[branding guide](../SVG_FIRST_APPLICATION_BRANDING.md).

## Exercise and recovery

Run the existing appearance and persistence tests, then the focused
framework.ui.appearance.admission and framework.ui.appearance.typography checks
from a configured Framework build. For example, from that build directory:

```powershell
ctest -R "^framework[.]ui[.]appearance([.]|$|_persistence)" --output-on-failure --no-tests=error
```

The new admission cases cover all 22 inline text fields, non-finite sizes,
malformed lists, supported bounds and rejected model updates. Native cases
inspect the computed interface/code size relationship and requested family list.
The comma-decimal locale case reports a skip if that locale is unavailable;
native display cases report a skip if GTK cannot initialize. Keep those skips
separate from passing coverage.

After the checks, open an application, select a custom appearance, change the
editor font and scale, and inspect a source editor or another code-role control.
Restart the application to check its existing persistence workflow. Record which
fonts were installed and inspect light, dark and high-contrast modes at the
intended display scales. If only code text stays unchanged, inspect the widget's
role and CSS precedence. If CSS parsing fails, preserve the diagnostic and check
profile admission and decimal formatting before removing validation.

References: [GTK font properties](https://docs.gtk.org/gtk4/css-properties.html),
[GLib locale-independent number formatting](https://docs.gtk.org/glib/func.ascii_formatd.html).
