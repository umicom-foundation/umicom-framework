# Plan a video with a storyboard

A **storyboard** is a plan for the scenes in a video. A scene describes what the viewer will see and hear. Planning the scenes first helps you decide which pictures, characters, narration and music you need before generating or editing the video.

Umicom Media's Storyboard page combines an ordered scene list with references to assets in your creative library. An **asset** is a piece of source material, such as an image, recording or document. An **avatar** is a character representing a person in your story. In this workflow, import its picture as an image and describe its role in the scene.

Preparing instructions is a local text operation. It does not animate an avatar, upload pictures or generate a video. The generation tools remain a separate, reviewed step. A model needs the actual reference files through a supported attachment workflow to reproduce their content; an asset name alone cannot provide that content.

## Follow the workspace

```text
Asset library                 Storyboard                         Creation tool
images / audio / text   ->     ordered scenes + asset tags   ->    reviewed brief
                                      |
                                      +----> export instructions to your folder
```

The page has three resizable columns: **Assets and references**, **Scenes in playback order**, and the prepared instructions. Drag the dividers to give a column more room. The existing video editor remains available under **Video Edit**.

## Prepare your assets

1. Open Umicom Media and choose **Storyboard** from the top navigation or the File menu.
2. Choose **Open asset library**. Use that library's file control to choose a local file, give it an ID and a label, select its type, then import it. An ID is a short, unique name used to identify the asset. A label is a description people can read.
3. For this exercise, import a coastline image with ID `coast`, an avatar image with ID `guide`, and a narration document with ID `narration`. Choose files you can use for the project. The IDs are case sensitive: `Guide` and `guide` are different.
4. Wait for each import to finish. The library owns a copy of the imported bytes. A declared type describes the intended use; it does not prove that a decoder or generation provider supports the file.
5. Save the library using its own save controls if you want to reopen those assets later. See [Creative asset libraries](creative-asset-libraries.html).
6. Return to **Storyboard** and choose **Refresh assets**. The list displays references such as `@{coast}`. Refresh again after importing or removing assets.

## Write the first scene

1. Enter `Welcome to the coast` as the scene title.
2. Set the requested duration to `10` seconds. This is a planning value, not a promise that a generation service supports exactly that duration.
3. In the story field, write:

   > Start with a wide view of @{coast} in warm evening light. Introduce the guide shown in @{guide}. Use @{narration} as the source for a short spoken welcome. Move the camera slowly towards the sea. Keep the guide's appearance consistent between scenes.

4. You can type each tag, or place the cursor in the story, choose an imported asset from the dropdown and press **Insert tag at cursor**. If text is selected, the tag replaces that selection. The operation refuses text that would exceed the limit; it does not cut the story short.
5. Choose **Add as new scene**. Your scene appears in the numbered list. Editing the fields afterwards changes an unapplied draft until you choose **Apply changes to selected scene** or **Add as new scene**.

A tag has an opening `@{`, the exact library ID, and a closing `}`. For example, `@{guide}` identifies the image named `guide`. The words around the tag explain its role. A regular email address containing `@` is ordinary text.

## Add and arrange scenes

1. Replace the title and story fields with a second scene, such as `The journey continues` and `Follow the path in @{coast}. Let @{guide} point towards the next destination.`
2. Choose **Add as new scene**. This adds another scene without replacing the first.
3. Select a scene in the dropdown to edit it. Choose **Apply changes to selected scene** to retain the new text, or **Discard pending edits** to reload the last applied version.
4. Choose **Move scene earlier** or **Move scene later** to change the order.
5. Choose **Remove selected scene** only when you want to remove it from this session. Scene removal currently has no Undo command; export useful instructions before removing work.

If you try to change scenes while a draft is unapplied, the selection returns to the current scene. Apply or discard those edits first. This protects the draft from disappearing when you browse the list.

## Prepare, review and use the instructions

