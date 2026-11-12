![Umicom](../../resources/brand/umicom-logo.svg)

# Create a new project without replacing an existing destination

A new project needs a different filesystem policy from applying an additive
recipe to an existing workspace. Framework keeps both operations available.
Use the new-project service when the destination must be new; use the older
generator only when its documented skip/overwrite behavior is intended and the
caller exclusively controls that workspace.

## Understand the sequence

1. Validate the request and render the complete owned generation plan.
2. Compose the candidate project model, language defaults and build profile.
3. Validate model/profile state and available registry capacity before writes.
4. Admit every portable relative path and combined destination length.
5. Create the final project directory with an exclusive native operation.
6. Prepare checked descendant directories and create each file exclusively.
7. Publish the model, build profile and registry revision after all writes succeed.

The selected absolute parent must already exist. No Git operation, compiler,
installer, network request or application launch occurs during this sequence.
Generated source remains untrusted input for any later execution.

## Choose the public entry point

| Entry point | Intended use |
| --- | --- |
| UmiDeveloperProjectCreateNew | Studio/CLI creation with required model and profile outputs |
| umi_developer_project_service_generate_new | Shared composition with optional model/profile outputs |
| umi_developer_project_generator_create | Apply an already owned plan to a new destination |
| umi_developer_project_service_generate | Retained additive/overwrite compatibility service |
| umi_developer_project_generator_apply | Retained additive/overwrite plan writer |

Include umicom/developer_project/new_project.h for the frontend operation and
link the installed Umicom::developer target. Keep the service, request and
destination under one caller's serialized ownership during creation.

~~~c
UmiDeveloperProjectGeneratorReport progress;
UmiStatus status = UmiDeveloperProjectCreateNew(
    service, &request, &progress, &model, &profile);
if (status == UMI_STATUS_ALREADY_EXISTS) {
    /* Preserve the destination and ask for another new folder name. */
} else if (status != UMI_STATUS_OK) {
    /* Retain progress and any partial files for diagnosis. */
}
~~~

Model and profile outputs remain unchanged on refusal. The optional attempt
report is reset even when an argument is invalid. Output storage must not
overlap inputs or another output. Successful creation keeps the model registry's
existing upsert semantics: reusing an application ID can replace that ID's
registered model. A new folder name is not a new application identity policy.

## Read partial progress accurately

For live creation, directories_created counts native directory creations,
including the project root. files_created counts files whose exclusive create
succeeded, even if their later write, flush or close failed. bytes_written counts
bytes accepted by those writes. files_skipped stays zero because this mode
refuses a collision instead of skipping an existing file.

The returned status is essential. A nonzero file count on failure does not mean
that those files are complete. Keep the folder, record the status and inspect
the partial contents. The service does not delete them, roll back the directory
tree, or retry through the overwrite API.

The lower-level generator/service dry run performs lexical admission without
filesystem access. It reports predicted files and bytes and zero directory
creations. The frontend wrapper preserves its older dry-run refusal when its
destination already exists. The CLI project plan command continues to render
without checking destination existence. None of these previews reserves a name.

## Follow native ownership limits

Directory preparation and file creation reuse the
[rooted directory](rooted-directories.md) and rooted-file services. A link or
reparse point at the selected parent or inside the project is refused. Existing
final names are not opened for replacement. Ancestors above the selected parent
remain trusted; this API does not inspect the entire filesystem namespace.

Each rooted operation owns its handles for that call. The whole project is not
one filesystem transaction, and handles are not retained between files. Prevent
external destination mutation while creating the project. Privileged writers,
mount changes, Unicode/short-name alias behavior and power-loss durability are
not fully isolated. Exclusive file creation prevents overwriting an existing
leaf but does not provide a complete adversarial concurrency guarantee.

Recipe executable and optional flags remain descriptive metadata, as in the
legacy writer. This operation does not add executable permissions or silently
omit optional files. Keep interpreter/build execution as a separate reviewed
step.

## Run focused qualification

Run each command separately and stop on errors. Use a Framework build directory
configured from the source under review.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications\framework"
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-project-generator-create-test umicom-project-generator-create-effects-test umicom-new-project-test umicom-new-project-preflight-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]developer_project[.](generator-create[.]|generator-create-effects[.]|new-project[.]|new-project-preflight[.])"
~~~

The 51 authored scenarios include 28 native fixture cases and 23 injected-boundary
cases. Native fixtures cover existing content, repeated creation, missing and
linked parents, Unicode paths, previews and model/profile preservation.
Injected boundaries cover profile refusal, registry capacity, revision
exhaustion, absolute-root/overwrite admission, partial directory/file progress and stopping after the first failed
effect. They exercise the real composition/writer source with controlled
boundary outcomes; they do not establish native concurrency behavior.

Also run the existing CLI, service-allocation, generated-profile, path-admission,
rooted directory and rooted file suites. Preserve capability skips separately.
Record source revision, compiler, operating system, filesystem and complete logs.
The authored scenarios have not established compiler or runtime qualification.


## Share outcome wording across frontends

UmiDeveloperProjectCreationDescribe formats a bounded description from the
returned status and checked-creation report. The native CLI and project form
both use it. A collision before any effects asks for another destination;
a collision after native creations explains that partial work remains. This
avoids describing every ALREADY_EXISTS result as a pre-existing root folder.

The formatter performs no I/O and leaves its output unchanged when storage is
too small or the report is inconsistent. A NULL report explicitly means that
progress is unavailable. It accepts checked-creation reports with zero skipped
files; it is not a formatter for the legacy additive generator's skip policy.
The project form keeps more specific input-validation detail when available.

