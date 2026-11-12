# Create media from a source excerpt

A **creation brief** describes what you want to make. It combines a short source excerpt, your creative direction and an output choice. For example, a paragraph about a railway journey can become the starting point for a storyboard, an image or a video. A storyboard is a numbered plan of the shots that make up a film.

Umicom Media prepares the brief on your computer. This step arranges your words; it does not call a model or generate a picture. You can edit the result before passing it to a generation tool or a text model.

## Prepare a brief

1. Open Umicom Media and choose **Create from text** in the top navigation. You can also choose it from **File**. The video editor and its current project remain available under **Video Edit**.
2. Drag the divider between the two columns to give the source or the review more room. Each column scrolls independently.
3. Enter a **Project title**, such as `Coastal journey`.
4. Choose **Storyboard**, **Image**, **Video** or **Music brief**. Music brief produces a written plan, not an audio file.
5. Paste a short passage into **Source excerpt**. You can copy it from a book, document, conversation or your own notes. This page accepts text; it does not open PDF, DOCX or ebook files.
6. Enter **Creative direction**. For example: `A quiet documentary. Show a small passenger train and natural evening light. Avoid on-screen text.` This field is optional.
7. Choose **Prepare brief**. The right column shows the instructions, project title, direction and source together. No network request has been made.
8. Read and edit the brief. Check names, claims, scene details and the intended output. Treat the result as instructions for another tool, not evidence that media has already been generated.

Try this source passage:

> A passenger train follows the coast at sunset. It slows beside a small station. A traveller steps onto the platform and looks towards the sea.

With **Storyboard** selected, the prepared text asks for numbered shots, visual descriptions and narration. It does not invent those shots itself. A text model can develop the plan in the next step.

## Use a local or remote text model

A **model** is the program that produces a response. An **endpoint** is the address where a model service accepts requests. Installing model files alone does not start such a service.

1. In **Model settings file**, enter an absolute path that you choose, such as `C:\umicom\work\media-connections.sqlite3`. An absolute path includes the drive or filesystem root. This file stores connection settings, not your source excerpt or chat history. Opening it begins a local settings load; it does not send a request to a provider.
2. Choose **Open model connections with this brief**. The connection window displays a copy under **Captured context for one chat**. Later changes in Media do not silently change this copy.
3. Select a saved connection. To add one, use **New**, enter its ID, label, endpoint and model identifier, choose its route, enable it, then use **Save draft**. A local service must be running and provide the chat-completions protocol supported by the existing adapter. Use **Local loopback (no credential)** for that route. Remote connections use HTTPS and their configured credential reference.
4. Use the connection window's chat command. The new chat contains the captured brief as context. Enter a request such as `Develop the captured brief into six numbered shots. Include suggested narration for each shot.`
5. Choose **Review request**, inspect the endpoint and complete request, give the displayed approval and choose **Send reviewed request once**. Only this explicit action sends the reviewed text.
6. Read the response. Copy the useful text back into the Media brief or save it with the chat's export controls. Responses are not inserted into the video timeline automatically.

An Umicom model can participate through this route when it is served through a compatible endpoint. This page does not load Umicom model weights directly, train a model or fine-tune it. Those are separate model-runtime capabilities. A server responding at an address is not by itself proof that it supports the expected request and response format.

## Create an image or video

1. Select **Image** or **Video** as the output choice and prepare the brief again. Review the new instructions.
2. Choose **Use in AI Image** or **Use in AI Video**. Media copies the brief into its existing image or video generation page and brings that page into view. This does not submit a request.
3. If that page already contains a prompt, Media preserves it. Return to **Create from text** and select **Allow replacing the edited brief or generator prompt on the next action** only if you want to replace those words. Choose the handoff again. The permission applies to one action and is then cleared.
4. In the generation page, choose the provider settings and review the request using that page's controls. Existing approvals are cleared when a new brief arrives. A generation already in progress is not replaced.
5. Send only after inspecting the provider request. The image route uses the existing Seedream adapter; the video route uses the existing PixVerse adapter. API availability, provider credentials and provider charges still apply.
6. Save a successful image or follow the video receipt/status workflow. Import a completed local asset into the appropriate editing tool when supported. Handoff does not create a persistent generation queue or automatically download and add clips to the timeline.

## Plan music and combine media

Choose **Music brief** to ask a text model for mood, instrumentation, tempo suggestions and song structure. A tempo describes the pace of music, commonly measured in beats per minute. The response is a plan: it is not a recording or synthesized performance.

Use the existing [song planning guide](song-and-music-video-plans.html) to work with timed lyrics and shot plans. Use [audio arrangements](audio-arrangements.html) to combine supported local audio, and [creative asset libraries](creative-asset-libraries.html) to keep source files together. Each editor has its own supported formats and save controls. An exported text brief is not a saved video project, asset library or audio arrangement.

