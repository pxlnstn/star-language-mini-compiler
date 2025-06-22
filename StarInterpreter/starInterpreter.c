#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Define maximum sizes and other constants
#define MAX_IDENTIFIER_LENGTH 10
#define MAX_INTEGER_LENGTH 8
#define MAX_STRING_LENGTH 256
#define MAX_VARIABLES 100
#define MAX_TOKEN_LENGTH 256

// Define token types
enum TokenType {
    Identifier,
    IntConst,
    Operator,
    String,
    Keyword,
    EndOfLine,
    Comma,
    LeftCurlyBracket,
    RightCurlyBracket,
    Terminator
};

// Define data types for variables
enum VarType {
    Integer,
    Text
};

// Token structure
typedef struct {
    enum TokenType type;
    char value[MAX_STRING_LENGTH];
} Token;

// Variable structure
typedef struct {
    char name[MAX_IDENTIFIER_LENGTH + 1];
    enum VarType type;
    union {
        int intValue;
        char strValue[MAX_STRING_LENGTH];
    } value;
} Variable;

// Function prototypes
char* read_source_code(const char* filepath);
Token* tokenize_source_code(const char* source_code);
void write_tokens_to_file(Token* tokens, const char* filename);
void interpret(Token* tokens);
Variable* find_variable(const char* name);
void declare_variable(const char* name, enum VarType type);
void assign_variable(const char* name, const char* value);
void execute_statement(Token** tokens);
void handle_read(Token** tokens);
void handle_write(Token** tokens);
void handle_loop(Token** tokens);

// Global variable storage
Variable variables[MAX_VARIABLES];
int var_count = 0;

int main() {
    const char* source_code_file = "code.sta";
    char* source_code = read_source_code(source_code_file);
    Token* tokens = tokenize_source_code(source_code);
    interpret(tokens);

    free(source_code);
    free(tokens);
    return 0;
}

