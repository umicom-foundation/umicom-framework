# Draw and edit chart annotations

Chart drawings record a user's analysis at two time and price coordinates.
Framework owns their geometry and editing rules so a desktop product and another
renderer can show the same drawing. Trader supplies the controls through the
shared GTK chart panel.

An anchor is one time and price pair. Time is a nonnegative number of milliseconds
since the Unix epoch; prices are finite numbers. These drawings use a linear price
scale. They do not download market history or place orders.

## Choose a shape

| Tool name | Anchors | Result |
| --- | --- | --- |
| `trend` | Two different times | A segment between the anchors. |
| `support` | First price | A horizontal level across the chart. |
| `resistance` | First price | A horizontal level across the chart. |
| `range` | Different times and prices | An outlined box between opposite corners. |
| `liquidity-zone` | Different times and prices | A translucent box marking the user's analysis. |
| `ray` | Two different times | A line from the first anchor through the second, continuing in that direction. |

A liquidity zone is an annotation, not a measurement from an order book. A ray
can point left or right. Swapping the corners of a box keeps its shape; swapping
the anchors of a ray changes its origin and direction.

## Create a drawing in a frontend

1. Include `umicom/chart/drawing_tools.h` and `umicom/chart/drawing_edit.h`, or
   the aggregate `umicom/chart/interaction.h`.
2. Choose a unique drawing ID and the pane ID that owns the drawing. Trader uses
   the selected instrument ID as its pane ID.
3. Call `UmiChartDrawingInitialize` with a tool kind and the two anchors. Check
   the returned status before using the output. Invalid anchors leave it unchanged.
4. Add the drawing with `umi_chart_drawing_registry_upsert`. Read it back from the
   registry to obtain its assigned revision, which identifies this stored version.
5. Use `UmiChartDrawingProject` for clipped geometry or `UmiChartDrawingRender`
   to append commands to a render scene. Neither call changes the drawing.

The following fragment assumes the caller owns a valid registry and handles the
returned status:

```c
UmiChartDrawingSnapshot drawing;
UmiStatus status = UmiChartDrawingInitialize(
    "analysis.range.1", "instrument.1", UMI_CHART_DRAWING_RANGE,
    (UmiChartPoint){60000, 100.0}, (UmiChartPoint){120000, 105.0}, &drawing);
if (status == UMI_STATUS_OK) {
    status = umi_chart_drawing_registry_upsert(registry, &drawing);
}
```

In a trading workspace, use `UmiTradingWorkspaceAddChartDrawing` instead. It
checks the selected instrument and allocates an unused ID. The workspace also
provides `UmiTradingWorkspaceMoveChartDrawing`,
`UmiTradingWorkspaceSetChartDrawingLocked` and
`UmiTradingWorkspaceDuplicateChartDrawing` so frontend code keeps those guards.

## Edit the version the user reviewed

1. Read the drawing and retain its pane, ID and revision with the displayed row.
2. Pass those values to `UmiChartDrawingSetGeometry` or
   `UmiChartDrawingSetLocked`. Never substitute a newer revision silently.
3. If the record changed, the call returns `UMI_STATUS_BUSY`. Refresh the row and
   let the user choose again. A different pane returns `UMI_STATUS_INVALID_STATE`.
4. For duplication, pass an unused ID to `UmiChartDrawingDuplicate`. The new
   drawing has the same anchors and opaque style text, but is unlocked and
   unselected. The original keeps its selection and lock.
5. Read the new stored version after a successful change. Locking to the current
   value or setting identical anchors succeeds without advancing the revision.

Locked drawings reject geometry changes. Trader also rejects their individual
removal. These are editing rules, not access control: a reviewed whole-chart
restore can replace locked records. Low-level registry APIs remain available;
frontends should use the protected editing functions for user actions.

Use these APIs on the thread that owns the chart. They do not provide concurrent
mutation locking. Strings and geometry are copied; callers do not transfer
ownership of their input buffers.

## Rendering and saved charts

Segments and rays are clipped in data coordinates before conversion to pixels,
so a partly visible line keeps its slope. Offscreen drawings return success with
`visible` set to zero. A box covering a viewport with only one timestamp spans
its width. That special display rule does not change its stored anchors.

Rendering needs at most two commands per drawing. Capacity is checked before
appending; the caller must supply a valid plot style. Support and resistance use
its positive and negative colours. Other lines are amber and zones are blue.
The saved `style` field is retained as opaque text; this renderer does not yet
offer per-drawing colour or stroke editing.

The existing checkpoint format stores all six tool names, anchors, lock and
selection flags without a format change. The generic archive can preserve an
unknown future tool; trading restore refuses tools it cannot interpret. Updating
an older frontend is required before it can restore range, zone or ray drawings.

Saving and restoring are explicit operations. See
[Chart checkpoints](CHART_CHECKPOINTS.md) for the Framework storage contract and
the product's saved-chart guide for its native workflow.

The current controls do not provide undo, object groups, hide/show, drawing
alerts, or broker order submission. A copied drawing initially overlaps its
source until the user moves it.


## Choose an appearance for one drawing

[Drawing appearance](CHART_DRAWING_APPEARANCE.md) explains the shared colour,
outline width and fill services. These services keep the existing style field
and checkpoint representation, preserve older styles, and compare the displayed
row revision before publishing an explicit change.
