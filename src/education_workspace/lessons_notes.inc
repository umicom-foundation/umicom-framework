/* Umicom Framework | notes practicum | Sammy Hegab, Umicom Foundation | MIT
 * Canonical lesson content. Generated views borrow these immutable records. */
    {
        "notes.values",
        "notes",
        "foundations.c-data",
        "1. Values that belong to a note",
        "A notes application needs to remember text and explain whether it saved a change. Start by separating those two jobs. The text is data. A return value is a message from one function to another: the requested operation either succeeded or it did not.\n\nIn the Notes project, a NoteBook stores a small collection of notes. Its count tells us how many slots contain valid notes. The array may have room for eight notes, but that does not mean eight notes already exist. Capacity is available space; count is the amount currently used.\n\nRead the declarations before running the program. size_t is the unsigned type used for object sizes and array positions. A char array stores bytes. Ordinary C text ends with a zero byte, written '\\0'. That terminator takes one slot, so an array of 64 bytes can hold at most 63 text bytes plus its terminator. A displayed character may occupy several bytes in UTF-8.\n\nThese distinctions prevent a common user-facing fault: an application saying Saved when the text was actually too large. Our function checks its input and reports the failure instead of silently discarding the end of a note.",
        "char title[64];       /* At most 63 text bytes, then the terminator. */\nsize_t count = 0;       /* No notes have been created yet. */\nconst size_t capacity = 8;",
        "Export the notes project. Locate NOTE_CAPACITY and NOTE_TEXT_CAPACITY in notes.h. Predict whether a 63-byte title fits in an array of 64 bytes. Write your prediction in the learning note before checking the code.",
        "Count the terminator as one more byte. Count describes used entries, not allocated slots.",
        "notes",
        UMI_TEACHER_LANGUAGE_C23, 25U, {
            {"A 64-byte C text array needs one terminator. How many text bytes fit?", {"64", "63", "65"}, 1U, "The terminator uses one byte: 64 - 1 = 63 text bytes."},
            {"A notebook has eight slots and count is zero. How many notes exist?", {"Eight", "One", "Zero"}, 2U, "Capacity is space that could be used. Count is the number of valid entries."},
            {"A save function rejects oversized text. What should the view display?", {"A failure explanation", "Saved", "An empty success notification"}, 0U, "Only display success after the operation has actually succeeded."},
        }
    },
    {
        "notes.contracts",
        "notes",
        "foundations.c-functions",
        "2. A function makes a promise",
        "A function contract tells the caller what it may provide and what it may expect back. NotesSet receives a notebook, a key and some text. The caller still owns the input strings. The notebook keeps its own copies when the operation succeeds.\n\nLook at the order of the checks. Null pointers, invalid keys and oversized text are rejected before the notebook changes. This gives the caller a useful guarantee: a failed update leaves the previous note intact. It also makes a unit test straightforward. Save a valid note, try an invalid replacement, then read the original note again.\n\nA const char pointer allows the function to read a string without promising to modify it. It does not say that the memory will live forever. When NotesFind returns a pointer into the notebook, that pointer is borrowed. Copy the text when it must survive later edits or destruction of the notebook.\n\nThe header is the public promise; notes.c contains the implementation. Keeping them separate lets another application use the contract without depending on private decisions such as where the checks are placed.",
        "int result = NotesSet(&book, \"meeting\", \"Bring a notebook\");\nif (result != 0) {\n    /* Explain the failure; do not report that the note was saved. */\n}",
        "In test_notes.c, find the oversized replacement check. Describe the value that must remain after the rejected update. Add a similar test for an empty key.",
        "Validate first, then copy. A borrowed pointer does not extend the lifetime of its owner.",
        "notes",
        UMI_TEACHER_LANGUAGE_C23, 30U, {
            {"When should an oversized replacement be rejected?", {"After copying half", "Before changing the notebook", "Only when the program closes"}, 1U, "Checking before mutation preserves the previous valid note."},
            {"Who owns the input string after NotesSet returns?", {"The original caller", "The compiler", "The notes view"}, 0U, "The notebook copies successful input; it does not take ownership of the caller’s string."},
            {"Where should a caller look first for the function contract?", {"A generated binary", "The operating-system registry", "The public header"}, 2U, "The header states the supported interface; the implementation explains how it is fulfilled."},
        }
    },
    {
        "notes.memory",
        "notes",
        "foundations.c-memory",
        "3. Read only the entries that exist",
        "An array provides indexed storage, not an automatic list of valid entries. A loop over notes must stop at count. With count equal to three, the valid indices are zero, one and two. Index three is already outside that set.\n\nMemory safety is not only about avoiding a crash. Reading an unused slot may display stale or misleading text while the program appears healthy. Writing beyond an array can damage unrelated state. Both faults break the user’s expectation that one note operation affects only its intended note.\n\nTrace the search loop on paper. Begin with i equal to zero, compare i with count, inspect the current entry only when the comparison succeeds, and then increment i. For an empty notebook the loop body must not execute at all.\n\nThe Notes project uses fixed-capacity storage so you can inspect its ownership without first learning an allocator. A production component may allocate memory dynamically, but it still needs the same boundaries: who owns the object, which entries are valid, and when the storage stops being usable.",
        "for (size_t i = 0; i < book.count; ++i) {\n    /* Only entries below count are valid. */\n}",
        "Run the empty-notebook test. Then add notes until capacity is reached and verify that one extra insertion fails without increasing count. Do not remove the existing assertions.",
        "For count equal to three, list 0, 1 and 2. The condition is i < count, not i <= count.",
        "notes",
        UMI_TEACHER_LANGUAGE_C23, 35U, {
            {"Which indices describe three valid entries?", {"1, 2, 3", "0, 1, 2", "0, 1, 2, 3"}, 1U, "Zero-based indices begin at zero and stop before count."},
            {"How many times should the search body run for an empty notebook?", {"Once", "Eight times", "Zero times"}, 2U, "With count zero, the loop condition is false before the first iteration."},
            {"A full notebook rejects a new key. What happens to count?", {"It stays unchanged", "It increases anyway", "It becomes zero"}, 0U, "A rejected insertion must leave the existing valid state unchanged."},
        }
    },
    {
        "notes.tests",
        "notes",
        "foundations.c-modules",
        "4. Prove a change with a focused test",
        "A useful test checks a promise that matters to the user. For the Notes project, the promises include inserting a new key, replacing an existing key without adding another entry, and keeping old data after a rejected change.\n\nConfigure, build and test are separate steps. CMake configuration describes the build. The build invokes the compiler and linker. CTest runs the registered checks. A successful configuration does not prove that a C source file compiles, and a successful compilation does not prove that its behaviour is correct.\n\nRead a failing assertion as a question: which expected property did the program violate? Begin with the first failing check rather than deleting it. Keep the existing test and make the smallest justified correction. Run the focused checks again, then the wider relevant suite.\n\nThe quiz record in Education Studio is separate from this evidence. Passing a multiple-choice quiz does not compile your edited project. The real project tests run in your development environment, and their output must be inspected there.",
        "cmake -S . -B build -G Ninja\ncmake --build build\nctest --test-dir build --no-tests=error --output-on-failure",
        "Build the exported notes project in Studio or PowerShell. Add an assertion that replacing meeting leaves count unchanged. Run CTest and record what the assertion proves in your learning note.",
        "A build checks whether the sources can become a program; a test checks a stated behaviour. Keep those results separate.",
        "notes",
        UMI_TEACHER_LANGUAGE_C23, 40U, {
            {"Which step actually runs registered behaviour checks?", {"Configuration", "CTest", "Staging a Git commit"}, 1U, "CTest runs registered tests; configure and build have different jobs."},
            {"Replacing a note with the same key should do what?", {"Add a duplicate", "Clear all notes", "Keep the same count"}, 2U, "An existing key identifies the same note, so replacement does not create another entry."},
            {"Does a passed Education quiz prove your edited C project passes?", {"No", "Yes", "Only when the window is open"}, 0U, "Quiz answers and compiled-code test results are different evidence."},
        }
    },
