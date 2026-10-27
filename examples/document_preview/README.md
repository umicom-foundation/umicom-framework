<!--
Umicom Framework
File: examples/document_preview/README.md
PURPOSE: Teach bounded local documentation reading with a real SDK consumer.
AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
LICENCE: MIT
-->

# Read a local document with Umicom Framework

This example reads a small HTML page and prints a text preview. HTML is a text
format that describes a page using tags such as `<p>` for a paragraph. A preview
extracts words from that page; it does not run scripts or display images.

You need an installed Umicom Framework SDK containing `Umicom::browser` and its
dependencies, a C23 compiler, CMake and Ninja. Use the same compiler and runtime
as the SDK. Install the current SDK first: an older library cannot supply a new
function simply because a newer header is present.

## 1. Configure the example

Open PowerShell. These commands assume the source and SDK use the paths below.
The source directory holds files you edit; the build directory holds generated
files. Run one command at a time and stop if it fails.

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\examples\document_preview" -B ".\build\sdk-document-preview" -G Ninja -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe -DCMAKE_PREFIX_PATH=C:/umicom/installed/applications
```

CMake should identify the compiler and finish with “Build files have been written
to”. If it cannot find `UmicomFrameworkConfig.cmake`, check the completed SDK
installation and the prefix. Do not continue with an incomplete configuration.

## 2. Build and check

```powershell
cmake --build ".\build\sdk-document-preview" --parallel 2
ctest --test-dir ".\build\sdk-document-preview" --output-on-failure --no-tests=error
```

The compiler translates the C file into machine instructions. The linker connects
those instructions with Framework libraries. CTest runs the example on its sample
page and checks that an invocation without a filename is refused. Those checks
do not qualify every browser service or the Studio graphical interface.

## 3. Run the program

```powershell
& ".\build\sdk-document-preview\umicom-document-preview.exe" "C:\umicom\Umicom-Applications\framework\examples\document_preview\lesson.html"
```

The first line should be `Title: Learning with Umicom`. The output also reports
the source byte count, whether the preview was shortened, and the readable text.
The byte count includes HTML tags and depends on the file's line endings.

An **absolute path** identifies the file from the drive root. A relative path such
as `lesson.html` depends on the current directory and is deliberately refused by
this service. A quoted argument keeps a path containing spaces together.

## 4. Follow the source

1. `main` receives the number of arguments and their text.
2. `malloc` allocates the output structure. The application owns that allocation
   and releases it with `free` on each completed path.
3. `umi_browser_document_preview_read_file` opens an ordinary local file under a
   one-mebibyte limit: 1,048,576 bytes. A byte is a unit of stored data; it is not
   necessarily one displayed character.
4. Framework checks the open file, reads through the same handle, rejects embedded
   zero bytes and extracts a bounded preview.
5. The program prints the result and checks whether output was written.

The public declaration is `include/umicom/browser/document_preview.h`. The
implementation composes `src/browser/local_document.c` with the existing platform
reader and HTML preview parser. Studio's Documentation panel uses that same
reader with its own input limit, then converts the byte-oriented text for GTK.

The service checks for observable changes to the opened file. It cannot prove
that an adversarial writer made no undetectable change. Parent directories must
be trusted; this function is not a directory sandbox. File I/O can block, so an
interactive application needs suitable scheduling for slow storage.

## 5. Explore safely

Edit the title and paragraph in a practice copy of `lesson.html`, then run the
program again with its absolute path. You do not need to rebuild when only the
input page changes. Rebuild when `main.c` changes.

Try a filename that does not exist. Expect a nonzero exit and a refusal message.
Keep the exact message when reporting a problem. Do not use private documents,
device paths or untrusted network locations for this exercise.

The text parser handles a limited HTML subset. It does not reproduce page
layout, promise complete HTML conformance, validate all text as UTF-8, or replace
a full web browser. A native view must validate or repair UTF-8 before displaying
the result, including after any capacity-based shortening.
