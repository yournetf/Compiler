#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>

#define MAX_STACK_SIZE 100

#define NUM_VARS 100  
float vars[NUM_VARS];

/* Global declarations */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp;
FILE *out_fp;
FILE *tree_fp;
FILE* postorder_fp;
FILE* ICG_fp;

enum labelType{
    identifier,
    constant,
    op,
};

struct TreeNode {
    labelType label;
    int tokenCode;
    std::string value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(labelType l = identifier, int tknCode = 0, const std::string& val = "", TreeNode* leftChild = nullptr, TreeNode* rightChild = nullptr)
        : label(l), tokenCode(tknCode), value(val), left(leftChild), right(rightChild) {}

    void printNode(){   
        printf("TreeNode: \n");
        printf(" -------- \n");
        printf("|   %d   |\n", label);
        printf(" -------- \n");
        printf("|   %d   |\n", tokenCode);
        printf(" -------- \n");
    }
};


/* Stack structure for postfix evaluation */
float stack[MAX_STACK_SIZE];
int stackTop = -1;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void error();
TreeNode* assignment();
TreeNode* factor();
TreeNode* term();
TreeNode* expr();
TreeNode* exp(); 
void evalInfix(); 
int evalPostfix(const char *postfix); 
TreeNode* makeNode();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define FLOAT_LIT 12
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define EXP_OP 25
#define LEFT_PAREN 26
#define RIGHT_PAREN 27
#define EOF_TOKEN 28
#define TILDE_OP 29
#define MOD_OP 30

/* Tree functions for creating syntax tree */
TreeNode* makeNode(labelType l, int tknCode, const std::string& val = "",TreeNode* leftChild = nullptr, TreeNode* rightChild = nullptr) {
    return new TreeNode(l, tknCode, val, leftChild, rightChild);
}

/* Stack utility functions for postfix evaluation */
void push(float value) {
    if (stackTop < MAX_STACK_SIZE - 1)
        stack[++stackTop] = value;
    else
        fprintf(out_fp, "Stack overflow\n");
}

float pop() {
    if (stackTop >= 0)
        return stack[stackTop--];
    else {
        fprintf(out_fp, "Stack underflow\n");
        return 0.0;
    }
}

void assignVariable(char var, float value) {
    int index = var - 'a';  
    if (index >= 0 && index < NUM_VARS) {
        vars[index] = value;
        fprintf(out_fp, "Assigned %.6g to %c\n", value, var);
    } else {
        fprintf(out_fp, "Error: Invalid variable '%c'\n", var);
    }
}

float getVariable(char var) {
    int index = var - 'a';
    if (index >= 0 && index < NUM_VARS) {
        return vars[index];
    } else {
        fprintf(out_fp, "Error: Variable '%c' not found\n", var);
        return 0.0;
    }
}


// Helper function to print float values without trailing zeros
void printFloatWithoutTrailingZeros(float value) {
    fprintf(ICG_fp, "%.6g", value);
}

/* printStack - a function to print the current stack vertically */
void printStack() {
    fprintf(ICG_fp, "Current stack (top to bottom):\n");
    if (stackTop == -1) {
        fprintf(ICG_fp, "Stack is empty.\n\n");
    } else {
        for (int i = stackTop; i >= 0; i--) {
            if (i == stackTop) {
                fprintf(ICG_fp, "--> ");
                printFloatWithoutTrailingZeros(stack[i]);
                fprintf(ICG_fp, " (top)\n");  
            } else {
                fprintf(ICG_fp, "    ");
                printFloatWithoutTrailingZeros(stack[i]);
                fprintf(ICG_fp, "\n");
            }
        }
        fprintf(ICG_fp, "\n");
    }
}