// Function to read source code from file
char* read_source_code(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* source_code = (char*)malloc((file_size + 1) * sizeof(char));
    if (source_code == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    fread(source_code, sizeof(char), file_size, file);
    source_code[file_size] = '\0';

    fclose(file);

    return source_code;
}

// Function to check if a character is a valid identifier character
int is_valid_identifier_char(char ch) {
    return isalnum(ch) || ch == '_';
}

// Function to tokenize source code
Token* tokenize_source_code(const char* source_code) {
    Token* tokens = (Token*)malloc(MAX_STRING_LENGTH * sizeof(Token));
    if (tokens == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    int num_tokens = 0;
    const char* ptr = source_code;
    bool in_comment = false; // Flag to track if we are inside a comment

    while (*ptr != '\0') {
        if (isspace(*ptr)) {
            ptr++;
            continue; // Skip whitespace
        }

        // Comments
        if (*ptr == '/' && *(ptr + 1) == '*') {
            in_comment = true; // Set the flag to true to mark start of comment
            ptr += 2; // Skip the opening comment characters
            continue; // Continue to the next character
        }

        // If we are inside a comment, skip characters until we find the end of the comment
        if (in_comment) {
            while (*ptr != '*' || *(ptr + 1) != '/') {
                if (*ptr == '\0') {
                    // If the comment doesn't terminate before the end of the file, lexical error
                    fprintf(stderr, "Lexical error: Unterminated comment\n");
                    exit(EXIT_FAILURE);
                }
                ptr++;
            }
            ptr += 2; // Skip the closing comment characters
            in_comment = false; // Reset the flag as we've reached the end of the comment
            continue; // Continue to the next character
        }

        // Keywords and Identifiers
        if (isalpha(*ptr)) {
            int i = 0;
            char keyword[MAX_IDENTIFIER_LENGTH + 1];
            char identifier[MAX_IDENTIFIER_LENGTH + 1]; // Maximum length + 1 for null terminator

            while ((isalpha(*ptr) || *ptr == '_') && i < MAX_IDENTIFIER_LENGTH) {
                keyword[i++] = *ptr++;
            }
            keyword[i] = '\0';

            // Check if the word is a keyword
            if (strcmp(keyword, "int") == 0 || strcmp(keyword, "text") == 0 ||
                strcmp(keyword, "is") == 0 || strcmp(keyword, "loop") == 0 ||
                strcmp(keyword, "times") == 0 || strcmp(keyword, "read") == 0 ||
                strcmp(keyword, "write") == 0 || strcmp(keyword, "newLine") == 0) {
                tokens[num_tokens].type = Keyword;
                strcpy(tokens[num_tokens].value, keyword);
                num_tokens++;
            } else {
                if (isalpha(*ptr) || *ptr == '_') {
                    fprintf(stderr, "Lexical error: Identifier exceeds maximum length\n");
                    exit(EXIT_FAILURE);
                } else {
                    tokens[num_tokens].type = Identifier;
                    strcpy(tokens[num_tokens].value, keyword);
                    num_tokens++;
                }
            }
        }

        // Integer constant
        else if (isdigit(*ptr) || (*ptr == '-' && isdigit(*(ptr + 1)))) {
            int i = 0;
            if (*ptr == '-') {
                tokens[num_tokens].value[i++] = *ptr++; // Include the minus sign
            }
            while (isdigit(*ptr) && i < MAX_INTEGER_LENGTH + 1) {
                tokens[num_tokens].value[i++] = *ptr++;
            }

            if (i > MAX_INTEGER_LENGTH) {
                fprintf(stderr, "Lexical error: Integer constant exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }

            tokens[num_tokens].value[i] = '\0';
            int value = atoi(tokens[num_tokens].value);
            if (value < 0) {
                value = 0;
                fprintf(stderr, "Lexical warning: Integer constant forced to zero\n");
            }
            sprintf(tokens[num_tokens].value, "%d", value);
            tokens[num_tokens++].type = IntConst;
        }

        // String constants
        else if (*ptr == '"') {
            int i = 0;
            *ptr++;
            while (*ptr != '"' && *ptr != '\0' && i < MAX_STRING_LENGTH) {
                tokens[num_tokens].value[i++] = *ptr++;
            }
            if (*ptr == '"') {
                *ptr++;
            }
            tokens[num_tokens].value[i] = '\0';
            tokens[num_tokens].type = String;

            if (i >= MAX_STRING_LENGTH) {
                fprintf(stderr, "Lexical error: String constant exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }

            if (*ptr == '\0') {
                fprintf(stderr, "Lexical error: Unterminated string constant\n");
                exit(EXIT_FAILURE);
            }
            num_tokens++;
        }

        // End of line
        else if (*ptr == '.') {
            tokens[num_tokens++].type = EndOfLine;
            ptr++;
        }

        // Comma
        else if (*ptr == ',') {
            tokens[num_tokens++].type = Comma;
            ptr++;
        }

        // Operator tokens
        else if (*ptr == '+' || *ptr == '-' || *ptr == '*') {
            tokens[num_tokens].type = Operator;
            tokens[num_tokens].value[0] = *ptr;
            tokens[num_tokens].value[1] = '\0';
            num_tokens++;
            ptr++;
        }

        // Brackets
        else if (*ptr == '{') {
            tokens[num_tokens++].type = LeftCurlyBracket;
            ptr++;
        }
        else if (*ptr == '}') {
            tokens[num_tokens++].type = RightCurlyBracket;
            ptr++;
        }

        // Move to next character
        else {
            ptr++;
        }
    }

    // Add terminator token
    tokens[num_tokens].type = Terminator;
    strcpy(tokens[num_tokens].value, "");

    return tokens;
}

// Function to interpret tokens
void interpret(Token* tokens) {
    Token* current_token = tokens;

    while (current_token->type != Terminator) {
        execute_statement(&current_token);
    }
}

void assign_variable(const char* name, const char* value) {
    Variable* var = find_variable(name);
    if (var == NULL) {
        fprintf(stderr, "Error: Variable %s not declared.\n", name);
        exit(EXIT_FAILURE);
    }

    // Check if the value contains an arithmetic expression
    if (strchr(value, '+') || strchr(value, '-')) {
        char var_value[MAX_STRING_LENGTH];
        int result = evaluate_expression(value); // Implement evaluate_expression function
        if (result < 0) result = 0;
        sprintf(var_value, "%d", result);
        if (var->type == Integer) {
            var->value.intValue = result;
        } else {
            strncpy(var->value.strValue, var_value, MAX_STRING_LENGTH);
            var->value.strValue[MAX_STRING_LENGTH - 1] = '\0';
        }
    } else {
        if (var->type == Integer) {
            var->value.intValue = atoi(value);
            if (var->value.intValue < 0) {
                var->value.intValue = 0;
                fprintf(stderr, "Warning: Negative value forced to zero for variable %s.\n", name);
            }
        } else {
            strncpy(var->value.strValue, value, MAX_STRING_LENGTH);
            var->value.strValue[MAX_STRING_LENGTH - 1] = '\0';
        }
    }
}


void execute_statement(Token** tokens) {
    Token* current_token = *tokens;

    // Handle variable declarations and assignments
    if (current_token->type == Keyword && (strcmp(current_token->value, "int") == 0 || strcmp(current_token->value, "text") == 0)) {
        enum VarType var_type = (strcmp(current_token->value, "int") == 0) ? Integer : Text;
        current_token++;
        while (current_token->type == Identifier) {
            char var_name[MAX_IDENTIFIER_LENGTH + 1];
            strcpy(var_name, current_token->value);
            current_token++;
            if (current_token->type == Keyword && strcmp(current_token->value, "is") == 0) {
                current_token++;
                char var_value[MAX_STRING_LENGTH];
                strcpy(var_value, current_token->value);
                current_token++;
                declare_variable(var_name, var_type);
                assign_variable(var_name, var_value);
            } else {
                declare_variable(var_name, var_type);
            }
            if (current_token->type == Comma) {
                current_token++;
            }
        }
    } else if (current_token->type == Identifier) {
        char var_name[MAX_IDENTIFIER_LENGTH + 1];
        strcpy(var_name, current_token->value);
        current_token++;
        if (current_token->type == Keyword && strcmp(current_token->value, "is") == 0) {
            current_token++;
            char expression[MAX_STRING_LENGTH];
            strcpy(expression, current_token->value);
            current_token++;
            while (current_token->type == Operator || current_token->type == Identifier || current_token->type == IntConst) {
                strcat(expression, " ");
                strcat(expression, current_token->value);
                current_token++;
            }
            int result = evaluate_expression(expression);
            char result_str[MAX_STRING_LENGTH];
            sprintf(result_str, "%d", result);
            assign_variable(var_name, result_str);
        }
    } else if (current_token->type == Keyword) {
        if (strcmp(current_token->value, "read") == 0) {
            current_token++;
            handle_read(&current_token);
        } else if (strcmp(current_token->value, "write") == 0 || strcmp(current_token->value, "newLine") == 0) {
            handle_write(&current_token);
        } else if (strcmp(current_token->value, "loop") == 0) {
            handle_loop(&current_token);
        }
    }

    // Handle end of line
    if (current_token->type == EndOfLine) {
        current_token++;
    }

    *tokens = current_token;
}


// Function to handle read statement
void handle_read(Token** tokens) {
    Token* current_token = *tokens;
    while (current_token->type == Identifier) {
        char var_name[MAX_IDENTIFIER_LENGTH + 1];
        strcpy(var_name, current_token->value);
        current_token++;
        Variable* var = find_variable(var_name);
        if (var == NULL) {
            fprintf(stderr, "Runtime error: Variable %s not declared\n", var_name);
            exit(EXIT_FAILURE);
        }
        if (var->type == Integer) {
            int value;
            printf("Enter integer value for %s: ", var_name);
            scanf("%d", &value);
            var->value.intValue = value;
        } else {
            char value[MAX_STRING_LENGTH];
            printf("Enter string value for %s: ", var_name);
            scanf("%s", value);
            strcpy(var->value.strValue, value);
        }
        if (current_token->type == Comma) {
            current_token++;
        } else {
            break;
        }
    }
    if (current_token->type == EndOfLine) {
        current_token++;
    }
    *tokens = current_token;
}

// Function to handle write and newLine statements
void handle_write(Token** tokens) {
    Token* current_token = *tokens;
    if (strcmp(current_token->value, "write") == 0) {
        current_token++;
        while (current_token->type == Identifier || current_token->type == String) {
            if (current_token->type == Identifier) {
                Variable* var = find_variable(current_token->value);
                if (var == NULL) {
                    fprintf(stderr, "Runtime error: Variable %s not declared\n", current_token->value);
                    exit(EXIT_FAILURE);
                }
                if (var->type == Integer) {
                    printf("%d", var->value.intValue);
                } else {
                    printf("%s", var->value.strValue);
                }
            } else {
                printf("%s", current_token->value);
            }
            current_token++;
            if (current_token->type == Comma) {
                current_token++;
            } else {
                break;
            }
        }
        if (current_token->type == EndOfLine) {
            current_token++;
        }
    } else if (strcmp(current_token->value, "newLine") == 0) {
        printf("\n");
        current_token++;
        if (current_token->type == EndOfLine) {
            current_token++;
        }
    }
    *tokens = current_token;
}

// Function to handle loop statement
void handle_loop(Token** tokens) {
    Token* current_token = *tokens;
    current_token++;
    if (current_token->type == IntConst) {
        int loop_count = atoi(current_token->value);
        current_token++;
        if (strcmp(current_token->value, "times") == 0) {
            current_token++;
            if (current_token->type == LeftCurlyBracket) {
                current_token++;
                Token* loop_body_start = current_token;
                for (int i = 0; i < loop_count; i++) {
                    current_token = loop_body_start;
                    while (current_token->type != RightCurlyBracket) {
                        execute_statement(&current_token);
                    }
                }
                current_token++;
            }
        }
    }
    *tokens = current_token;
}

// Function to find a variable by name
Variable* find_variable(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

// Function to declare a new variable
void declare_variable(const char* name, enum VarType type) {
    if (find_variable(name) != NULL) {
        fprintf(stderr, "Semantic error: Variable already declared: %s\n", name);
        exit(EXIT_FAILURE);
    }
    if (var_count >= MAX_VARIABLES) {
        fprintf(stderr, "Semantic error: Too many variables declared\n");
        exit(EXIT_FAILURE);
    }
    Variable* var = &variables[var_count++];
    strncpy(var->name, name, MAX_IDENTIFIER_LENGTH);
    var->name[MAX_IDENTIFIER_LENGTH] = '\0';
    var->type = type;
    if (type == Integer) {
        var->value.intValue = 0;
    } else {
        var->value.strValue[0] = '\0';
    }
}

int evaluate_expression(const char* expr) {
    char buffer[MAX_STRING_LENGTH];
    strncpy(buffer, expr, MAX_STRING_LENGTH);
    buffer[MAX_STRING_LENGTH - 1] = '\0';

    char* token = strtok(buffer, " ");
    int result = 0;
    char op = '+';

    while (token != NULL) {
        int value;

        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            // Token is an integer constant
            value = atoi(token);
        } else {
            // Token is a variable name
            Variable* var = find_variable(token);
            if (var == NULL) {
                fprintf(stderr, "Semantic error: Undefined variable: %s\n", token);
                exit(EXIT_FAILURE);
            }
            if (var->type != Integer) {
                fprintf(stderr, "Semantic error: Variable is not an integer: %s\n", token);
                exit(EXIT_FAILURE);
            }
            value = var->value.intValue;
        }

        switch (op) {
            case '+': result += value; break;
            case '-': result -= value; break;
            case '*': result *= value; break;
            case '/': 
                if (value == 0) {
                    fprintf(stderr, "Runtime error: Division by zero\n");
                    exit(EXIT_FAILURE);
                }
                result /= value; 
                break;
            default:
                fprintf(stderr, "Semantic error: Unknown operator: %c\n", op);
                exit(EXIT_FAILURE);
        }

        token = strtok(NULL, " ");
        if (token != NULL) {
            op = token[0];
            token = strtok(NULL, " ");
        }
    }

    return result;
}







