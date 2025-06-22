# Interpreter

A console-based interpreter written in C for the custom scripting language **STAR**. This component parses and executes STAR programs line by line, obeying the language’s syntax and semantics.

---

## 🔧 What It Does

* Reads STAR source code (`.sta` files) and interprets each line
* Supports integer and text variable declarations and assignments
* Performs arithmetic operations with two operands only
* Executes read/write/newLine commands via console
* Handles simple `loop ... times` control flow, with or without code blocks
* Supports nested loops and inline comments
* Detects and reports runtime errors: uninitialized variables, string overflow, invalid input

---

## 🎓 STAR Language Semantics Overview

**Variable Declaration**

* `int` and `text` types
* Declared with or without initialization using constants
* Multiple variables can be declared/initialized on one line
* All variables must be declared before use

```star
int a, b.
text message is "Hello".
int x is 5, y is 10.
```

**Assignments**

* Use the `is` keyword
* Right-hand side can be:

  * Constant value
  * Another variable
  * Simple arithmetic with two operands (e.g., `a + 1`)
* Negative results are automatically coerced to 0

**Text Operations**

* `+` concatenates strings
* `-` removes the first occurrence of the second string from the first

**Input / Output**

* `read` optionally accepts prompt text
* If input is invalid (non-integer), 0 is assigned with a warning
* `write` prints values separated by commas
* `newLine` prints a line break

**Loops**

* Use `loop N times` syntax
* Optional code blocks with `{}`
* Loops can be nested

```star
loop 5 times write "*".

int i.
i is 1.
loop 10 times {
  write i.
  newLine.
  i is i + 1.
}
```

**Comments**

* Between `/*` and `*/`, can span multiple lines
* Ignored by interpreter

---

## 📁 Files

* `starInterpreter.c` — interpreter implementation in C
* `code.sta` — sample STAR program
---

## ⚠️ Runtime Behavior & Constraints

* Strings over 256 characters are truncated
* Integers over 99999999 cause an error
* All variables are global scope
* Syntax is strict and case-sensitive
* Expressions must be simple (two operands max)

---

## 🌐 Example

```star
int i.
i is 1.
loop 5 times {
  write i.
  newLine.
  i is i + 1.
}
```

**Output:**

```
1
2
3
4
5
```