## Save work and recover from mistakes

1. Use the text export control beneath the model controls. Enter a complete new `.txt` filename in a folder you can access and choose its save action. The folder must already exist. Existing files are not replaced.
<!-- Earlier text-only recovery limitation retained for review: 2. Check the export message before closing Media. Source fields and the settings-path entry are not automatically saved between launches. The exported brief includes the source and direction as plain text; there is no form-state import in this version. -->
2. Check the export message before closing Media. Plain-text export remains separate from the Creation document controls described below. Those controls save and reopen source fields and the edited prompt explicitly; the model settings-path entry is not included, and there is no automatic recovery.
3. If you change a source field, Media keeps the previous brief but blocks handoff until you prepare it again. You can still save the displayed draft.
4. If you edited the brief, preparing it again asks for explicit replacement. Save or copy those edits first if you want to keep them.
5. If a settings file cannot be opened, check that its path is absolute and writable. A failed load is shown in the connection window. Changing a path does not move an existing settings database.
6. If generation is busy, wait for its current result or use that tool's existing stop control. A handoff never cancels a provider job for you.

The title accepts up to 128 UTF-8 bytes, the excerpt 2048 and the direction 512. The editable brief accepts up to 4096 bytes for handoff and export. UTF-8 is a way to store characters as bytes; some characters take more than one byte. Oversized input is refused rather than silently cut short. Use a smaller excerpt and keep the complete original document separately.


## Save and reopen a creation document

1. Expand Creation document in Create from text. Enter an ordinary absolute JSON path in a trusted folder; Save may create missing parent folders.
2. To save the current title, output choice, source, direction and edited prompt, select Allow replacing the selected file on the next save, then choose Save creation text. Incomplete fields and a prompt retained from older source edits can be saved.
3. To open, select Allow replacing current source and prompt on the next open, then choose Open creation document. This explicitly replaces the current form text; save any work you want to keep first.
4. Wait for the result. The editing area and file form are disabled during the job. Request cancellation is cooperative: a save already publishing may still finish, and the result reports that outcome.
5. Review the persistent file-status label. Editing the path, kind, source fields or prompt clears file approvals. Each approval is consumed by the next accepted operation.
6. After opening, the prompt remains an edited draft and model handoffs stay disabled. Preparing a replacement for nonempty restored text requires the existing replacement checkbox. Save the restored wording before replacing it if you want to compare both versions.
7. If a file is damaged, unsupported or too large, current text is kept. If a direct edit or path change occurs while an Open worker runs, its completion does not overwrite that newer panel state.

Creation documents use the shared Framework service, which captures owned text and an owned path before a worker starts. The worker never reads native widget buffers. An incoming storyboard source is refused with a busy result while document I/O owns the creation panel.

Closing or retiring the panel requests cancellation and prevents further actions. A late completion cannot update a retired panel. An atomic save already publishing is not undone by closing the window.

The document does not save model settings paths, model choices, credentials, provider permissions, receipts, generated assets, video/audio projects or undo history. There is no automatic save, restart recovery or close-time unsaved-change prompt.

The native controls and 24 recovery scenarios are authored but unexecuted. Qualify actual file replacement, native display behavior, installed libraries and closure during I/O separately from portable document parsing.

See [Editable creation drafts](editable-creation-drafts.html) for the versioned schema, ownership rules and installed C example.

## How Framework and Media share the work

The application chooses its Media profile. Framework owns the brief, validation, widgets and provider handoff. Another application can reuse the same services without duplicating provider requests.

| Source file | Responsibility |
| --- | --- |
| `include/umicom/creative_workspace/media_brief.h` | Portable input fields, output choices and ownership contract |
| `src/creative_workspace/media_brief.c` | Validate and copy the input into an immutable prepared brief |
| `include/umicom/ui/gtk4/media_brief.h` | Native workspace construction and retirement contract |
| `adapters/gtk4/media_brief_gtk4.c` | Source editing, review, explicit export and guarded handoff |
| `adapters/gtk4/creative_workspace_surface_gtk4.c` | Add the page to Media while retaining existing editors |
| `adapters/gtk4/provider_connections_window_gtk4.c` | Open the shared connection chooser with an explicit storage path |
| `tests/creative_workspace/test_media_brief.c` | Portable validation and copied-source regression cases |
| `tests/creative_workspace/test_media_brief_gtk4.c` | Native preparation, stale-source, prompt replacement and retirement cases |

`UmiMediaBriefCreate` copies its input into an owned result. `UmiMediaBriefPrompt` returns borrowed text that remains valid until `UmiMediaBriefDestroy`. A host must copy it if it needs the text after destruction. Native hosts call `UmiMediaBriefGtkRetire` before retiring their generation owners so retained buttons cannot restart work.
