# Keep drawings while hiding them

Hiding an annotation removes its lines, shading and labels from the chart.
The drawing still exists. Its ID, anchors, style, selection and lock are kept,
so it can be shown again without reconstructing it.

## Use the native chart

1. Select an instrument and choose a drawing from the object list.
2. Choose **Hide / Show**. The list keeps the drawing and adds **(hidden)**.
3. Choose **Hide / Show** again to display that drawing.
4. Use **Hide all** or **Show all** for the current instrument's drawing set.
   Other instruments and order markers are unchanged.
5. Choose **Save chart** to keep the visibility choices in the saved chart.

Hidden drawings count towards the drawing capacity. A lock protects geometry
and individual removal; it does not prevent hiding or showing. Duplicating a
hidden drawing makes an unlocked hidden copy. Show the copy before choosing
**Move selected**. These controls never send an order.

If a drawing or list changed after it was displayed, the action is refused.
Review the refreshed list and try again. An unfinished move is cancelled by a
visibility action. There is no visibility undo stack or named drawing group.
Show all explicitly shows every retained drawing in the current instrument,
including ones hidden individually before the last Hide all action.

## Use the C23 API

A registry is Framework's owner of value copies of drawings. A revision is a
number identifying the version you inspected. Passing that number back avoids
applying an old command to newer work.

1. Create or obtain the existing `UmiChartDrawingRegistry`.
2. Read the intended row with `umi_chart_drawing_registry_find`.
3. Call `UmiChartDrawingSetHidden` with its pane, ID, revision and either `1`
   to hide or `0` to show.
4. Check the returned status. `UMI_STATUS_BUSY` means the inspected revision
   changed; read and review it again. A wrong pane is rejected.
5. Read the row again before another mutation, because a change advances its
   revision. Requesting the already-current visibility changes no revision.

For a whole pane, capture `umi_chart_drawing_registry_revision` and pass it to
`UmiChartDrawingSetPaneHidden`. It validates all affected rows before publishing
the changes together. A failure leaves every row and the output count unchanged.
The count reports only changed rows. An empty pane succeeds with a count of zero.
An edit to another pane also changes the registry revision and invalidates this
whole-pane request. These operations run on the registry owner's thread.

The complete example `examples/chart/drawing_visibility.c` demonstrates the
single-row workflow without a GUI, market connection or disk access. It reports
one hidden drawing, then the same drawing shown again.

Trading applications should use `UmiTradingWorkspaceSetChartDrawingHidden` or
`UmiTradingWorkspaceSetChartDrawingsHidden`. They additionally require that the
instrument is still selected in the workspace. Chart visibility is shared
across its views; it is not stored as a separate widget preference.

## Render and save

`UmiChartDrawingProject` returns `visible=0` for a valid hidden tool.
`UmiChartDrawingRender` then adds no commands. Trend, Support, Resistance, Range,
Liquidity zone and Ray use this rule. Custom renderers that read snapshot
coordinates directly must check the hidden flag themselves.

The immutable chart document and checkpoint keep visibility with the drawing.
Restoring a reviewed checkpoint replaces the pane's complete drawing set and
visibility. A visibility change after preview makes that preview stale.
Unknown future tool names can retain valid metadata in a saved document, but
the native chart lists and renders only its six supported tools.

## Compatibility

Zero `visibility_flags` means visible. The only defined bit is
`UMI_CHART_DRAWING_VISIBILITY_HIDDEN`; domain validation rejects other bits.
Initialize snapshots to zero or use `UmiChartDrawingInitialize`.

The public snapshot gained a trailing field. Rebuild Framework and every
consumer together; mixing old and new compiled snapshot layouts is unsupported.
The existing generic registry API still normalizes `api_version` to 1; it is
not a binary compatibility bridge for older compiled structures.

New checkpoint drawing records declare schema 2. The new reader accepts old
records and loads them visible. It rejects incomplete versioned records and
unknown visibility bits. Older applications cannot read the new drawing records;
they may offer their existing last-good recovery path. Preserve the profile
database before switching back to an older application. Hiding alone is not
a save, and a memory-only save does not survive application exit.
