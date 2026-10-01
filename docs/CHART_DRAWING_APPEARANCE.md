# Give a chart drawing its own appearance

Framework owns drawing appearance so native charts and other renderers can
produce the same colours, outlines and fills. Applications call the existing
chart and trading services; they do not need a second store for these values.

## Create and apply a choice

1. Include **umicom/chart/drawing_appearance.h**.
2. Read the drawing currently displayed to the user from its drawing registry.
   Keep the copied ID, pane ID and row revision.
3. Call **UmiChartDrawingAppearanceFromHex** with a colour such as **#33AAFF**,
   a width in tenths of a logical pixel, and a fill percentage.
   Width **25** means **2.5** pixels. Width must be **10** to **80**; fill must
   be **0** to **80**.
4. Apply with **UmiChartDrawingSetAppearance**, passing the captured row
   revision. In a trading workspace use
   **UmiTradingWorkspaceSetChartDrawingAppearance**, which also checks that
   the same instrument is selected.
5. Redraw through **UmiChartDrawingRender**. It resolves appearance before
   sending ordinary commands to the scene renderer.
6. Save through the chart checkpoint or trading chart persistence service
   when the user explicitly requests a save.

For example, once the caller has obtained a current drawing and registry:

~~~c
UmiChartDrawingAppearance appearance;
UmiStatus status = UmiChartDrawingAppearanceFromHex("#33AAFF", 25, 30, &appearance);
if (status == UMI_STATUS_OK) {
    status = UmiChartDrawingSetAppearance(registry, drawing.pane_id,
        drawing.id, drawing.revision, &appearance);
}
/* A non-OK result leaves the original drawing in place.
 * Show the result and reload current evidence before retrying a stale edit. */
~~~

These services run on the model owner's thread. They do not provide concurrent
access or send trading commands. Applying changes copies the appearance while
retaining identity, geometry, selection, lock and visibility. Identical stored
choices do not advance the registry revision. A NULL appearance explicitly
clears the custom choice to the tool defaults.

## Understand the stored representation

The existing bounded **style** string stores this versioned ASCII form:

~~~text
umi-drawing:1:33AAFF:25:30
~~~

The final fields mean RGB colour, width in tenths and fill percent. Width and
fill always contain two decimal digits. Encoding uses no locale-dependent
decimal separator. The decoder accepts either case for hexadecimal digits;
the encoder writes uppercase.

No checkpoint schema change is required: the existing style field already
round-trips through chart documents, checkpoint hashes and reviewed restores.
Duplication copies the same field.

**UmiChartDrawingAppearanceDecode** distinguishes an empty style
(**NOT_FOUND**), an unsupported format/version (**UNAVAILABLE**), and a
malformed recognised encoding (**PARSE_ERROR**). The output stays unchanged
on failure. Callers pass the readable buffer capacity; a missing terminator
within that capacity is an invalid argument.

Rendering uses the original tool defaults for empty, unknown or malformed
styles and leaves their stored bytes intact. An editor should explain this
before offering explicit replacement or clearing. Opening or rendering a saved
chart must not migrate its style silently.

## Rendering and failure behaviour

Custom RGB channels range from 0 to 255. Outlines and level labels are opaque.
Only ranges and liquidity zones use the fill percentage; zero omits the fill
command. Other tools can retain the same representation without drawing a fill.
The caller supplies a valid plot theme when resolving appearance directly.

The scene builder clips geometry first, skips hidden/offscreen objects and
checks command capacity before adding an appearance's commands. A filled box
uses two commands; an unfilled box uses one. A support/resistance level uses
one line and one label. Default colours and widths retain the previous values
exactly.

The native editor loads a copied row explicitly. Chart timers do not change
unapplied fields. Apply requires the loaded identity, pane and row version to
remain current. Retained controls stop acting when their chart is detached.

See the Trader guide **DRAWING_APPEARANCE.md** for the user workflow. Regression
sources under **tests/chart_appearance** cover codecs, version guards,
rendering, persistence and native controls. Product acceptance also traverses
Trader's actual chart storage and controls. These tests must be executed on
the target platform before claiming release qualification.

[Undo and redo drawing edits](CHART_DRAWING_HISTORY.md) explains how to reverse an applied appearance and when to load the drawing again.
