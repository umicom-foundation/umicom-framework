# Prepare and publish a project recipe

Project creation has two different stages. A **recipe** describes borrowed file names and template text. A **generation plan** owns the resulting file names and contents. The generator later writes that plan to a filesystem. Keeping those stages separate lets Studio show a review and lets a CLI report a refusal before creating files.

## Ownership and admission

1. Keep every string and file array in a recipe alive for the whole validation or build call. A pointer must designate a readable, terminated C string; the validator cannot discover the extent of an arbitrary pointer.
2. Initialize a variable set with umi_developer_project_variable_set_init. Set copies each name and value into bounded owner storage.
3. Validate the complete recipe. Every file pointer is checked before any duplicate-path comparison reads a later record. Secondary-language entries must also be present and nonempty.
4. Build into a caller-owned UmiDeveloperProjectGenerationPlan. The builder uses a private heap candidate because the fixed plan is too large for routine worker-stack allocation.
5. Check the returned status before displaying or applying a new plan. A refusal leaves every output byte unchanged; it does not make an old plan current for the refused request.

<!-- Earlier exact-spelling boundary retained for review; the active paragraph documents shared portable admission.
The template and plan checks retain a lexical relative-path policy. Drive/stream separators, rooted paths and any double-dot substring are refused. Exact duplicate names are refused. Filesystem case folding, alternate separators, symbolic links and other filesystem aliases require separate destination review; this API is not a filesystem sandbox.
-->

The template and plan checks retain a lexical relative-path policy. Drive/stream separators, rooted paths and any double-dot substring are refused. Rendered plans additionally reuse portable rooted-file names and reject ASCII case/separator aliases and file/parent conflicts. Unicode folding, symbolic links and actual filesystem object identity remain outside this check. See [project path conflicts](project-path-conflicts.md) for examples and qualification boundaries.

## A small C23 consumer

~~~c
#include <stdlib.h>
#include "umicom/developer_project/generation_plan.h"

static UmiStatus prepare(UmiDeveloperProjectGenerationPlan *plan)
{
    UmiDeveloperProjectVariableSet variables;
    umi_developer_project_variable_set_init(&variables);
    UmiStatus status = umi_developer_project_variable_set(
        &variables, "MESSAGE", "Hello from Umicom");
    if (status != UMI_STATUS_OK) return status;

    const UmiDeveloperProjectTemplateFile files[] = {
        {"README.md", "# {{MESSAGE}}\n", 0, 0}
    };
    const UmiDeveloperProjectTemplate recipe = {
        .structure_size = sizeof(UmiDeveloperProjectTemplate),
        .api_version = UMI_DEVELOPER_PROJECT_API_VERSION,
        .template_id = "example.readme",
        .title = "A reviewed project",
        .description = "One owned document",
        .kind = UMI_DEVELOPER_PROJECT_META,
        .build_system = UMI_DEVELOPER_PROJECT_BUILD_NONE,
        .primary_language_id = "developer.language.markdown",
        .files = files,
        .file_count = 1U
    };
    return umi_developer_project_generation_plan_build(
        &recipe, &variables, "C:/projects/example", plan);
}
~~~

Allocate the plan on the heap in the calling application, check allocation, call prepare, inspect its returned status, and release the plan with free when finished. This example only constructs memory; it does not create the named directory. Production clients should reuse the existing project-generation request and destination services when obtaining user inputs.

## Failure behaviour

| Boundary | Result |
| --- | --- |
| Later file has a null path | INVALID_ARGUMENT before pairwise string comparison |
| Two admitted paths are identical | ALREADY_EXISTS |
| Missing substitution variable | NOT_FOUND |
| Unclosed substitution marker | PARSE_ERROR |
| Rendered string exceeds its fixed destination | CAPACITY_EXCEEDED |
| Candidate allocation fails | NO_MEMORY |
| Valid complete recipe | OK and a complete replacement output |

Variable-set validation checks count, occupied string termination, nonempty names and duplicate names. A malformed set is rejected before lookup. A failed set operation preserves all bytes, including unused slots and revision. Empty values remain supported. Successful replacement is allowed in a full set; adding another name is refused. Revision exhaustion is refused before mutation.

The low-level render-text function remains a streaming buffer operation and can leave partial text after syntax or capacity failure. Its callers must discard that text. The generation builder is the transaction boundary; its private candidate absorbs those partial writes and is released on every failure.

## Owner qualification

From the Framework checkout, these commands use the existing Windows preset. Stop at the first failing command.

~~~powershell
cmake --preset windows-ucrt64-debug
cmake --build --preset windows-ucrt64-debug --parallel 2 --target umicom-project-template-admission-test umicom-project-variable-admission-test umicom-project-generation-transaction-test umicom-project-generation-allocation-test
ctest --test-dir build/windows-ucrt64-debug --output-on-failure --no-tests=error -R "framework[.]developer_project[.](template-admission|variable-admission|generation-transaction|generation-allocation)"
~~~

The focused selection contains 62 authored cases. Record the source revision and modifications, compiler, configuration, selected case names, skips and exit status. These cases do not write project files, configure a generated project or launch GTK. A passing focused selection must therefore be followed by the relevant built-in template generation and installed-SDK qualification before claiming that a complete new application works.

The allocation case compiles the real generation implementation through a narrow allocator seam. It checks initial allocation refusal, release after a later render refusal, and successful retry. Other cases cover late invalid records, full storage, aliasing into the previous plan, and maximum admitted file count.

## Extend the service

Add reusable admission to Framework rather than duplicating it in a wizard. Preserve the distinction between syntax checking and filesystem identity. If a new recipe needs a resource file, keep its binary transport separate from this text-only plan: a C string cannot represent an arbitrary ICO file. Application branding should use the shared SDK branding helper and approved SVG/ICO resources.

See the [worked lesson](../learning/prepare-a-project-recipe.html) for a step-by-step failure exercise.
