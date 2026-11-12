![Umicom](../../resources/brand/umicom-logo.svg)

# Create projects through the native Umicom CLI

The project command is a thin adapter over Framework's existing project service. Studio and the CLI use the same built-in recipe registry, destination rules, variable rendering and new-project operation.

The commands described here are authored. CLI, filesystem and generated-project qualification remain separate execution steps. Existing create/new app commands keep their previous behaviour.

## List available recipes

~~~powershell
umicom project templates
~~~

The output gives each complete template ID, title and file count. It describes recipes compiled into this Framework command. It does not establish that the corresponding compiler, toolkit or SDK is installed.

For a GTK C application, the ID is developer.template.c23-gtk4. The C++ variant is developer.template.cpp-gtk4 and the thin desktop variant is developer.template.thin-desktop-application. Use developer.template.c23-console for a console application.

## Preview a project

Every identity and destination option is explicit. The parent must be absolute; the folder is one portable component. This example uses a new owner-controlled project location.

~~~powershell
umicom project plan --template developer.template.c23-gtk4 --name "Umicom Example" --id org.umicom.example --target umicom_example --parent C:/umicom/projects --folder umicom-example
~~~

Add --show-files to display the complete rendered file texts. Planning creates no directories or project files and does not check whether the destination already exists. It reports the intended destination, file paths and text-byte counts. It does not configure CMake or run any generated content.

Review the full template ID and destination. Names and application IDs follow the existing generation-request rules; display names allow spaces, while other identity fields use letters, digits, dots, hyphens and underscores. Target names follow the stricter existing target-name rule. These options are not a general-purpose language-escaping interface.

## Create the reviewed project

Repeat the same request with create and the explicit apply flag:

~~~powershell
umicom project create --template developer.template.c23-gtk4 --name "Umicom Example" --id org.umicom.example --target umicom_example --parent C:/umicom/projects --folder umicom-example --apply
~~~

The service rejects an existing destination. There is no overwrite option. Creation writes the current built-in recipe; it does not consume a persisted signed plan, so use the same qualified CLI binary and request that you reviewed.

No Git repository is initialized and no commit, push, build, test, installation or application launch is performed by this command. Those are separate owner actions. The created project remains untrusted input for later build execution.

<!-- Earlier progress wording retained for review:
The destination must not be changed concurrently by another process. A failure during writes or a later model update can leave new files behind. The command reports the number of files written and skipped; it does not delete partial work or automatically retry. Inspect and preserve that directory, diagnose the cause, and select another new destination for a corrected attempt.
-->
The selected parent must already exist. Model and build-profile validation precede writes; the project directory and files use exclusive native creation. The destination must not be changed concurrently by another process. A later write or registry failure can leave new files behind. Counts include files created before a write/flush/close failure, so read the returned failure status alongside them. Nothing is deleted or automatically retried. Preserve and inspect the folder, then select another new destination for a corrected attempt. See [checked project creation](checked-project-creation.md) for the shared contract and qualification cases.

## Build the generated GTK application

After source review, from the generated project's root:

~~~powershell
Set-Location C:\umicom\projects\umicom-example
$env:Path = "C:/msys64/ucrt64/bin;$env:Path"
cmake --preset windows-ucrt64-debug -DCMAKE_PREFIX_PATH=C:/umicom/installed/framework
cmake --build --preset windows-ucrt64-debug --parallel 2
.\build\windows-ucrt64-debug\bin\umicom_example.exe
~~~

Stop at the first failure. The SDK path is an example and must identify the installation you actually qualified. The GTK recipes require Umicom::ui_gtk4 and the shared branding helper. See [GTK starters](gtk-project-starters.md) for install/package commands, SVG/ICO checks, inherited-font limits and clean-machine qualification. A console starter has different dependencies.

## Interpret command results

| Exit code | Meaning |
| --- | --- |
| 0 | The requested catalogue, plan or creation operation completed; read its stated scope |
| 1 | Service, rendering, destination, write, allocation or output-stream failure |
| 2 | Invalid command/options or refused identity/destination syntax |

Each option may appear once. Unknown flags, missing values, empty arguments, duplicate options, --apply on plan and --show-files on create are refused. Create without --apply is refused before service execution.

A preview success is not a reservation. A creation success is not a successful application build. Keep those outcomes distinct in automation and user interfaces.

## Qualify the command itself

From the Framework checkout with the existing Windows build configured:

~~~powershell
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom umicom-project-cli-test umicom-project-service-allocation-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]developer_project[.](cli[.]|service-allocation|native-route-)"
~~~

There are 25 adapter scenarios, one service allocation scenario and two native route checks when the native command target is enabled. The tests preserve prior fixture directories and choose a new bounded name on repeated creation runs. They do not build or launch generated products.

The service allocation scenario refuses the large heap workspace, checks a fresh report and unchanged model, verifies release on a later refusal, retries a dry run and refuses an exhausted service revision before writes. Source metadata, compiler identity, skipped cases and logs must accompany execution evidence.

Read the [terminal project lesson](../learning/create-projects-from-the-terminal.html) and [project recipe ownership guide](project-template-admission.md) before adding another frontend.
