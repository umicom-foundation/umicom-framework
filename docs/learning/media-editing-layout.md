# Using the Umicom Media editing workspace

![Umicom](../../resources/brand/umicom-logo.svg)

The editing workspace keeps your source files, preview and editing controls visible together. A **clip** is a named part of a video file. A **sequence** places clips in an order. A **recipe** stores that order and the chosen start and end times; it does not contain the video itself.

This guide describes the sequential video editor. It supports up to six clips and ten minutes of output. Preview is silent. The renderer creates silent H.264 MP4 at 30 frames per second; soundtrack and selectable subtitle exports are separate operations. Multi-track compositing, waveform editing and transitions are not implemented in this editor.

## 1. Find the five working areas

1. **Top navigation:** choose Video Edit to edit your sequence. Create from text prepares a written brief. AI Video, AI Image, Audio and Canvas open their respective tools. Moving between these pages keeps the same sequence editor alive.
2. **Left library:** search and select the clips already in the sequence. Expand Import clip to enter a new clip name and choose a local source file. The cards currently show file icons and names, rather than extracted video thumbnails.
3. **Centre preview:** select a clip, then use the play button to preview its applied range. The square button stops playback. Hover over either icon to read its name. Preview is not a rendering of the complete sequence.
4. **Right inspector:** Clip range contains the source start and end. Split clip and Clip order expand to show their controls. The Export tab contains output paths and rendering options.
5. **Bottom timeline:** select a clip to inspect it. Undo and Redo are beside the timeline. The scissors button opens the split controls; it does not cut immediately. Zoom changes the display width of clips, not their duration.

The wordmark and icon come from the shared Umicom SVG assets. The interface does not use artwork or logos from example layouts.

## 2. Make room for your work

1. Resize the application window using its outer border. On first display, the editor gives bounded space to the side panels and the remaining width to the preview.
2. Drag the divider beside the library, beside the inspector or above the timeline. These are **split panes**: moving a divider changes the space allocated to neighbouring panels.
3. Collapse Import clip after adding your sources. Collapse an inspector section when you no longer need its fields. Collapsing keeps the existing field values, including changes you have not applied.
4. Open Layout above the editor. Use Show media library, Show inspector and Show timeline to hide or reveal those areas. Focus preview hides all three.
5. Choose Remember layout (this window), change the arrangement, then choose Restore remembered layout. This memory lasts only for the current window. It is not stored in the recipe or restored after restarting the application.
6. Choose Reset panel layout to show all panels and size them for the current editor area. Reset does not remove clips or clear the editing history.

The panes have minimum sizes. On a small display, hide a panel or collapse its sections instead of trying to shrink a control below its usable size. Free-floating windows and arbitrary docking are not supported by this editor.

## 3. Add, trim and split a clip

1. In Media, expand Import clip. Enter `intro` as the clip name and choose a local video file you can read.
2. In Inspector, expand Clip range. Enter `2000` for Source start and `6000` for Source end. These values are **milliseconds**: 1000 milliseconds is one second. This example selects four seconds of source video, starting two seconds into the file. The end time is excluded.
3. Choose Add clip in the left panel. The library and timeline show the new clip. The editor does not inspect the file to fill in its duration automatically.
4. Select the clip and press the preview play button. If playback cannot start, check the file path, the chosen range and the available playback backend. An icon or a card in the library is not proof that a codec can decode the video.
5. To change its range, edit Source start and Source end, then choose Apply trim. Typing into a field alone does not modify the recipe.
6. Choose the scissors button on the timeline. The inspector opens Split clip even if the inspector was hidden. Enter `4000` for Split at source time and `intro-right` for the new right-hand clip ID, then choose Split clip. This splits the source range at four seconds into the source file, not four seconds into the timeline.
7. Use Undo to reverse the split. Use Redo to apply it again. To rearrange a selected clip, expand Clip order, enter its new position starting at 1, then choose Move clip.

Use Project to choose a recipe path and Save new recipe to write a new recipe. Use Export to choose an encoder and a new output path. The controls ask for paths you choose; navigating to Project or Export does not itself write a file. See [Edit a local video sequence](video-sequences.html) for the complete save, render, soundtrack and subtitle workflow.

## 4. Understand the Framework implementation

Umicom Media displays the shared Framework editor. Its layout does not create a second copy of the editing engine.

| Source file | Responsibility |
| --- | --- |
| `adapters/gtk4/media_surface.inc` | Product navigation and canonical branding |
| `adapters/gtk4/video_sequence_editor.inc` | Placement of library, player, inspector and timeline |
| `adapters/gtk4/video_sequence_presentation.inc` | Compact buttons and sections around existing commands |
| `adapters/gtk4/video_sequence_layout.inc` | Divider positions, visibility and window-local layout recall |
| `adapters/gtk4/video_sequence_gtk4.c` | Event callbacks and the owned sequence editor |
| `tests/video_sequence/workspace_presentation.inc` | Regression cases for navigation, drafts and editing history |

A **callback** is a function called when an event occurs. For example, the timeline's Undo button still uses the existing editing callback. Replacing its visible text with an icon must preserve its callback, command identity and accessible name. An **accessible name** describes a control to assistive technology; it should also make sense without seeing its icon.

Resizing is a presentation event. It must not add an editing-history entry. GTK can notify other code while a panel is being shown or resized. The layout functions therefore set the editor's presentation guard while adjusting widgets and stop if the owner closes the editor. The regression cases deliberately attempt an edit during a notification to check this separation.

## 5. Check a layout change

1. Add two clips and select the second.
2. Type a new right-hand clip name in Split clip without applying a split. Collapse and reopen the section. The name should remain.
3. Hide the inspector, then use the scissors shortcut. The same fields should reappear with the same values.
4. Resize the window and drag all three dividers. The selected clip, source paths and sequence should remain unchanged.
5. Use Undo once. It should reverse the last media edit, not a panel resize or section expansion.
6. Switch to another mode and back to Video Edit. Check your selected clip and unapplied fields again.

If a button does nothing, record its name, the visible mode and whether an export operation is in progress. If the logo is absent, record the runtime's SVG loader message rather than substituting a different image. Share a screenshot and the relevant application log when reporting the issue; omit credentials and private source content.
