# STAR Language Mini Compiler ⭐

A console-based two-stage mini compiler written in C for the custom scripting language **STAR**. Developed as part of a university-level compiler design course.

---

## 🔧 Project Overview

The compiler consists of two main components:

* **Project 1: Lexical Analyzer**
* **Project 2: Interpreter**

Each phase has its own directory, source files, test cases, and README.

---

## 🚀 About STAR Language

**STAR** is a simplified scripting language supporting:

* `int` and `text` data types
* Basic arithmetic and string operations
* Input/output commands: `read`, `write`, `newLine`
* Fixed-syntax control flow with `loop ... times`

### Example STAR Code

```star
int a, b.
read "Enter first:" a.
read "Enter second:" b.
c is a + b.
write "Result is:", c.
newLine.
```

---

## 📂 Project Structure

```
/LexicalAnalyzer/         - Lexical analyzer that tokenizes STAR source code
/StarInterpreter/   - Interpreter that executes STAR programs line by line
```

Each folder contains its own README, source code, sample input, and optional report.

---

## 🧠 Key Skills Demonstrated

* Lexical analysis and token classification
* Syntax-aware interpretation
* C programming with structured error handling
* File I/O and text parsing

---

## 👥 Contributors

* Pelin Su Üstün
* Gülay Ertan