1. Apply the current scene edits and choose **Prepare instructions**.
2. Read the right-hand column. It shows each scene's order, requested duration, story and referenced asset IDs, types and labels. Source file paths and asset bytes are not added to these instructions.
3. If a tag is missing or misspelled, correct it or import the missing asset, then prepare again. A failed preparation preserves the previous displayed instructions and blocks copying a new scene to the creation tool.
4. Select the scene you want to develop and choose **Use selected scene in Create from text**. That page opens with the selected scene as a Video source brief. If it already contains work, first save that work, then explicitly allow replacement on the Storyboard page and copy again.
5. Review the received source and choose **Prepare brief**. Follow [Create media from a source excerpt](media-creation-briefs.html) to refine it with a configured text model or hand it to an available video tool. Copying a scene does not submit a provider request.
6. Review provider capabilities, supported attachments and the request before generation. Reference tags remain text until a provider attachment workflow supplies the files. Confirm that any generated video meets your instructions; model output is not guaranteed to preserve characters, timing or text accurately.

The handoff rechecks referenced IDs, labels and types against the library. If the referenced metadata has changed since preparation, prepare and review again. This check concerns the written plan: it does not compare the contents of pictures or certify generated media. The plan includes an instruction to use the canonical Umicom SVG watermark in the final export. This text does not itself burn a watermark into a video.

## Save your work where you choose

1. Prepare and review the instructions you want to keep.
2. In the export control, enter a complete new filename in an existing folder you can access, for example `C:\umicom\projects\coastal-film\storyboard-instructions.txt` if you have already created that folder.
3. Use the export save action and check its result. An existing file is not overwritten; use a new name for the next revision.
4. Save the asset library separately. The exported instructions are text, not a video or a complete project archive.

<!-- The earlier session-only instructions are preserved for source review; the scene-document workflow below now replaces that limitation.
Scenes currently stay in memory for the current session. There is no automatic storyboard recovery or structured scene import. Export before closing Media. You can use the exported text as a reference when recreating scene fields; reopening the text does not automatically reconstruct the scene list. If you edit scenes after preparing, the export still contains the previous displayed instructions until preparation succeeds again.
-->

Use Scene document to save and reopen applied scene fields. Instructions export remains a separate text workflow: it does not reconstruct the scene list. If you edit scenes after preparing or open another scene document, the export still contains the previous displayed instructions until preparation succeeds again. There is no automatic save, session recovery or close-time unsaved-change prompt; save the intended scene document before closing Media.
## Save and reopen editable scenes

1. Apply or add pending scene edits, or use Discard pending edits. Save and Open refuse to proceed while the current form has unapplied changes.
2. Expand Scene document and enter the complete absolute path of a JSON file in a trusted folder. Review the path carefully; Save may create missing parent folders.
3. To save, select Allow replacing the selected file on the next save, then choose Save applied scenes. This can replace an existing file at that path.
4. To open, select Allow replacing current scenes on the next open, then choose Open scene document. A malformed or unsupported document preserves the current scenes.
5. Wait for the result. The form is disabled while the job runs. Request cancellation asks it to stop cooperatively; a save already publishing can still finish.
6. Review the persistent file-status label and result message. If another source edit or path change occurs during Open, the completion keeps the current panel instead of applying the loaded draft.
7. Save the asset library separately. After reopening, refresh assets and prepare instructions again before using a scene in Create from text.

Each approval applies to the next accepted operation. Editing the path or scene input clears approvals. A save captures the applied scene values and an owned path before starting a worker; it does not read GTK widgets from that worker.

An empty scene list can be saved and reopened. Unresolved or incomplete asset tags remain editable text. Open does not upload assets, restore a prepared handoff or authorize a provider request.

The scene document stores order, title, story and requested duration only. It does not contain asset bytes, creation briefs, model selections, generation receipts, undo history or unapplied widget text. Keep those workflows separate when collecting a project.