/* main driver */
int main() {
    // Open files
    out_fp = fopen("output.txt", "w");
    tree_fp = fopen("tree.txt", "w");
    postorder_fp = fopen("postorder.txt", "w");
    ICG_fp = fopen("intermediateCode.txt", "w");
    if (out_fp == NULL) {
        printf("ERROR - cannot open output.txt for writing\n");
        return -1;
    }
    // Open input file
    if ((in_fp = fopen("front.in", "r")) == NULL)
        fprintf(out_fp, "ERROR - cannot open front.in \n");
    else {
        // fprintf(out_fp, "PART A (test case 1): \n\n");
        // fprintf(out_fp, "Evaluating postfix expression ( 6 ~ 6 2 ^ 4 1 * 8 * - .5 ^ + 2 1 * /):\n");
        // fprintf(out_fp, "\n");
        // float result = evalPostfix(" 6 ~ 6 2 ^ 4 1 * 8 * - .5 ^ + 2 1 * /");
        // fprintf(out_fp, "Postfix evaluation result: %.6g\n", result);
        // fprintf(out_fp, "-------------------------------------\n");

        // fprintf(out_fp, "PART A (test case 2): \n\n");
        // fprintf(out_fp, "Evaluating postfix expression (a 1 = b 6 = c 8 =  b ~ b 2 ^ 4 a * c * - .5 ^ + 2 a * /):\n");
        // fprintf(out_fp, "\n");
        // result = evalPostfix("a 1 = b 6 = c 8 =  b ~ b 2 ^ 4 a * c * - .5 ^ + 2 a * /");
        // fprintf(out_fp, "Postfix evaluation result: %.6g\n", result);
        // fprintf(out_fp, "-------------------------------------\n");

        // fprintf(out_fp, "PART B: \n");
        fprintf(out_fp, "Evaluating infix expression:\n");
        evalInfix();     
        
        // Close files
        fclose(postorder_fp);
        fclose(tree_fp);

        // Open the result of postorder.txt as an input file, as opposed to a output file.
        std::ifstream postorderIn_fp("postorder.txt");
        if (!postorderIn_fp.is_open()) {
            std::cerr << "ERROR - cannot open postorder.txt\n";
        } else {
            std::string line;
            while (std::getline(postorderIn_fp, line)) {
                evalPostfix(line.c_str());
            }
            postorderIn_fp.close();
            }
        }
    return 0;
}

void postorderTraversal(TreeNode* node, FILE* file) {
    if (node == nullptr) {
        return;
    }

    // Recursively traverse the left subtree
    postorderTraversal(node->left, file);
    
    // Recursively traverse the right subtree
    postorderTraversal(node->right, file);
    
    
    // Write the current node's value to the file
    if (!node->value.empty()) {
        fprintf(file, "%s", node->value.c_str());
    } else {
        fprintf(file, "Token: %d\n", node->tokenCode);
    }
    
}


/* evalPostfix - Evaluate a postfix expression */
int evalPostfix(const char *postfix) {
    const char *ch = postfix;
    while (*ch != '\0') {
        if (isdigit(*ch) || *ch == '.') {
            printStack();
            char numBuffer[32];
            int numIndex = 0;

            while (isdigit(*ch) || *ch == '.') {
                numBuffer[numIndex++] = *ch;
                ch++;
            }
            numBuffer[numIndex] = '\0';  

            float num = atof(numBuffer);  
            fprintf(ICG_fp, "Pushing %.6g onto stack\n", num);
            push(num);
        } else if (*ch == '=') {
            printStack();  
            float value = pop();  
            float varCode = pop();  
            char var = (char)varCode;  
            assignVariable(var, value);  
            push(value);  
            fprintf(ICG_fp, "Assigned %.6g to variable %c\n", value, var);
            printStack();  
        } else if (isalpha(*ch)) {
            fprintf(ICG_fp, "Pushing variable '%c' onto stack\n", *ch);
            push(getVariable((char)*ch));  
            ch++;
        } else if (*ch == '+') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: +\n");
            float op2 = pop();
            float op1 = pop();
            push(op1 + op2);
            fprintf(ICG_fp, "Pushing %.6g + %.6g onto stack\n", op1, op2);
        } else if (*ch == '-') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: -\n");
            float op2 = pop();
            float op1 = pop();
            push(op1 - op2);
            fprintf(ICG_fp, "Pushing %.6g - %.6g onto stack\n", op1, op2);
        } else if (*ch == '*') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: *\n");
            float op2 = pop();
            float op1 = pop();
            push(op1 * op2);
            fprintf(ICG_fp, "Pushing %.6g * %.6g onto stack\n", op1, op2);
        } else if (*ch == '/') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: /\n");
            float op2 = pop();
            float op1 = pop();
            if (op2 != 0) {
                push(op1 / op2);
                fprintf(ICG_fp, "Pushing %.6g / %.6g onto stack\n", op1, op2);
            } else {
                fprintf(ICG_fp, "Division by zero\n");
            }
        } else if (*ch == '^') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: ^\n");
            float op2 = pop();
            float op1 = pop();
            push(pow(op1, op2));
            fprintf(ICG_fp, "Pushing %.6g ^ %.6g onto stack\n", op1, op2);
        } else if (*ch == '~') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: ~\n");
            float op1 = pop();
            push(-1 * op1);
            fprintf(ICG_fp, "Pushing %.6g onto stack\n", -1 * op1);
        } else if (*ch == '%') {
            printStack();
            fprintf(ICG_fp, "Sees lexeme: %%\n");
            float op2 = pop();
            float op1 = pop();
            if (op2 != 0) {
                push(fmod(op1, op2));  // Use fmod for floating-point modulo
                fprintf(ICG_fp, "Pushing %.6g %% %.6g onto stack\n", op1, op2);
            } else {
                fprintf(ICG_fp, "Modulo by zero\n");
            }
        }
        ch++;
    }
    return pop();
}

