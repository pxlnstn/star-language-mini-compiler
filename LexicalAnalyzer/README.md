# Lexical Analyzer

A console-based lexical analyzer written in C for the custom scripting language **STAR**. This component reads a `.sta` file and produces a `.lex` file with tokenized output according to STAR's lexical rules.

---

## 🔧 What It Does

* Reads `code.sta` as input (a STAR source file)
* Processes each line character by character
* Classifies tokens such as:

  * Identifiers
  * Keywords
  * Operators
  * String constants
  * Integer constants
  * Punctuation (periods, commas, brackets)
* Writes corresponding tokens line by line to `code.lex`
* Enforces STAR language lexical rules strictly, and issues errors when rules are violated

---

## 📊 STAR Lexical Rules Summary

**Identifiers**

* Max 10 characters, case sensitive
* Start with a letter, can include letters/digits/underscores
* Example: `Identifier(my_var_1)`

**Integer Constants**

* Max 8 digits, no negatives allowed
* Example: `IntConst(1290)`

**Operators**

* Allowed: `+`, `-`, `*`, `/`
* Example: `Operator(*)`

**Brackets**

* `{` and `}` used in loop blocks
* Example: `LeftCurlyBracket`, `RightCurlyBracket`

**String Constants**

* Enclosed in double quotes (`"`), max 256 characters
* Cannot include quotes inside; unterminated or overlong strings → lexical error
* Example: `String("Hello!")`

**Keywords** (all lowercase, case sensitive)

* `int`, `text`, `is`, `loop`, `times`, `read`, `write`, `newLine`
* Example: `Keyword(int)`

**End of Line**

* `.` marks end of a command
* Example: `EndOfLine`

**Comma**

* Used for separating values in declarations and write statements

**Comments**

* `/* ... */`, no token generated
* Unterminated comment → lexical error

---

## 📁 Files

* `lexical_analyzer.c` — main source code for lexical analysis
* `code.sta` — sample STAR source input
* `code.lex` — output file with token list
* `Report.docx` — (optional) documentation and testing info

---

## 🔄 Example Input and Output

### Input (`code.sta`):

```star
c is c / 2.
```

### Output (`code.lex`):

```
Identifier(c)
Keyword(is)
Identifier(c)
Operator(/)
EndOfLine
```

---

## ⚠️ Known Limitations

* No nested expressions allowed
* No error recovery — lexer stops at first critical violation
* Output is plain-text token list (not XML/JSON)