Closing or retiring the panel requests cancellation. A retained control cannot start a new file operation after retirement, and a late completion does not change the retired UI. Retirement cannot undo an atomic save that has already started.

The native controls and regression scenarios are authored. File replacement, display layout, installed-runtime behavior and closing while I/O is active require execution in the qualification environment.

See [Editable storyboard documents](editable-storyboard-documents.html) for the C API, installed-client example and exact file contract.


## Limits and troubleshooting

| What you enter | Current limit or recovery |
| --- | --- |
| Scenes | Up to 16 in one storyboard |
| Scene title | 128 UTF-8 bytes |
| Story | 1024 UTF-8 bytes, including tags |
| Duration | 1–600 requested seconds per scene |
| References | Up to 8 distinct asset IDs per scene; repeated use of one tag counts once |
| Library | Up to 32 assets, subject to the library's combined byte limit |
| Missing tag | Refresh the inventory and check spelling, case and braces |
| Busy library | Wait for its import, save or open operation to finish, then retry |
| Scene too large for handoff | The combined instructions must fit the creation brief's 2048-byte source field. Shorten the scene or export the complete instructions |
| Existing creation draft | Save it first; enable replacement explicitly for the next copy |
| Export appears out of date | Apply edits, prepare again and check the displayed text before exporting |

UTF-8 stores characters using one or more bytes. A 1024-byte field may hold fewer than 1024 characters. Input that exceeds a limit is rejected, and existing applied scenes remain unchanged.

## How the C implementation works

The reusable engine belongs to Umicom Framework. The Media application selects the Media workspace; it does not implement another tag parser or another asset library.

The small example below creates one text-only scene. It does not open a window or contact a model. Compile it as part of a program linked to `Umicom::creative_workspace`.

```c
#include "umicom/creative_workspace/media_storyboard.h"
#include <stdio.h>

int main(void)
{
    UmiMediaStoryboardDraft draft = {0};
    UmiMediaStoryboardScene scene = {
        .title = "Opening",
        .story = "Show a coastal path in evening light.",
        .duration_seconds = 10U
    };
    UmiMediaStoryboard *plan = NULL;
    UmiCreativeExport text = {0};
    UmiStatus status = UmiMediaStoryboardAppend(&draft, &scene);
    if (status == UMI_STATUS_OK)
        status = UmiMediaStoryboardCreate(&draft, NULL, 0U, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiMediaStoryboardInstructions(plan, &text);
    if (status == UMI_STATUS_OK)
        (void)fwrite(text.bytes, 1U, text.size, stdout);
    UmiCreativeExportFree(&text);
    UmiMediaStoryboardDestroy(plan);
    return status == UMI_STATUS_OK ? 0 : 1;
}
```

`draft` owns the editable scene values. `{0}` starts those values empty. `Append` validates a scene before changing the draft. `Create` makes an independent prepared plan; passing `NULL, 0U` says there are no assets. A tagged story instead needs the entries captured from its asset library. `Instructions` creates text owned by the caller. The two final cleanup calls release that text and the plan even if an earlier step failed.

| Framework file | What to study |
| --- | --- |
| `include/umicom/creative_workspace/media_storyboard.h` | Scene fields, limits and ownership contracts |
| `src/creative_workspace/media_storyboard.c` | Validation, ordered edits and exact tag binding |
| `adapters/gtk4/media_storyboard_gtk4.c` | Scene selection, draft protection, asset tags and resizable columns |
| `adapters/gtk4/media_brief_gtk4.c` | Receiving a source without silently overwriting an existing draft |
| `tests/creative_workspace/test_media_storyboard.c` | Boundary, Unicode, identity and output-ownership checks |
| `tests/creative_workspace/test_media_storyboard_gtk4.c` | Control activation, asset import, export, handoff and retired-window checks |

Try misspelling a tag, then correct it. Notice how the existing reviewed instructions remain available while the new handoff is blocked. This separates an editable draft from a validated result—a pattern useful in document editors, transaction entry and other applications.
