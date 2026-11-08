# Inspect source-build recipes

A source-build recipe is a bounded JSON description of inputs, toolchain claims and dependent stages. Framework accepts the description into an immutable owner and can project its stages into the existing task registry. This service does not execute stages or verify the files described by the recipe.

## Inspect a saved recipe

1. Build and install a matching native Framework tool and development SDK through your normal qualification workflow.
2. Select an absolute path to a JSON recipe. The example below uses the public SDK exercise in Bits2Banking.
3. Run the inspector and read both the status and its limitation message.

```powershell
umicom recipe inspect --file C:/umicom/Umicom-Bits2Banking/labs/native/framework-recipes/recipe.json
```

Accepted input prints the recipe ID, exact-byte SHA-256, build/host/target triples and stage counts. Exit 0 means metadata inspection succeeded. Exit 1 means file admission, parsing or output failed; exit 2 means invalid command arguments. No configure, compiler, test, install, package or deployment process is started by this route. Existing execution commands remain separate.

## Describe the closed format

All members below are required. Unknown members and duplicate decoded names are rejected, including command and shell fields. JSON comments are not supported. Start with this illustrative declaration; its digest values are claims, not measured evidence:

```json
{
  "schema": 1,
  "id": "source-demo",
  "build": "x86_64-w64-mingw32",
  "host": "x86_64-w64-mingw32",
  "target": "riscv64-unknown-elf",
  "toolchain": {
    "id": "riscv-gcc",
    "compiler": "tools/bin/gcc",
    "manifest": "tools/provenance.json",
    "sha256": "0000000000000000000000000000000000000000000000000000000000000000"
  },
  "inputs": [
    {
      "id": "main",
      "path": "src/main.c",
      "sha256": "0000000000000000000000000000000000000000000000000000000000000000",
      "bytes": 0
    }
  ],
  "stages": [
    {
      "id": "compile",
      "label": "Compile source",
      "phase": "build",
      "dependencies": [],
      "inputs": [
        "main"
      ],
      "outputs": [
        "out/main.o"
      ]
    }
  ]
}
```

Build identifies where tools are built, host where the built tool or program runs, and target where a compiler's produced code runs. Triples are syntax-checked identifiers; they do not prove ABI compatibility, compiler installation or cross-compilation support. The toolchain digest identifies a claimed provenance manifest, not the compiler binary itself.

Each stage uses configure, build, test, install, package or deploy. Inputs name root source IDs; outputs name exact files. Dependencies name other stage IDs and may point forward in declaration order. The shared graph rejects unresolved references and cycles. It does not infer dependencies from file names, verify that an output exists, or connect a stage output to another stage input. A source may be referenced by multiple stages; each stage must use distinct references.

| Boundary | Limit |
| --- | --- |
| Recipe bytes | 1 MiB |
| Root sources / stages | 64 / 32 |
| Dependencies / source references / outputs per stage | 16 each |
| IDs / labels / triples | 95 / 159 / 95 bytes before the terminating byte |
| Relative file path | 511 ASCII bytes |
| Source byte claim | 0 through 9223372036854775807 |
| Digest | 64 lowercase hexadecimal characters |

IDs start with a lowercase letter and then use lowercase letters, digits, dot, underscore or hyphen. Triples have at least three nonempty hyphen-separated components using lowercase letters, digits, underscore and dot. Labels may contain UTF-8 but no C0 controls or DEL. File claims use forward slashes and ASCII letters, digits, spaces, dots, underscores and hyphens. Absolute paths, drive prefixes, empty or dot segments, parent traversal, wildcards, backslashes, trailing dots/spaces and DOS device stems are refused. Compiler, manifest, source and output file claims must not overlap by case-insensitive equality or ancestor path. These rules prevent ambiguous metadata; they do not establish filesystem confinement.

## Use the C owner

Link the installed `Umicom::developer` target and include `umicom/build/recipe.h`. JSON interpretation belongs to this higher-level owner; the underlying build task library does not depend on the parser.

1. Call `UmiBuildRecipeOpen(bytes, size, &recipe, &report)` for already-owned bytes, or `UmiBuildRecipeReadFile(absolute_path, &recipe, &report)` for a local regular file.
2. Read copied summary data with `UmiBuildRecipeInfoRead`; borrow source and stage entries with `InputAt` and `StageAt` only while the recipe lives.
3. Optionally call `UmiBuildRecipeCreateTasks`. The returned registry owns independent copies and uses semantic phases with empty command text and workspace-trust requirements.
4. Destroy the recipe once with `UmiBuildRecipeDestroy` and destroy any returned registry separately. Failure preserves the caller output owner. Success transfers a new owner; the caller remains responsible for any previous owner it replaces.

The file wrapper reuses Framework's bounded regular-file reader. It refuses a final link or device, but parent aliases may be followed. Use a trusted parent directory. I/O can block, so native UI clients should call it on an appropriate worker. The memory parser opens no file. Neither path downloads sources or trusts digest claims automatically.

## Check and recover

The registered `framework.recipe.*` checks cover valid limits, all phases, forward and diamond dependencies, schema rejection, aliases, cycles, byte ownership, CLI admission and the real native route. Run them in your qualified build tree:

```powershell
ctest --test-dir C:/umicom/Umicom-Applications/framework/build-ucrt --output-on-failure --no-tests=error -R "^framework[.]recipe[.]"
```

Use your actual configured build directory if it differs. Tests are provided as regression coverage; this guide is not evidence that they passed. On rejection, preserve the exact JSON and bounded diagnostic before changing it. Correct the declared path or dependency rather than weakening the validator. On a missing SDK symbol, compare installed headers and linked archives. A digest mismatch after whitespace is expected because recipe identity covers exact bytes, not normalized JSON. Authenticity, real file verification, reproducible builds, checkpoint resume and authorized execution require separate services and qualification.
