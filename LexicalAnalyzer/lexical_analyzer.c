#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_IDENTIFIER_LENGTH 10
#define MAX_INTEGER_LENGTH 8
#define MAX_STRING_LENGTH 256



// Token types
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


typedef struct {
    enum TokenType type;
    char value[MAX_STRING_LENGTH];
} Token;

// Function to check if a character is a valid identifier character
int is_valid_identifier_char(char ch) {
    return isalnum(ch) || ch == '_';
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

        // Keywords
        if (isalpha(*ptr)) {
            int i = 0;
            char keyword[MAX_IDENTIFIER_LENGTH + 1];
            char identifier[MAX_IDENTIFIER_LENGTH + 1]; // Maximum length + 1 for null terminator

            while ((isalpha(*ptr) || *ptr == "_") && i < MAX_IDENTIFIER_LENGTH) {
                keyword[i++] = *ptr++;
        
            }
            keyword[i] = '\0';

            // Check if the word is a keyword
            if (strcmp(keyword, "int") == 0 || strcmp(keyword, "text") == 0 ||
                strcmp(keyword, "is") == 0 || strcmp(keyword, "loop") == 0 ||
                strcmp(keyword, "times") == 0 || strcmp(keyword, "read") == 0 ||
                strcmp(keyword, "write") == 0 || strcmp(keyword, "newLine") == 0) {
                tokens[num_tokens].type = Keyword;

                // Assign the token value
                strcpy(tokens[num_tokens].value, keyword);
                num_tokens++;

            } 
            else {
                
                if(isalpha(*ptr) || *ptr == "_"){

                // Identifier exceeds maximum length, issue an error message
                fprintf(stderr, "Lexical error: Identifier exceeds maximum length\n");
                exit(EXIT_FAILURE);
                }
                else{
                    tokens[num_tokens].type = Identifier; // If not a keyword, consider it as an identifier
                    strcpy(tokens[num_tokens].value, keyword);
                    num_tokens++;
                }
            }

            
        }

        // Integer constant
        else if (isdigit(*ptr) || (*ptr == '-' && isdigit(*(ptr + 1)))) {
            int i = 0;
            // Handle the case where the minus sign is part of the integer constant
            if (*ptr == '-') {
                tokens[num_tokens].value[i++] = *ptr++; // Include the minus sign
            }
            while (isdigit(*ptr) && i < MAX_INTEGER_LENGTH + 1) { // Allow one extra character for the sign
                tokens[num_tokens].value[i++] = *ptr++;
            }

            // Check if the integer constant exceeds the maximum length
            if (i > MAX_INTEGER_LENGTH) {
                fprintf(stderr, "Lexical error: Integer constant exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }

            tokens[num_tokens].value[i] = '\0';

            // Convert the string to an integer
            int value = atoi(tokens[num_tokens].value);

            // Check if the integer is negative
            if (value < 0) {
                value = 0; // Force negative values to zero
                fprintf(stderr, "Lexical warning: Integer constant forced to zero\n");
            }

            // Convert the integer back to a string and update the token value
            sprintf(tokens[num_tokens].value, "%d", value);

            tokens[num_tokens++].type = IntConst;
        }


        // String constants
        else if (*ptr == '"') {
            int i = 0;
            tokens[num_tokens].value[i++] = *ptr++; // Include opening quote
            while (*ptr != '"' && *ptr != '\0' && i < MAX_STRING_LENGTH) {
                tokens[num_tokens].value[i++] = *ptr++;
            }
            if (*ptr == '"') {
                tokens[num_tokens].value[i++] = *ptr++; // Include closing quote
            }
            tokens[num_tokens].value[i] = '\0';
            tokens[num_tokens].type = String;

            // Check if the string constant exceeds 256 characters
            if (i >= MAX_STRING_LENGTH) {
                fprintf(stderr, "Lexical error: String constant exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }

            // Check if a string constant cannot terminate before the file end
            if (*ptr == '\0') {
                fprintf(stderr, "Lexical error: Unterminated string constant\n");
                exit(EXIT_FAILURE);
            }

            num_tokens++;
        }


        // Keywords
        else if (isalpha(*ptr)) {
            int i = 0;
            while (isalpha(*ptr) && i < MAX_STRING_LENGTH) {
                tokens[num_tokens].value[i++] = *ptr++;
            }
            tokens[num_tokens].value[i] = '\0';
            tokens[num_tokens++].type = Keyword;
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
        else if (*ptr == '+' || *ptr == '-' || *ptr == '*' || *ptr == '/') {
            tokens[num_tokens].type = Operator;
            tokens[num_tokens].value[0] = *ptr;
            tokens[num_tokens].value[1] = '\0'; // Null terminate the string
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

// Function to write tokens to output file
void write_tokens_to_file(Token* tokens, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (tokens[i].type != Terminator) {
        // Print the token's type based on the enum value
        switch(tokens[i].type) {
            case Identifier:
                fprintf(file, "Identifier(");
                break;
            case IntConst:
                fprintf(file, "IntConst(");
                break;
            case Operator:
                fprintf(file, "Operator(");
                break;
            case String:
                fprintf(file, "String(");
                break;
            case Keyword:
                fprintf(file, "Keyword(");
                break;
            case EndOfLine:
                fprintf(file, "EndOfLine");
                break;
            case Comma:
                fprintf(file, "Comma");
                break;
            case LeftCurlyBracket:
                fprintf(file, "LeftCurlyBracket(");
                break;
            case RightCurlyBracket:
                fprintf(file, "RightCurlyBracket(");
                break;
            default:
                fprintf(file, "Unknown(");
                break;
        }
    
        
        if (tokens[i].type != Comma && tokens[i].type != EndOfLine) {
            fprintf(file, "%s)", tokens[i].value);
        }
        fprintf(file, "\n");
        i++;
    }

    fclose(file);
}



int main() {
    const char* source_code_file = "code.sta";
    const char* output_file = "code.lex";

    char* source_code = read_source_code(source_code_file);
    Token* tokens = tokenize_source_code(source_code);
    write_tokens_to_file(tokens, output_file);

    printf("Lexical analysis completed. Tokens written to %s\n", output_file);

    free(source_code);
    free(tokens);
    return 0;
}