void visualizeSyntaxTree(TreeNode* node, FILE* tree_fp, int depth = 0, bool isRight = true, std::string indent = "") {
    if (node == nullptr) return;

    // Create labels for different node types
    std::string label;
    switch (node->label) {
        case 0: label = "id: " + std::to_string(node->tokenCode) + " val: " + (node->value); break;
        case 1: label = "const: " + std::to_string(node->tokenCode)+ " val: " + (node->value); break;
        case 2: 
            if (node->tokenCode == 21) label = "+";
            else if (node->tokenCode == 22) label = "-";
            else if (node->tokenCode == 23) label = "*";
            else if (node->tokenCode == 24) label = "/";
            else if (node->tokenCode == 30) label = "%";
            else if (node->tokenCode ==25) label = "^";
            break;
        default: label = "unknown";
    }


    // Write current node with connection to tree_fp
    if (depth > 0) {
        fprintf(tree_fp, "%s%s%s\n", indent.c_str(), (isRight ? "└── " : "├── "), label.c_str());
        indent += isRight ? "    " : "│   ";
    } else {
        fprintf(tree_fp, "%s\n", label.c_str());
    }

    // Recursively write left and right children
    if (node->left || node->right) {
        visualizeSyntaxTree(node->left, tree_fp, depth + 1, false, indent);
        visualizeSyntaxTree(node->right, tree_fp, depth + 1, true, indent);
    }
}



/* evalInfix - Evaluate an infix expression using recursive parsing */
void evalInfix() {
    getChar();
    do {
        lex();
        TreeNode* syntaxTreeRoot = assignment();
        visualizeSyntaxTree(syntaxTreeRoot, tree_fp);
        postorderTraversal(syntaxTreeRoot, postorder_fp);
    } while (nextToken != EOF_TOKEN);
}



/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            nextToken = LEFT_PAREN;
            break;
        case ')':
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '^':  
            addChar();
            nextToken = EXP_OP;
            break;
        case '~':  
            addChar();
            nextToken = TILDE_OP;
            break;
        case '.':  
            addChar();
            nextToken = FLOAT_LIT;
            fprintf(out_fp, "%d", nextToken);
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        case '%':
            addChar();
            nextToken = MOD_OP;
            break;
        default:
            addChar();
            nextToken = EOF_TOKEN;
            break;
    }
    return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else
        fprintf(out_fp, "Error - lexeme is too long \n");
}

