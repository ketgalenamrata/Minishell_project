# Minishell_project
🖥️ MiniShell (Custom Unix Shell in C)
📌 Overview
This project implements a custom Unix-like command-line shell written in C. It replicates core functionalities of standard shells such as Bash, including command execution, process management, and basic shell operations.

The goal is to demonstrate a solid understanding of operating system concepts, including process creation, system calls, and command parsing.

⚙️ Key Features
🧾 Command parsing and execution
⚡ Process creation using fork() and exec()
⏳ Foreground and background process handling
📁 Built-in commands (e.g., cd, exit)
🔄 Input handling and command loop
🧠 Error handling for invalid commands
🧱 Core Concepts Used
System calls (fork, exec, wait)
Process lifecycle management
File descriptors and I/O
Command-line parsing
Unix/Linux environment interaction
📂 Project Structure
MiniShell/
│
├── src/        # Core shell implementation
├── include/    # Header files
├── Makefile    # Build configuration
└── README.md
🚀 How It Works
Shell displays a prompt and waits for user input
Input is parsed into command and arguments
A child process is created using fork()
The command is executed via exec()
Parent process waits (or runs in background)
Loop continues until exit command is issued
🛠️ Build & Run
Requirements:
GCC compiler
Linux / Unix-based system
Steps:
make
./minishell
⚠️ Limitations
No advanced piping (|) or redirection (if not implemented)
Limited built-in command support
Basic parsing (no comp
