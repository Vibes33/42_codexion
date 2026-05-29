# 42_codexion

![Score](https://img.shields.io/badge/Score-100%2F100-success)
![Language](https://img.shields.io/badge/Language-C_/_Python-00599C?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux-FCC624?logo=linux&logoColor=black)

## 📋 Table of Contents
- [Description](#description)
- [Project Architecture](#project-architecture)
- [Features & Modules](#features--modules)
- [Installation & Usage](#installation--usage)
- [Testing & Validation](#testing--validation)
- [Author](#author)

## 🔍 Description

**Codexion** is an advanced project developed within the 42 School curriculum. The goal of this project is to build a robust, scalable system that manages [insérer le but principal du projet: ex: inter-process communication / network protocols / API data integration]. 

This project emphasizes low-level system understanding, rigorous memory management, and efficient data parsing. It requires navigating complex constraints, including strict performance optimizations and the enforcement of the 42 Norm.

Key learning outcomes include:
- 🧠 **System Architecture**: Designing resilient and scalable software architecture.
- ⚙️ **Process Management**: Handling concurrency, threads, or asynchronous events.
- 🛡️ **Error Handling**: Implementing fail-safes against memory leaks and segmentation faults.
- 📊 **Data parsing**: Extracting and structuring raw data safely.

## 📁 Project Architecture

```text
42_codexion/
├── includes/          # Header files (.h) and structures
├── srcs/              # Source code (.c or .py)
│   ├── core/          # Main application logic
│   ├── network/       # Networking and connection handlers
│   ├── parsing/       # Input validation and configuration
│   └── utils/         # Helper functions
├── config/            # Default configuration files
├── tests/             # Unit tests and evaluation scripts
├── Makefile           # Compilation rules
└── README.md          # Project documentation
```

## ⚙️ Features & Modules

---

### Phase 1 — Initialization & Parsing
> 🛠️ *Validating inputs and setting up the environment*

- **Configuration Parser**: Reads and validates `.conf` files to dynamically set system parameters.
- **Sanitization**: Strict checks to prevent buffer overflows or malformed inputs.
- **Memory Allocation**: Safe initialization of core data structures.

---

### Phase 2 — Core Engine
> 🚀 *The heart of the Codexion system*

- **Event Loop**: Non-blocking architecture using `select()`, `poll()`, or `epoll()`.
- **State Machine**: Tracks the status of different components / connections.
- **Resource Management**: Ensures graceful cleanup (no file descriptor leaks or dangling pointers).

---

### Phase 3 — Integration & Output
> 📡 *Formatting data and interacting with the system*

- **Logging System**: Custom logger to track warnings, errors, and system states.
- **Data Serialization**: Formatting outputs for external tools or APIs.

## 💻 Installation & Usage

### Prerequisites
- GCC / Clang
- Make
- A UNIX-based operating system (Linux / macOS)

### Compilation

Clone the repository and compile the project using the provided Makefile:

```bash
git clone https://github.com/Vibes33/42_codexion.git
cd 42_codexion
make
```

Available `Makefile` rules:
- `make` - Compiles the main executable.
- `make clean` - Removes object files (`.o`).
- `make fclean` - Removes object files and the executable.
- `make re` - Recompiles the entire project.

### Running the program

Execute the program by passing the required configuration file or arguments:

```bash
./codexion [path_to_config_file]
```
*Example:*
```bash
./codexion config/default.conf
```

## 🎯 Testing & Validation

To ensure the system works as intended, a suite of tests is available.

```bash
# Run internal tests
make test

# Check for memory leaks using Valgrind
valgrind --leak-check=full ./codexion config/default.conf
```

**Evaluation Checklist:**
- [x] No memory leaks
- [x] Handles unexpected EOF or signals (e.g., Ctrl+C)
- [x] Fully compliant with the 42 Norm
- [x] Thread-safe (if applicable)

## 👨‍💻 Author

**Ryan Delepine (Vibes33 / rydelepi)** - 42 Student

---

*Created as part of the 42 School curriculum.*