/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    } else
        charClass = EOF;
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank() {
    while (isspace(nextChar))
        getChar();
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
expressions */
int lex() {
    lexLen = 0;
    getNonBlank();

    // Boolean used to track implicit multiplication
    static bool needMultiplication = false;  

    if (needMultiplication) {
        needMultiplication = false;
        // Insert a multiplication token
        nextToken = MULT_OP;  
        strcpy(lexeme, "*");
        return nextToken;
    }

    switch (charClass) {
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = IDENT;


            if (charClass == LETTER || charClass == DIGIT || nextChar == '(') {
                needMultiplication = true;
            }
            break;

        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }

            // Handle floating-point numbers
            if (nextChar == '.') {
                addChar();
                getChar();
                while (charClass == DIGIT) {
                    addChar();
                    getChar();
                }
            }

            nextToken = INT_LIT;  // Or FLOAT_LIT, based on your existing logic

            // Check for implicit multiplication after a number
            if (charClass == LETTER || nextChar == '(') {
                needMultiplication = true;
            }
            break;

        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;

        case EOF:
            nextToken = EOF_TOKEN;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    }

    fprintf(out_fp, "Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
    return nextToken;
}


/*****************************************************/
/* error - a function to handle syntax errors */
void error() {
    fprintf(out_fp, "Syntax error\n");
}

/*****************************************************/

/* assignment - Parse strings in the language generated by <assignment> */
TreeNode* assignment() {

    fprintf(out_fp, "Enter <assign>\n");

    // Left-hand side: Variable
    
    TreeNode* variableNode = makeNode(identifier, IDENT, lexeme);
    lex();

    
    // Right-hand side: Expression
    TreeNode* exprNode = expr();

    TreeNode* assignNode = makeNode(op, ASSIGN_OP, lexeme, variableNode, exprNode);

    fprintf(out_fp, "Exit <expr>\n");
    return assignNode;
}


/* expr - Parse strings in the language generated by <expr> */
TreeNode* expr() {
    fprintf(out_fp, "Enter <expr>\n");
    TreeNode* node = term();  


    while(nextToken == ADD_OP || nextToken == SUB_OP){
        int opToken = nextToken;
        std::string opValue(lexeme);
        lex();
        TreeNode* leftChild = node;
        TreeNode* rightChild = term();
        node = makeNode(op, opToken, opValue, leftChild, rightChild);
    }
    fprintf(out_fp, "Exit <expr>\n");
    return node;
}

/*****************************************************/
/* term - Parse strings in the language generated by <term> */
TreeNode* term() {
    fprintf(out_fp, "Enter <term>\n");
    TreeNode* node = factor();
    while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) {
        int opToken = nextToken;
        std::string opValue(lexeme);
        lex();  
        TreeNode* leftChild = node;
        TreeNode* rightChild = factor();
        node = makeNode(op, opToken, opValue, leftChild, rightChild);
    }
    fprintf(out_fp, "Exit <term>\n");
    return node;
}

/* factor - Parse strings in the language generated by <factor> */
TreeNode* factor() {
    fprintf(out_fp, "Enter <factor>\n");
    TreeNode* node = exp();

    while (nextToken == EXP_OP) {
        int expToken = nextToken;
        std::string opValue(lexeme);
        lex();  
        TreeNode* leftChild = node;
        TreeNode* rightChild = expr();
        node = makeNode(op, expToken, opValue, leftChild, rightChild);
    }
    fprintf(out_fp, "Exit <factor>\n");
    return node;
}

/*****************************************************/
/* exp - Parse strings in the language generated by <exp> */
TreeNode* exp() {
    fprintf(out_fp, "Enter <exp>\n");
    TreeNode* node = nullptr;
    if (nextToken == LEFT_PAREN) {
        lex();  
        node = expr(); 
        if (nextToken == RIGHT_PAREN) {
            lex();  
        }
    } else if (nextToken == IDENT) {
        std::string opValue(lexeme);
        node = makeNode(identifier, nextToken, opValue);
        lex();  
    } else if (nextToken == INT_LIT || nextToken == FLOAT_LIT){
        std::string opValue(lexeme);
        node = makeNode(constant, nextToken, opValue);
        lex();
    }
    fprintf(out_fp, "Exit <exp>\n");
    return node;
}

/*****************************************************/
