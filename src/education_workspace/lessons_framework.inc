/* Umicom Framework | framework practicum | Sammy Hegab, Umicom Foundation | MIT
 * Canonical lesson content. Generated views borrow these immutable records. */
    {
        "framework.ownership",
        "framework",
        "foundations.framework",
        "1. Put reusable behaviour in Framework",
        "A notes window is an application view. Reading and writing Umicom-owned state is a shared service responsibility. The Framework Notes project keeps this distinction visible: its view requests storage through UmiDataServer and does not call SQLite directly.\n\nThe Master Controller composes the application and decides when its services are created and released. A small application-specific controller can translate a button click into a storage request. The shared Data Server remains the authority for persistence, and the view receives a result rather than reaching into a database handle.\n\nThis boundary is useful even in a small application. Another view can use the same storage contract, and a test can exercise that contract without opening a graphical window. A different backend need not change the meaning of the application’s request.\n\nThe project starts with a memory Data Server. Its contents deliberately disappear when the program ends. A later exercise may select SQLite explicitly, but a memory demonstration must never be described as durable storage.",
        "UmiDataServer *server = NULL;\nUmiStatus status = umi_data_server_create_memory(&server);\nif (status == UMI_STATUS_OK) {\n    status = umi_data_server_set(server, \"note\", \"Workshop plan\");\n}\numi_data_server_destroy(server);",
        "Export the framework project. Locate every umi_data_server call. Identify which object owns the server and when it is destroyed. Confirm that the example does not call a SQLite API.",
        "The view asks for a save; the service owns storage. Memory storage is intentionally not durable.",
        "framework",
        UMI_TEACHER_LANGUAGE_C23, 30U, {
            {"Who should own shared Umicom persistence behaviour?", {"Each button", "Framework Data Server", "A global application string"}, 1U, "Reusable persistence belongs to the Framework service boundary."},
            {"Does a memory Data Server survive program exit?", {"No", "Yes", "Only after a quiz"}, 0U, "Memory-only state is released with the process."},
            {"What is the Master Controller’s role here?", {"Draw every widget directly", "Bypass service checks", "Compose and coordinate the application lifecycle"}, 2U, "Composition and lifecycle coordination keep domain and view responsibilities separate."},
        }
    },
    {
        "framework.transaction",
        "framework",
        "foundations.framework",
        "2. Save related changes together",
        "A transaction groups related writes into one unit. Consider changing both a note and its category. Either both changes should become visible together, or the earlier state should remain. A half-applied update could mislead another panel that reads the category beside the text.\n\nThe Framework Data Server exposes begin, commit and rollback through its public C contract. Begin starts the unit of work. Commit accepts it. Rollback restores the previous state when the operation cannot complete. Every return value matters; issuing commit is not the same as proving it succeeded.\n\nThe reference project includes a rollback test. It saves an original note, begins a transaction, changes the note and rolls back. A later read must return the original note. That is observable behaviour, not merely a check that a function was called.\n\nThe Education Workspace uses this same Data Server for learning records. It saves an accepted action and its new revision in a transaction. When another open application has a newer revision, the stale writer is rejected so an old view cannot silently replace new progress.",
        "status = umi_data_server_begin(server);\nif (status == UMI_STATUS_OK) {\n    status = umi_data_server_set(server, \"note\", \"Updated plan\");\n    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);\n    else (void)umi_data_server_rollback(server);\n}",
        "Run the Framework project tests. Read the value before, inside and after the rollback. Explain why calling commit without checking its result is not enough.",
        "Follow the stored value, not just the function names. A failed commit needs explicit recovery handling.",
        "framework",
        UMI_TEACHER_LANGUAGE_C23, 35U, {
            {"What should rollback restore?", {"Only the last label", "The previous transaction state", "An arbitrary empty value"}, 1U, "Rollback undoes the uncommitted changes in that transaction."},
            {"Does calling commit guarantee success?", {"No; inspect its result", "Yes", "Only in a GUI"}, 0U, "An operation can fail; its return value is part of the contract."},
            {"An old view encounters newer saved progress. What is safer?", {"Overwrite it", "Guess a merged state", "Reject the stale write and reload"}, 2U, "Rejecting stale state prevents silent loss of newer learning actions."},
        }
    },
    {
        "framework.view",
        "framework",
        "foundations.gtk",
        "3. Connect the window to the service",
        "The optional GTK view has an editor, a Save button, a Load button and a status label. Those controls do different jobs. Typing changes the editor buffer; it does not save the note automatically. Clicking Save requests a service update and then displays the returned outcome.\n\nA signal connects a user action to a callback. The callback receives a borrowed pointer to application state. That state must remain alive for as long as the callback can run. Closing the window ends the application run before the owner releases its Data Server.\n\nThe view uses plain text for stored content and status messages. User-written note text must not be interpreted as a command or as trusted markup. A note containing angle brackets is still a note, not an instruction to execute something.\n\nThe GUI is optional so the shared storage example and tests can be built without GTK. Enabling the GUI requires the actual GTK development dependency. A headless pass does not establish that the buttons, focus handling or window lifetime have passed their graphical checks.",
        "/* The callback translates an action into a service request. */\n/* Save: read editor text -> Data Server set -> display status. */\n/* Load: Data Server get -> replace editor text only on success. */",
        "Enable the optional GTK view after the headless tests pass. Save a fictional note, edit the buffer without saving, then press Load. Confirm which version appears. Close the window and check that the process exits.",
        "Editor text and saved text are separate states. Keep callback context alive until the window can no longer use it.",
        "framework",
        UMI_TEACHER_LANGUAGE_C23, 40U, {
            {"Does typing in this example immediately save the note?", {"Yes", "No", "Only uppercase text"}, 1U, "Save is an explicit action; the editor buffer can differ from stored text."},
            {"How long must callback context remain alive?", {"Until the callback is connected", "For one line of code", "While the callback may still run"}, 2U, "The callback borrows the context and cannot extend its lifetime."},
            {"How should user-written note text be displayed?", {"As plain text", "As shell commands", "As trusted markup"}, 0U, "Stored user content is data, not executable or trusted presentation instructions."},
        }
    },
    {
        "framework.delivery",
        "framework",
        "foundations.change",
        "4. Keep the learning record and the build evidence honest",
        "You now have two useful records: your self-study quiz progress and the source plus tests of a real project. They answer different questions. The quiz checks your interpretation of a lesson. The project build and tests check the compiled implementation that you edited.\n\nUse Studio’s existing Guided Learning page to open the same new practicum window. The earlier foundations lesson cards remain available, so you can revisit a topic without losing that established route. The standalone Education Studio and Studio practicum use the same Framework learning-record service.\n\nBefore publishing a change, inspect exactly what is staged. Keep existing tests, comments and licence notices. Make a focused commit on the intended main branch only after validation. In a suite with submodules, push changed child repositories before the parent records their revisions.\n\nThe exported HTML learning record is a local self-study summary. It is not an identity-verified qualification, a signed audit trail or proof that every practical exercise ran. Its notes are plaintext, so do not add passwords, private customer data or live financial information.",
        "git status --short --branch\ngit diff --check\n/* Review changes, build and test before staging a focused contribution. */",
        "Export your learning record. Read its scope notice and check the quiz counts against the application. Open the three project folders in Studio and keep actual CTest output with your practical work.",
        "Keep claims tied to their evidence. A local quiz record is not a compiled-code result or an authenticated certificate.",
        "framework",
        UMI_TEACHER_LANGUAGE_C23, 30U, {
            {"What does the exported learning record prove?", {"A certified identity", "Your local saved quiz activity", "That every application builds"}, 1U, "The report describes local self-study activity, not independent certification or build evidence."},
            {"Which source material should remain during a reviewed update?", {"Only new code", "No comments", "Existing tests, comments and licence notices"}, 2U, "Preserving prior work protects behaviour, authorship and the engineering explanation."},
            {"Which repositories are pushed first in a submodule suite?", {"Changed child repositories", "The parent regardless of children", "An unrelated repository"}, 0U, "The parent should record child commits that have already been published."},
        }
    },