Nineteen pure message scenarios cover success, preview, refusal, partial
collisions, missing progress, report preservation, bounded output and invalid
report combinations. The existing native developer-dialog scenario also checks
that an existing-root refusal uses the same wording, leaves fields editable and
does not invoke project adoption. These cases require separate execution:

~~~powershell
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-project-creation-message-test umicom-developer-dialog-gtk4-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.](developer_project[.]creation-message[.]|ide_workflow[.]developer_dialog[.]gtk4)"
~~~

<!-- The synchronous-state note is retained as design history. The owned
worker described below replaces that execution path; qualification stays separate.
The native form still performs project creation synchronously. Shared progress
descriptions improve reporting but do not establish a responsive background
creation workflow or a GUI qualification pass.
-->

## Keep the project form responsive

The native New Project form copies the chosen template and all five identity
fields before starting a background worker. A worker is an execution thread
that performs the slower filesystem operation while GTK continues processing
its event loop. The worker creates its own Framework project service; it never
reads widgets, borrows the form's catalogue service or invokes application code.

The original shared creation service still validates the project and build
profile before writing. A worker does not make a multi-file write atomic.
Partial files remain after a native error. Read the reported progress, preserve
those files for inspection and choose a fresh destination when necessary.

Use the form in this order:

1. Choose a template, enter the project identity and select a new full path
   under an existing parent directory.
2. Choose **Create Project** once. Inputs and the action button become
   insensitive while the copied request is running.
3. Wait for the result. Normal **Close** and **Cancel** explain that a started
   write must finish; they do not promise to undo it.
4. On success, the form passes its owned model and profile to the live
   application. The displayed identity is restored from the captured request
   if an extension changed a field programmatically during the write.
5. If opening fails, use **Open Project** to retry adoption of the completed
   source. This retry does not generate files a second time.
6. Review build settings and workspace trust separately before running tools.

An application may temporarily hide a still-owned form. Successful completion
then retains the source identity without calling the application; presenting
the form again offers **Open Project**. Destroying the form or parent retires
callback authority. It does not cancel or roll back writes already started.
Keep the creating GTK main context running until the completion and task data
have drained. The worker owns no host callback context.

A widget notification can close a host before scheduling starts. Both the
ordinary project form and application-module form now create their task after
those admission checks. This avoids constructing a task that is then abandoned
without a result. The application-module form retains its existing reviewed
source workflow.

Twenty authored lifecycle scenarios hold dispatch, then release the operation
onto a real GTask thread with an injected writer. They cover copied inputs,
separate service ownership, repeated clicks, Close/Cancel, hidden completion,
retired owners, callback reentrancy, failed adoption, native refusal and partial
progress. They also exercise application-module admission retirement. The
injected writer creates no files; real filesystem behavior remains covered by
the separate integration and rooted-storage suites.

~~~powershell
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-project-creation-worker-gtk4-test umicom-developer-dialog-gtk4-test umicom-simulation-starters-gtk4-test umicom-developer-settings-lifetime-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]ide_workflow[.](project_creation[.]gtk4[.]|developer_dialog[.]gtk4|starter[.]gtk4[.]|settings[.]gtk4[.]project-)"
~~~

The existing Studio first-use wizard scenarios now wait for the actual form
completion before checking files and workspace adoption. Run those from the
corresponding Studio build as well. A timeout is a failure, not a successful
creation; timed-out native fixtures are retained instead of being removed while
a writer might still be using them.

These changes and regression scenarios are authored but not compiler, thread,
filesystem or GUI qualification evidence. Retain source and GTK/GLib runtime
identities with the owner-run results.

The task lifecycle follows [GTask](https://docs.gtk.org/gio/class.Task.html) and
[its worker-thread completion contract](https://docs.gtk.org/gio/method.Task.run_in_thread.html).



## Keep the host alive while source is being written

A host must check its pending source work before normal shutdown or replacing
the form. UmiGtk4DeveloperDialogWritePending is a GTK-thread observation of the
shared form's state. It covers admission, the writer and completion publication,
including hidden forms. It excludes read-only template previews. NULL is idle;
a pointer whose owner already called Destroy is not a valid query argument.

A native close-request callback can use the observation before its other close
checks:

~~~c
/* host->project_form is owned by this host until its explicit Destroy call. */
if (UmiGtk4DeveloperDialogWritePending(host->project_form)) {
    show_message(host, "Wait for source creation, then close again.");
    return TRUE; /* Keep the native host and its event loop alive. */
}
~~~

The observation does not reserve anything. Call it on the GTK thread immediately
before deciding whether to close or replace the form. Preserve the owner's
normal unsaved-document and running-tool checks. Do not automatically close when
the write finishes: opening the newly created source may introduce new work
that the user should review.

Studio applies this shared check to normal close, **Close Anyway**, and opening
a replacement project/application form. A pending write returns BUSY for form
replacement. The user can retry after the result is available. Direct forced
process termination and explicit controller destruction still cannot promise
that a multi-file write will finish or roll back.

The twenty Framework worker scenarios now also observe idle, admission, hidden
and completed activity. Three Studio first-use scenarios exercise native host
close and replacement of the ordinary or application form before GTK processes
the writer completion. They retain the real writer; they do not invoke generated
build tools.

~~~powershell
Set-Location "C:\umicom\Umicom-Applications"
cmake --build --preset windows-ucrt64-all-debug --parallel 2 --target umicom-studio-first-use-test
ctest --test-dir build/windows-ucrt64-all-debug --output-on-failure --no-tests=error -R "studio[.]first_use[.]gtk4[.]wizard-(close|replace|application)-during-write"
~~~

Build against the Framework source containing the new activity query first.
These commands are supplied for owner qualification; the new close behavior
has not yet been established by an executed GUI result.
