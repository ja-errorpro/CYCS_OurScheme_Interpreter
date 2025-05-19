/****************************************************/
/*  CPP Template for School                         */
/*  Author: CompileErr0r(YiJia)                     */
/*  Author ID: 11127137                             */
/*  Compile Environment: Linux 5.15.90.1 WSL x86_64 */
/*  Compiler: g++ 9.4                               */
/****************************************************/

#ifdef CONSTOPT

#pragma GCC optimize("Ofast")
#pragma loop_opt(on)
// #pragma GCC optimize("Ofast,unroll-loops,no-stack-protector,fast-math")
// #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
#pragma comment(linker, "/stack:200000000")

#endif

#ifdef GCC
#include <bits/stdc++.h>
#else
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#endif
// #include<bits/extc++.h>
using namespace std;

template <class T>
long long Mod(T a, T b) {
    return ((a % b) + b) % b;
}
#define endl '\n'
#define cendl putchar('\n');
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

#define fi first
#define se second
#define toupper(s) transform(s.begin(), s.end(), s.begin(), ::toupper)
#define tolower(s) transform(s.begin(), s.end(), s.begin(), ::tolower)
#define stof stod

const int MAX = 1e9 + 7;
const int MOD = 998244353;

enum AtomType {
    // For anti-plagiarism, use magic(random) numbers
    UNKNOWN = 0,
    NIL = 0x3f129,
    DOT = -0xd129a,
    NUMBER = 0x1a2f,
    FLOAT = 6181825,
    STRING = -0b1011111000000,
    SYMBOL = 07126531,
    T = 0xdead,
    QUOTE = 0xCaFe,
    LEFT_PAREN = 0xb33f,
    RIGHT_PAREN = 0x5069,
    INT = 0x4a8763,
    ERROR = 0xa2c991
};

class Atom {
   public:
    int type;
    string symbol;
    bool isFunction;
    int line;
    int column;
    Atom() {
        type = AtomType::UNKNOWN;
        symbol = "";
        isFunction = false;
    }

    Atom(int type, string symbol, int line, int column) {
        this->type = type;
        this->symbol = symbol;
        this->line = line;
        this->column = column;
        isFunction = false;
    }
};

enum ErrorType {
    UNKNOWN_ERROR = 0,
    EOF_ERROR,
    UNEXPECTED_TOKEN_ERROR,
    NO_CLOSING_QUOTE_ERROR,
    UNBOUND_ERROR,
    NON_LIST_ERROR,
    NUMBER_OF_ARGUMENT_ERROR,
    ARGUMENT_TYPE_ERROR,
    APPLY_NON_FUNCTION_ERROR,
    NO_RETURN_VALUE_ERROR,
    NO_RETURN_VALUE_ERROR_IN_FUNCTION,
    UNBOUND_SYMBOL_ERROR,
    DIVISION_BY_ZERO_ERROR,
    DEFINE_FORMAT_ERROR,
    COND_FORMAT_ERROR,
    UNBOUND_PARAMETER_ERROR,
    UNBOUND_TEST_CONDITION_ERROR,
    UNBOUND_CONDITION_ERROR,
    LEVEL_ERROR,
    LAMBDA_FORMAT_ERROR,
    LET_FORMAT_ERROR,
    CUSTOM_ERROR,
    SET_FORMAT_ERROR
};
struct AbstractSyntaxTreeNode {
    Atom atom;
    AbstractSyntaxTreeNode *left, *right;
    bool isFunction;
    AbstractSyntaxTreeNode() {
        atom = Atom(AtomType::DOT, ".", -MOD, -MOD);
        left = right = nullptr;
        isFunction = false;
    }
    AbstractSyntaxTreeNode(Atom atom) {
        this->atom = atom;
        left = right = nullptr;
        isFunction = false;
    }
};

unordered_set<AbstractSyntaxTreeNode *> garbageCollector;

class Errors {
    // exception class
   public:
    ErrorType type;
    string message;
    string unexpectedToken;
    AbstractSyntaxTreeNode *node;
    Errors(ErrorType e, string message, string unexpectedToken = "", AbstractSyntaxTreeNode *node = nullptr) {
        this->type = e;
        this->message = message;
        this->unexpectedToken = unexpectedToken;
        this->node = node;
    }
    string what() { return message; }
};

bool isAtomButNotParen(Atom atom) {
    return atom.type == AtomType::SYMBOL || atom.type == AtomType::INT || atom.type == AtomType::FLOAT ||
           atom.type == AtomType::STRING || atom.type == AtomType::NIL || atom.type == AtomType::T;
}

class LexicalAnalyzer {
   public:
    vector<Atom> tokenList;
    int lineNumber;
    int columnNumber;
    string peekTarget;
    LexicalAnalyzer() {
        lineNumber = 1;
        columnNumber = 0;
    }

    void reset() {
        lineNumber = 1;
        columnNumber = 0;
        tokenList.clear();
        cin.clear();
        peekTarget = "";
        // before next s-exp chars, the same line is not line 1.
        skipLineComment(skipWhiteSpaces());
    }

    bool isspace(char c) { return std::isspace(c) || c == '\0'; }

    void errorreset(int column = 0) {
        lineNumber = 1;
        columnNumber = column;
        tokenList.clear();
        peekTarget = "";
        // skipWhiteSpaces();
        // skipLineComment();
    }

    bool isSeparator(char c) {
        return isspace(c) || c == '(' || c == ')' || c == '\'' || c == '"' || c == ';';
    }

    bool isReturn(char c) { return (c == '\n' || c == '\r'); }

    char getNextChar() {
        char c = getchar();
        if (c == EOF) throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
        recordLocation(c);
        return c;
    }

    void recordLocation(char c) {
        if (isReturn(c)) {
            columnNumber = 0;
            lineNumber++;
        } else {
            columnNumber++;
        }
    }

    char getNextNonWhiteSpaceChar() { // read a non-white space char
        char c = getNextChar();
        while (isspace(c)) {
            c = getNextChar();
        }
        return c;
    }

    string getNextString() {
        // keep reading until get next " or return line
        // Note: must handle escape character(\\, \"...)
        string token = "";
        int old_line = lineNumber;
        int old_column = columnNumber;

        char c;
        try {
            c = getNextChar();
        } catch (Errors e) {
            if (e.type == ErrorType::EOF_ERROR) {
                eof = true;
                throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                             "ERROR (no closing quote) : END-OF-LINE encountered at Line " +
                                 to_string(old_line) + " Column " + to_string(old_column + 1),
                             "\n");
            }
        }
        if (isReturn(c)) {
            throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                         "ERROR (no closing quote) : END-OF-LINE encountered at Line " + to_string(old_line) +
                             " Column " + to_string(old_column + 1),
                         "\n");
        }
        int escape_count = 0;
        while (c != '"' && !isReturn(c)) {
            if (c == '\\') {
                c = cin.peek();
                if (c == EOF) {
                    eof = true;
                    throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                                 "ERROR (no closing quote) : END-OF-LINE encountered at Line " +
                                     to_string(old_line) + " Column " + to_string(columnNumber),
                                 "\n");
                    // throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
                }
                bool isEscape = true;
                if (c == 'n') {
                    token += '\n';
                    escape_count++;
                } else if (c == 't') {
                    token += '\t';
                    escape_count++;
                } else if (c == 'r') {
                    token += '\r';
                    escape_count++;
                } else if (c == '0') {
                    token += '\0';
                    escape_count++;
                } else if (c == '\\') {
                    token += '\\';
                    escape_count++;
                } else if (c == '"') {
                    token += '"';
                    escape_count++;
                } else if (isReturn(c)) {
                    columnNumber++;
                    throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                                 "ERROR (no closing quote) : END-OF-LINE encountered at Line " +
                                     to_string(old_line) + " Column " + to_string(columnNumber),
                                 "\n");
                } else {
                    token += '\\';
                    isEscape = false;
                }
                if (isEscape) getNextChar();

            } else {
                token += c;
            }
            try {
                c = getNextChar();
            } catch (Errors e) {
                if (e.type == ErrorType::EOF_ERROR) {
                    eof = true;
                    throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                                 "ERROR (no closing quote) : END-OF-LINE encountered at Line " +
                                     to_string(old_line) + " Column " + to_string(columnNumber + 1),
                                 "\n");
                }
            }
        }
        // c = cin.peek();
        if (isReturn(c)) {
            // columnNumber++;
            old_column += token.size() + 1 + escape_count;
            throw Errors(ErrorType::NO_CLOSING_QUOTE_ERROR,
                         "ERROR (no closing quote) : END-OF-LINE encountered at Line " + to_string(old_line) +
                             " Column " + to_string(old_column),
                         "\n");
        }
        token += c;

        // peekNextNonWhiteSpaceChar();
        // token += c;
        return token;
    }

    bool isNumber(string s, bool &isFloat) {
        bool result = true;
        isFloat = false;
        // check if there are + or - at the prefix
        // -. is NOT FLOAT!
        if (s.empty()) return false;
        if (s[0] == '+' || s[0] == '-') {
            s = s.substr(1);
        }
        if (s.size() == 0 || (s[0] == '.' && (int)s.size() < 2)) return false;
        for (int i = 0; i < (int)s.size(); i++) {
            if (!isdigit(s[i])) {
                if (s[i] == '.' && !isFloat) {
                    isFloat = true;
                } else {
                    result = false;
                    break;
                }
            }
        }
        return result;
    }

    AtomType getAtomType(string s) {
        if (s == "()" || s == "#f" || s == "nil") return AtomType::NIL;
        if (s == ".") return AtomType::DOT;
        if (s == "#t" || s == "t") return AtomType::T;
        if (s == "'") return AtomType::QUOTE; // Do we need to handle "quote" ?????
        if (s == "(") return AtomType::LEFT_PAREN;
        if (s == ")") return AtomType::RIGHT_PAREN;
        if (!s.empty() && s[0] == '"') return AtomType::STRING;

        bool isFloat;
        if (isNumber(s, isFloat)) {
            if (isFloat) return AtomType::FLOAT;
            return AtomType::INT;
        }

        return AtomType::SYMBOL;
    }

    Atom StringToAtom(string s) {
        Atom atom = {AtomType::UNKNOWN, "", lineNumber,
                     0}; // column number will be updated after format the token
        atom.type = getAtomType(s);
        if (atom.type == AtomType::NIL) s = "nil";
        if (atom.type == AtomType::INT || atom.type == AtomType::FLOAT) {
            if (s[0] == '+') {
                s = s.substr(1);
            }

            if (atom.type == AtomType::FLOAT) {
                if (s[0] == '.') { // .123, +.123,
                    s = "0" + s;
                } else if (s.back() == '.') { // 123., +123., -123.
                    s += "000";
                } else if ((int)s.size() > 1 && s[0] == '-' && s[1] == '.') {
                    s = "-0" + s.substr(1);
                }
            }
        }
        if (atom.type == AtomType::T) s = "#t";

        atom.symbol = s;
        atom.column = columnNumber - s.size() + 1;
        return atom;
    }

    /*
     Only takes a peek at the next token,
     but not get it.
    */
    bool eof = false;
    string peekToken() {
        if (peekTarget != "") return peekTarget;
        if (eof) throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
        string token = "";

        try {
            char c = getNextNonWhiteSpaceChar();

            // skip line comment
            if (c == ';') {
                c = cin.peek();
                if (c == EOF)
                    throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
                while (!isReturn(c)) {
                    c = getNextChar();
                    c = cin.peek();
                    if (c == EOF)
                        throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
                }

                return peekToken();
            }

            token += c;

            if (c == '"') {
                // Case 1: encounter a string,
                token += getNextString();
            } else if (!isSeparator(c)) {
                // Case 2: encounter a symbol,
                // keep reading until get next separator or white space
                c = cin.peek();
                if (c == EOF)
                    throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
                while (!isSeparator(c) && !isspace(c)) {
                    token += getNextChar();
                    c = cin.peek();
                    if (c == EOF)
                        throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
                }
            }
        } catch (Errors e) {
            if (e.type == ErrorType::EOF_ERROR)
                eof = true;
            else
                throw e;
        }

        peekTarget = token;

        return token;
    }

    Atom getToken() {
        if (peekTarget == "") peekToken();
        Atom atom = StringToAtom(peekTarget);
        if (!tokenList.empty() && tokenList.back().type == AtomType::LEFT_PAREN &&
            atom.type == AtomType::RIGHT_PAREN) {
            Atom theNIL = tokenList.back();
            theNIL.type = AtomType::NIL;
            theNIL.symbol = "nil";
            tokenList[tokenList.size() - 1] = theNIL;
        } else
            tokenList.push_back(atom);
        peekTarget = "";
        return atom;
    }

    void skipToNextLine(char c = '\0') { // only called when error occurs
        c = cin.peek();
        if (c == EOF) throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
        while (!isReturn(c)) {
            c = getchar();
            // recordLocation(c);
            c = cin.peek();
            if (c == EOF)
                throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
        }
        getchar();
    }

    // ret: is there a new line?(after skip there is no another s-exp)
    bool skipWhiteSpaces() { // legal s-exp, skip to return or next legal s-exp char
        char c = cin.peek();
        if (c == EOF)
            throw Errors(ErrorType::EOF_ERROR, "\n> ERROR (no more input) : END-OF-FILE encountered");
        while (isspace(c) && !isReturn(c)) {
            c = getchar();
            columnNumber++;
            c = cin.peek();
            if (c == EOF)
                throw Errors(ErrorType::EOF_ERROR, "\n> ERROR (no more input) : END-OF-FILE encountered");
        }

        if (isReturn(c)) {
            getchar();
            lineNumber = 1;
            columnNumber = 0;
            return true;
        }
        return false;
    }

    void skipLineComment(bool newLine = true) {
        char c = cin.peek();
        if (c == EOF)
            throw Errors(ErrorType::EOF_ERROR, "\n> ERROR (no more input) : END-OF-FILE encountered");
        if (c == ';') {
            while (!isReturn(c)) {
                c = getchar();
                c = cin.peek();
                if (c == EOF)
                    throw Errors(ErrorType::EOF_ERROR, "\n> ERROR (no more input) : END-OF-FILE encountered");
            }

            // getchar();
        }

        if (isReturn(c) && !newLine) {
            c = getchar();
            lineNumber = 1;
            columnNumber = 0;
        }
    }
} scanner;

class SyntaxAnalyzer {
    /*
    Syntax of OurScheme :

<S-exp> ::= <ATOM>
            | LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
            | QUOTE <S-exp>

<ATOM>  ::= SYMBOL | INT | FLOAT | STRING
            | NIL | T | LEFT-PAREN RIGHT-PAREN

            {} : 0 or more
            [] : 0 or 1
    */
   public:
    AbstractSyntaxTreeNode *root;

    void buildAbstractSyntaxTree(AbstractSyntaxTreeNode *&current, vector<Atom> tokenList,
                                 int &tokenListindex) {
        if (tokenListindex >= (int)tokenList.size()) return;
        if (current == nullptr) {
            current = new AbstractSyntaxTreeNode();
            garbageCollector.insert(current);
        }
        if (tokenList[tokenListindex].type == AtomType::LEFT_PAREN) {
            // LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
            // 1. put next S-exp into left child
            // 2. if there are more S-exp, recursively build in right child
            // 3. if there is a DOT, put the next S-exp into right child
            // 4. else, put nil-node into right child

            tokenListindex++;
            buildAbstractSyntaxTree(current->left, tokenList, tokenListindex);
            // tokenListindex++;
            AbstractSyntaxTreeNode *temp = current;
            while (tokenListindex < (int)tokenList.size() &&
                   tokenList[tokenListindex].type != AtomType::RIGHT_PAREN &&
                   tokenList[tokenListindex].type != AtomType::DOT) {
                temp->right = new AbstractSyntaxTreeNode();
                garbageCollector.insert(temp->right);
                buildAbstractSyntaxTree(temp->right->left, tokenList, tokenListindex);
                temp = temp->right;
            }

            if (tokenListindex < (int)tokenList.size() && tokenList[tokenListindex].type == AtomType::DOT) {
                current->atom = Atom(AtomType::DOT, ".", tokenList[tokenListindex].line,
                                     tokenList[tokenListindex].column);
                tokenListindex++;
                buildAbstractSyntaxTree(temp->right, tokenList, tokenListindex);
                tokenListindex++;

            } else {
                temp->right = new AbstractSyntaxTreeNode(Atom(AtomType::NIL, "nil", -MOD, -MOD));
                garbageCollector.insert(temp->right);
                if (tokenListindex < (int)tokenList.size() - 1) tokenListindex++;
            }

        } else if (tokenList[tokenListindex].type == AtomType::QUOTE) {
            // QUOTE <S-exp>
            // 1. put QUOTE to left child
            // 2. move to right child, build next S-exp in "current" left child
            // 3. finally, put nil-node into "current" right child

            current->left = new AbstractSyntaxTreeNode(tokenList[tokenListindex]);
            garbageCollector.insert(current->left);
            tokenListindex++;
            current->right = new AbstractSyntaxTreeNode();
            garbageCollector.insert(current->right);
            buildAbstractSyntaxTree(current->right->left, tokenList, tokenListindex);
            current->right->right = new AbstractSyntaxTreeNode(Atom(AtomType::NIL, "nil", -MOD, -MOD));
            garbageCollector.insert(current->right->right);
        } else {
            // <ATOM>
            // just put the atom into current node
            current->atom = tokenList[tokenListindex];
            tokenListindex++;
        }
    }

    SyntaxAnalyzer() { root = nullptr; }

    void clearAbstractSyntaxTree(AbstractSyntaxTreeNode *current) {
        if (current == nullptr) return;
        clearAbstractSyntaxTree(current->left);
        clearAbstractSyntaxTree(current->right);
        delete current;
        current = nullptr;
    }

    void clearAbstractSyntaxTree() {
        // clearAbstractSyntaxTree(root);
        root = nullptr;
    }
    void buildAbstractSyntaxTree(vector<Atom> tokenList = scanner.tokenList) {
        int tokenListindex = 0;
        buildAbstractSyntaxTree(root, tokenList, tokenListindex);
    }

    void printAll(AbstractSyntaxTreeNode *current) {
        if (current == nullptr) return;
        printAll(current->left);
        printAll(current->right);
    }

    void printAll() { printAll(root); }

    void printSpaces(int n) {
        for (int i = 0; i < n; i++) cout << " ";
    }
    void printAtom(Atom atom) {
        if (atom.type == AtomType::NIL)
            cout << "nil";
        else if (atom.type == AtomType::DOT)
            cout << "x";
        else if (atom.type == AtomType::INT)
            cout << atom.symbol;
        else if (atom.type == AtomType::FLOAT)
            printf("%.3f", stof(atom.symbol.c_str()));
        else if (atom.type == AtomType::STRING)
            cout << atom.symbol;
        else if (atom.type == AtomType::T)
            cout << "#t";
        else if (atom.type == AtomType::QUOTE)
            cout << "quote";
        else if (atom.isFunction)
            cout << "#<procedure " << atom.symbol << ">";
        else
            cout << atom.symbol;
    }

    void printRightLeftChildren(AbstractSyntaxTreeNode *current, int alignSpaces) {
        if (current == nullptr) return;
        AbstractSyntaxTreeNode *right = current->right;
        while (right != nullptr) {
            if (right->atom.type == AtomType::DOT) {
                AbstractSyntaxTreeNode *left = right->left;
                if (left->atom.type == AtomType::UNKNOWN ||
                    left->atom.type == AtomType::DOT) { // todo: test this case if it is correct
                    cendl;
                    printSpaces(alignSpaces + 2);
                    printAbstractSyntaxTree(left, alignSpaces + 2);
                } else {
                    cendl;
                    printSpaces(alignSpaces + 2);
                    printAtom(left->atom);
                    // cendl;
                }
            } else {
                if (right->atom.type != AtomType::NIL) { // atom
                    cendl;
                    printSpaces(alignSpaces + 2);
                    cout << ".";

                    cendl;
                }
                printAbstractSyntaxTree(right, alignSpaces + 2);
            }
            right = right->right;
        }
    }

    void printAbstractSyntaxTreeAtom(AbstractSyntaxTreeNode *current = nullptr, int alignSpaces = 0,
                                     bool addEndline = 1) {
        if (current == nullptr) current = root;
        if (current == nullptr) return;
        if (isAtomButNotParen(current->atom)) {
            printAtom(current->atom);
        } else {
            printAbstractSyntaxTree(current, alignSpaces, addEndline);
        }
    }

    void printAbstractSyntaxTree(AbstractSyntaxTreeNode *current = nullptr, int alignSpaces = 0,
                                 bool addEndline = 1) {
        // like pretty print in python
        if (current == nullptr) current = root;
        if (current == nullptr) return;
        if (current->atom.type == AtomType::UNKNOWN || current->atom.type == AtomType::DOT) {
            cout << "( ";
            AbstractSyntaxTreeNode **left = &current->left;
            AbstractSyntaxTreeNode **right = &current->right;
            if ((*left) != nullptr) {
                if ((*left)->atom.type == AtomType::QUOTE) { // left child is quote
                    printAtom((*left)->atom);                // quote
                } else if ((*left)->atom.type == AtomType::UNKNOWN ||
                           (*left)->atom.type == AtomType::DOT) { // left child is .
                    printAbstractSyntaxTree((*left), alignSpaces + 2);
                } else {
                    printAtom((*left)->atom);
                }
            }
            printRightLeftChildren(current, alignSpaces);
            cendl;
            printSpaces(alignSpaces);
            cout << ")";
        } else {
            if (current->atom.type != AtomType::NIL) {
                printSpaces(alignSpaces);
                printAtom(current->atom);
            }
        }
    }

    bool isSExpCorrect(Atom startAtom, LexicalAnalyzer &scannerExp = scanner) {
        if (isAtomButNotParen(startAtom)) return true; // Note: () is a valid atom
        if (startAtom.type == AtomType::LEFT_PAREN) {
            string nextToken = scannerExp.peekToken();
            int nextAtomType = scannerExp.StringToAtom(nextToken).type;
            if (nextAtomType == AtomType::RIGHT_PAREN) {
                scannerExp.getToken();
                return true;
            }

            Atom nextAtom = scannerExp.getToken(); // Is this another S-exp or DOT or RIGHT_PAREN?
            if (nextAtomType == AtomType::DOT) {
                throw Errors(ErrorType::UNEXPECTED_TOKEN_ERROR,
                             "ERROR (unexpected token) : atom or '(' expected when token at Line " +
                                 to_string(nextAtom.line) + " Column " + to_string(nextAtom.column) +
                                 " is >>" + nextAtom.symbol + "<<",
                             nextAtom.symbol);
            }

            while (nextAtom.type != AtomType::RIGHT_PAREN && nextAtom.type != AtomType::DOT) {
                isSExpCorrect(nextAtom, scannerExp);
                nextAtom = scannerExp.getToken();
            }
            if (nextAtom.type == AtomType::DOT) {
                nextAtom = scannerExp.getToken();
                isSExpCorrect(nextAtom, scannerExp);
                nextAtom = scannerExp.getToken();
                if (nextAtom.type != AtomType::RIGHT_PAREN) {
                    throw Errors(ErrorType::UNEXPECTED_TOKEN_ERROR,
                                 "ERROR (unexpected token) : ')' expected when token at Line " +
                                     to_string(nextAtom.line) + " Column " + to_string(nextAtom.column) +
                                     " is >>" + nextAtom.symbol + "<<",
                                 nextAtom.symbol);
                }
                return true;
            } else if (nextAtom.type == AtomType::RIGHT_PAREN) {
                return true;
            } else {
                throw Errors(ErrorType::UNEXPECTED_TOKEN_ERROR,
                             "ERROR (unexpected token) : ')' expected when token at Line " +
                                 to_string(nextAtom.line) + " Column " + to_string(nextAtom.column) +
                                 " is >>" + nextAtom.symbol + "<<",
                             nextAtom.symbol);
            }
        }
        if (startAtom.type == AtomType::QUOTE) {
            Atom nextAtom = scannerExp.getToken();
            isSExpCorrect(nextAtom, scannerExp);
            return true;
        }
        throw Errors(ErrorType::UNEXPECTED_TOKEN_ERROR,
                     "ERROR (unexpected token) : atom or '(' expected when token at Line " +
                         to_string(startAtom.line) + " Column " + to_string(startAtom.column) + " is >>" +
                         startAtom.symbol + "<<",
                     startAtom.symbol);
        return false;
    }

    bool isExit() {
        if (scanner.tokenList.size() < 3) return false;
        if (root != nullptr && root->left != nullptr && root->right != nullptr &&
            root->left->atom.type == AtomType::SYMBOL && root->right->atom.type == AtomType::NIL &&
            root->left->atom.symbol == "exit") {
            return true;
        }
        return false;
    }

} parser;

class Executor {
   public:
    unordered_map<string, AbstractSyntaxTreeNode *> globalSymbolTable;
    bool verbose = true;
    unordered_map<string, bool> reservedWords = {{"cons", true},
                                                 {"car", true},
                                                 {"cdr", true},
                                                 {"exit", true},
                                                 {"list", true},
                                                 {"quote", true},
                                                 {"'", true},
                                                 {"define", true},
                                                 {"atom?", true},
                                                 {"pair?", true},
                                                 {"list?", true},
                                                 {"null?", true},
                                                 {"integer?", true},
                                                 {"real?", true},
                                                 {"number?", true},
                                                 {"string?", true},
                                                 {"boolean?", true},
                                                 {"symbol?", true},
                                                 {"+", true},
                                                 {"-", true},
                                                 {"*", true},
                                                 {"/", true},
                                                 {"not", true},
                                                 {"or", true},
                                                 {"and", true},
                                                 {">", true},
                                                 {">=", true},
                                                 {"<", true},
                                                 {"<=", true},
                                                 {"=", true},
                                                 {"string-append", true},
                                                 {"string>?", true},
                                                 {"string<?", true},
                                                 {"string=?", true},
                                                 {"eqv?", true},
                                                 {"equal?", true},
                                                 {"begin", true},
                                                 {"if", true},
                                                 {"cond", true},
                                                 {"clean-environment", true},
                                                 {"let", true},
                                                 {"lambda", true},
                                                 {"verbose?", true},
                                                 {"verbose", true},
                                                 {"read", true},
                                                 {"write", true},
                                                 {"display-string", true},
                                                 {"newline", true},
                                                 {"symbol->string", true},
                                                 {"number->string", true},
                                                 {"create-error-object", true},
                                                 {"error-object?", true},
                                                 {"set!", true},
                                                 {"eval", true}

    }; // user is not allowed to define these words
    // bool: is it a function name?

    void init() {
        for (auto &it : reservedWords) {
            if (globalSymbolTable.find(it.first) == globalSymbolTable.end())
                globalSymbolTable[it.first] =
                    new AbstractSyntaxTreeNode(Atom(AtomType::SYMBOL, it.first, -MOD, -MOD));
            garbageCollector.insert(globalSymbolTable[it.first]);
            globalSymbolTable[it.first]->isFunction = it.second;
            globalSymbolTable[it.first]->atom.isFunction = it.second;
        }
    }

    Executor() { init(); }

    bool isList(AbstractSyntaxTreeNode *current) {
        AbstractSyntaxTreeNode **right = &current;
        while ((*right) != nullptr && (*right)->atom.type != AtomType::NIL) right = &((*right)->right);
        return (*right) != nullptr && (*right)->atom.type == AtomType::NIL;
    }

    void clear() {
        // find unique nodes in symbol table

        for (auto &it : garbageCollector) {
            delete it;
        }

        globalSymbolTable.clear();
    }

    /*
        This function can use to check argument count
        @param current: the head of the list
        @return: the size of the list
    */
    int size(AbstractSyntaxTreeNode *current) {
        int cnt = 0;
        while (current != nullptr && current->atom.type != AtomType::NIL) {
            cnt++;
            current = current->right;
        }
        return cnt;
    }

    AbstractSyntaxTreeNode *evaluateSymbols(
        AbstractSyntaxTreeNode *current, unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        if (current->atom.type != AtomType::SYMBOL) return current;

        if (localSymbolTable.find(current->atom.symbol) != localSymbolTable.end()) {
            return copyNode(localSymbolTable[current->atom.symbol]);
        }

        if (globalSymbolTable.find(current->atom.symbol) == globalSymbolTable.end()) {
            throw Errors(ErrorType::UNBOUND_ERROR, "ERROR (unbound symbol) : " + current->atom.symbol);
        }
        return copyNode(globalSymbolTable[current->atom.symbol]);
    }

    AbstractSyntaxTreeNode *evaluateTree(AbstractSyntaxTreeNode *current,
                                         unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        current->left = evaluateSymbols(current->left, localSymbolTable);
        current->right = evaluateSymbols(current->right, localSymbolTable);
        evaluateTree(current->left, localSymbolTable);
        evaluateTree(current->right, localSymbolTable);
        return current;
    }

    /*
        Copy a tree
        @param current: the root of the tree
        @return: root of the new tree
    */
    AbstractSyntaxTreeNode *copyNode(AbstractSyntaxTreeNode *current) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *newNode = new AbstractSyntaxTreeNode();
        newNode->atom = current->atom;
        newNode->left = copyNode(current->left);
        newNode->right = copyNode(current->right);
        newNode->isFunction = current->isFunction;
        garbageCollector.insert(newNode);
        return newNode;
    }

    /*
        Note: the atom node is not seen as a function by default
        @param symbol: the atom symbol
        @return: a new atom node
    */
    AbstractSyntaxTreeNode *makeAtom(string symbol) {
        Atom atom = scanner.StringToAtom(symbol);
        atom.column = -MOD;
        atom.line = -MOD;
        AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(atom);
        garbageCollector.insert(result);
        return result;
    }

    /*
        Top level functions: exit, clean-environment, define
        @param current: the root of the tree
        @param level: the level of the tree
    */
    AbstractSyntaxTreeNode *execTopLevelFunctions(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        // firstArgument is the name of the function(define, exit, clean-environment, etc.)
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (level != 0 &&
            (firstArgument->atom.symbol == "exit" || firstArgument->atom.symbol == "clean-environment" ||
             firstArgument->atom.symbol == "define")) {
            if (firstArgument->atom.symbol == "exit")
                throw Errors(ErrorType::LEVEL_ERROR, "ERROR (level of EXIT)");
            if (firstArgument->atom.symbol == "clean-environment")
                throw Errors(ErrorType::LEVEL_ERROR, "ERROR (level of CLEAN-ENVIRONMENT)");
            if (firstArgument->atom.symbol == "define")
                throw Errors(ErrorType::LEVEL_ERROR, "ERROR (level of DEFINE)");
        }
        if (firstArgument->atom.symbol == "exit") {
            if (size(current->right) != 0)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : exit");
            exit(0); // TODO
        } else if (firstArgument->atom.symbol == "clean-environment") {
            if (size(current->right) != 0)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : clean-environment");
            globalSymbolTable.clear();
            return makeAtom(verbose ? "environment cleaned" : "");
        } else if (firstArgument->atom.symbol == "define") {
            if (size(current->right) < 2) // after define, there should be at least 2 arguments
                throw Errors(ErrorType::DEFINE_FORMAT_ERROR, "ERROR (DEFINE format) : ", "", current);

            AbstractSyntaxTreeNode *secondArgument = current->right->left;

            // check if defining a function (define (f x) (g x x) ...)
            if (secondArgument->atom.type == AtomType::DOT) {
                string function_name = secondArgument->left->atom.symbol;
                if (reservedWords.find(function_name) != reservedWords.end())
                    throw Errors(ErrorType::DEFINE_FORMAT_ERROR, "ERROR (DEFINE format) : ", "", current);
                AbstractSyntaxTreeNode *thirdArgument = current->right->right; // function sequence
                AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode();
                garbageCollector.insert(result);
                result->atom = Atom(AtomType::SYMBOL, function_name, -MOD, -MOD);
                result->atom.isFunction = true;
                result->isFunction = true;
                result->atom.column = -MOD;
                result->atom.line = -MOD;
                result->left = secondArgument->right; // parameters
                result->right = thirdArgument;
                globalSymbolTable[function_name] = result;
                return nullptr;
            }

            if (size(current->right) > 2)
                throw Errors(ErrorType::DEFINE_FORMAT_ERROR, "ERROR (DEFINE format) : ", "", current);

            // define a reserved word is not allowed
            if (reservedWords.find(secondArgument->atom.symbol) != reservedWords.end())
                throw Errors(ErrorType::DEFINE_FORMAT_ERROR, "ERROR (DEFINE format) : ", "", current);
            // defining a non-symbol is not allowed
            if (secondArgument->atom.type != AtomType::SYMBOL)
                throw Errors(ErrorType::DEFINE_FORMAT_ERROR, "ERROR (DEFINE format) : ", "", current);

            AbstractSyntaxTreeNode *thirdArgument = current->right->right->left;
            AbstractSyntaxTreeNode *result;

            if (isAtomButNotParen(thirdArgument->atom) && thirdArgument->atom.type != AtomType::SYMBOL) {
                result = evaluateTree(copyNode(thirdArgument), localSymbolTable);
            } else if (thirdArgument->atom.type == AtomType::DOT &&
                       thirdArgument->left->atom.symbol == "lambda") {
                result = exec(thirdArgument, level + 1, localSymbolTable);
                result->atom.isFunction = true;
            } else {
                try {
                    result = exec(thirdArgument, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::NO_RETURN_VALUE_ERROR_IN_FUNCTION,
                                     "ERROR (no return value) : ", "", e.node);
                    }
                    throw e;
                }
            }

            // if (define a b) && b is a known symbol, then adr(a) = adr(b)
            // else copy the tree

            globalSymbolTable[secondArgument->atom.symbol] = result;
            // cout << (verbose ? (secondArgument->atom.symbol + " defined\n") : "");
            return nullptr;
        }
        return nullptr;
    }

    /*
        Constructors: cons, list
        @param current: the root of the tree
        @param level: the level of the tree
    */
    AbstractSyntaxTreeNode *execConstructors(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;

        if (firstArgument->atom.symbol == "cons") {
            if (size(current->right) != 2)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : cons");
            AbstractSyntaxTreeNode *secondArgument = current->right->left;
            AbstractSyntaxTreeNode *thirdArgument = current->right->right->left;
            Atom atom = Atom(AtomType::DOT, ".", -MOD, -MOD);
            AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(atom);
            garbageCollector.insert(result);
            try {
                result->left = exec(secondArgument, level + 1, localSymbolTable);
                result->right = exec(thirdArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            return result;
        } else if (firstArgument->atom.symbol == "list") {
            if (size(current->right) == 0) return makeAtom("nil");
            Atom atom = Atom(AtomType::DOT, ".", -MOD, -MOD);
            AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(atom);
            garbageCollector.insert(result);
            AbstractSyntaxTreeNode **right = &result;
            AbstractSyntaxTreeNode *temp = current->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                try {
                    (*right)->left = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                (*right)->right = new AbstractSyntaxTreeNode(Atom(AtomType::DOT, ".", -MOD, -MOD));
                garbageCollector.insert((*right)->right);
                right = &((*right)->right);
                temp = temp->right;
            }
            (*right) = new AbstractSyntaxTreeNode(Atom(AtomType::NIL, "nil", -MOD, -MOD));
            garbageCollector.insert((*right));
            return result;
        }
        return nullptr;
    }

    AbstractSyntaxTreeNode *execPartAccessors(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol == "car") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : car");
            AbstractSyntaxTreeNode *secondArgument = current->right->left;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(secondArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::DOT && result->atom.type != AtomType::UNKNOWN)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (car with incorrect argument type) : ", "", result);
            return result->left;
        } else if (firstArgument->atom.symbol == "cdr") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : cdr");
            AbstractSyntaxTreeNode *secondArgument = current->right->left;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(secondArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (isAtomButNotParen(result->atom))
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (cdr with incorrect argument type) : ", "", result);
            return result->right;
        }
        return nullptr;
    }

    /*
        atom?
        pair?
        list?
        null?
        integer?
        real?
        number? // in OurSchem, real? = number?, but not in Scheme (there are complex-numbers)
        string?
        boolean?
        symbol?
    */

    bool checkPrimitivePredicatesArgumentCount(AbstractSyntaxTreeNode *current) {
        if (current == nullptr) return false;
        if (current->left->atom.symbol != "atom?" && current->left->atom.symbol != "pair?" &&
            current->left->atom.symbol != "list?" && current->left->atom.symbol != "null?" &&
            current->left->atom.symbol != "integer?" && current->left->atom.symbol != "real?" &&
            current->left->atom.symbol != "number?" && current->left->atom.symbol != "string?" &&
            current->left->atom.symbol != "boolean?" && current->left->atom.symbol != "symbol?"

        )
            return false;
        if (size(current->right) != 1)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + current->left->atom.symbol);
        return true;
    }
    AbstractSyntaxTreeNode *execPrimitivePredicates(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        if (!checkPrimitivePredicatesArgumentCount(current)) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        AbstractSyntaxTreeNode *secondArgument = current->right->left;
        AbstractSyntaxTreeNode *result;
        try {
            result = exec(secondArgument, level + 1, localSymbolTable);
        } catch (Errors e) {
            if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "", e.node);
            }
            throw e;
        }
        if (firstArgument->atom.symbol == "atom?")
            return makeAtom(isAtomButNotParen(result->atom) ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "pair?")
            return makeAtom(result->atom.type == AtomType::DOT ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "list?")
            return makeAtom(isList(result) ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "null?")
            return makeAtom(result->atom.type == AtomType::NIL ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "integer?")
            return makeAtom(result->atom.type == AtomType::INT ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "real?" || firstArgument->atom.symbol == "number?")
            return makeAtom(
                result->atom.type == AtomType::FLOAT || result->atom.type == AtomType::INT ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "string?")
            return makeAtom(result->atom.type == AtomType::STRING ? "#t" : "nil");
        else if (firstArgument->atom.symbol == "boolean?")
            return makeAtom(result->atom.type == AtomType::T || result->atom.type == AtomType::NIL ? "#t"
                                                                                                   : "nil");
        else if (firstArgument->atom.symbol == "symbol?")
            return makeAtom(result->atom.type == AtomType::SYMBOL ? "#t" : "nil");
        return nullptr;
    }

    /*
        +, -, *, /
    */
    AbstractSyntaxTreeNode *execBasicArithmetic(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "+" && firstArgument->atom.symbol != "-" &&
            firstArgument->atom.symbol != "*" && firstArgument->atom.symbol != "/")
            return nullptr;
        if (size(current->right) < 2)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);

        // may have 2 or more arguments
        bool isFloat = false;
        if (firstArgument->atom.symbol == "+") {
            double doubleSum = 0;
            int intSum = 0;
            AbstractSyntaxTreeNode *temp = current->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *result;
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (result->atom.type == AtomType::FLOAT) {
                    isFloat = true;
                    doubleSum += stof(result->atom.symbol);
                } else if (result->atom.type == AtomType::INT) {
                    intSum += stoi(result->atom.symbol);
                } else {
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (+ with incorrect argument type) : ", "", result);
                }
                temp = temp->right;
            }

            if (isFloat) {
                // char buffer[100];
                // sprintf(buffer, "%.3f", doubleSum + intSum);
                AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(
                    Atom(AtomType::FLOAT, to_string(doubleSum + intSum), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            } else {
                AbstractSyntaxTreeNode *result =
                    new AbstractSyntaxTreeNode(Atom(AtomType::INT, to_string(intSum), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            }
        } else if (firstArgument->atom.symbol == "-") {
            AbstractSyntaxTreeNode *temp = current->right;
            if (temp == nullptr || temp->atom.type == AtomType::NIL) {
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
            }
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type == AtomType::FLOAT) {
                isFloat = true;
            } else if (result->atom.type != AtomType::INT) {
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (- with incorrect argument type) : ", "",
                             result);
            }
            double doubleSum = stof(result->atom.symbol);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *result;
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (result->atom.type == AtomType::FLOAT) {
                    isFloat = true;
                    doubleSum -= stof(result->atom.symbol);
                } else if (result->atom.type == AtomType::INT) {
                    doubleSum -= stoi(result->atom.symbol);
                } else {
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (- with incorrect argument type) : ", "", result);
                }
                temp = temp->right;
            }

            if (isFloat) {
                // char buffer[100];
                // sprintf(buffer, "%.3f", doubleSum);
                AbstractSyntaxTreeNode *result =
                    new AbstractSyntaxTreeNode(Atom(AtomType::FLOAT, to_string(doubleSum), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            } else {
                AbstractSyntaxTreeNode *result =
                    new AbstractSyntaxTreeNode(Atom(AtomType::INT, to_string((int)doubleSum), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            }
        } else if (firstArgument->atom.symbol == "*") {
            double doubleProduct = 1;
            AbstractSyntaxTreeNode *temp = current->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *result;
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (result->atom.type == AtomType::FLOAT) {
                    isFloat = true;
                    doubleProduct *= stof(result->atom.symbol);
                } else if (result->atom.type == AtomType::INT) {
                    doubleProduct *= stoi(result->atom.symbol);
                } else {
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (* with incorrect argument type) : ", "", result);
                }
                temp = temp->right;
            }

            if (isFloat) {
                // char buffer[100];
                // sprintf(buffer, "%.3f", doubleProduct);
                AbstractSyntaxTreeNode *result =
                    new AbstractSyntaxTreeNode(Atom(AtomType::FLOAT, to_string(doubleProduct), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            } else {
                AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(
                    Atom(AtomType::INT, to_string((int)doubleProduct), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            }
        } else if (firstArgument->atom.symbol == "/") {
            AbstractSyntaxTreeNode *temp = current->right;
            if (temp == nullptr || temp->atom.type == AtomType::NIL) {
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
            }
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type == AtomType::FLOAT) {
                isFloat = true;
            } else if (result->atom.type != AtomType::INT) {
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (/ with incorrect argument type) : ", "",
                             result);
            }
            double doubleQuotient = stof(result->atom.symbol);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *result;
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (result->atom.type == AtomType::FLOAT) {
                    isFloat = true;
                    if (fabs(stof(result->atom.symbol)) < 1e-20)
                        throw Errors(ErrorType::DIVISION_BY_ZERO_ERROR, "ERROR (division by zero) : /");
                    doubleQuotient /= stof(result->atom.symbol);
                } else if (result->atom.type == AtomType::INT) {
                    if (stoi(result->atom.symbol) == 0)
                        throw Errors(ErrorType::DIVISION_BY_ZERO_ERROR, "ERROR (division by zero) : /");
                    doubleQuotient /= stoi(result->atom.symbol);
                } else {
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (/ with incorrect argument type) : ", "", result);
                }
                temp = temp->right;
            }

            if (isFloat) {
                // char buffer[100];
                // sprintf(buffer, "%.3f", doubleQuotient);
                AbstractSyntaxTreeNode *result =
                    new AbstractSyntaxTreeNode(Atom(AtomType::FLOAT, to_string(doubleQuotient), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            } else {
                AbstractSyntaxTreeNode *result = new AbstractSyntaxTreeNode(
                    Atom(AtomType::INT, to_string((int)doubleQuotient), -MOD, -MOD));
                garbageCollector.insert(result);
                return result;
            }
        }
        return nullptr;
    }

    /*
        not, or, and
    */
    AbstractSyntaxTreeNode *execLogical(AbstractSyntaxTreeNode *current, int level,
                                        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol == "not") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : not");
            AbstractSyntaxTreeNode *secondArgument = current->right->left;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(secondArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }

            return makeAtom(result->atom.type == AtomType::NIL ? "#t" : "nil");
        } else if (firstArgument->atom.symbol == "or") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_CONDITION_ERROR, "ERROR (unbound condition) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (result->atom.type != AtomType::NIL) return result;
                temp = temp->right;
            }
            return result;
        } else if (firstArgument->atom.symbol == "and") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                try {
                    result = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_CONDITION_ERROR, "ERROR (unbound condition) : ", "",
                                     e.node);
                    }
                    throw e;
                }

                if (result->atom.type == AtomType::NIL) return makeAtom("nil");
                temp = temp->right;
            }
            return result;
        }
        return nullptr;
    }

    /*
        >, >=, <, <=, =
    */
    AbstractSyntaxTreeNode *execComparator(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != ">" && firstArgument->atom.symbol != ">=" &&
            firstArgument->atom.symbol != "<" && firstArgument->atom.symbol != "<=" &&
            firstArgument->atom.symbol != "=")
            return nullptr;
        if (size(current->right) < 2)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);

        AbstractSyntaxTreeNode *t = current->right;
        while (t != nullptr && t->atom.type != AtomType::NIL) {
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(t->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (" + firstArgument->atom.symbol + " with incorrect argument type) : ", "",
                             result);
            t = t->right;
        }
        if (firstArgument->atom.symbol == "<") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (< with incorrect argument type) : ", "",
                             result);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::INT && nextResult->atom.type != AtomType::FLOAT)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (< with incorrect argument type) : ", "", nextResult);
                if (stof(result->atom.symbol) >= stof(nextResult->atom.symbol)) return makeAtom("nil");
                result = nextResult;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == ">") { // (> a b c), should compare a and b, then b and c
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (> with incorrect argument type) : ", "",
                             result);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::INT && nextResult->atom.type != AtomType::FLOAT)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (> with incorrect argument type) : ", "", nextResult);
                if (stof(result->atom.symbol) <= stof(nextResult->atom.symbol)) return makeAtom("nil");
                result = nextResult;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == ">=") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (>= with incorrect argument type) : ", "",
                             result);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::INT && nextResult->atom.type != AtomType::FLOAT)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (>= with incorrect argument type) : ", "", nextResult);
                if (stof(result->atom.symbol) < stof(nextResult->atom.symbol)) return makeAtom("nil");
                result = nextResult;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == "<=") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (<= with incorrect argument type) : ", "",
                             result);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::INT && nextResult->atom.type != AtomType::FLOAT)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (<= with incorrect argument type) : ", "", nextResult);
                if (stof(result->atom.symbol) > stof(nextResult->atom.symbol)) return makeAtom("nil");
                result = nextResult;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == "=") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR, "ERROR (= with incorrect argument type) : ", "",
                             result);
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::INT && nextResult->atom.type != AtomType::FLOAT)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (= with incorrect argument type) : ", "", nextResult);
                if (stof(result->atom.symbol) != stof(nextResult->atom.symbol)) return makeAtom("nil");
                result = nextResult;
                temp = temp->right;
            }
            return makeAtom("#t");
        }
        return nullptr;
    }

    /*
        string-append, string>?, string<?, string=?
    */
    AbstractSyntaxTreeNode *execStringOperation(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "string-append" && firstArgument->atom.symbol != "string>?" &&
            firstArgument->atom.symbol != "string<?" && firstArgument->atom.symbol != "string=?")
            return nullptr;
        if (size(current->right) < 2)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
        AbstractSyntaxTreeNode *t = current->right;
        while (t != nullptr && t->atom.type != AtomType::NIL) {
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(t->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (" + firstArgument->atom.symbol + " with incorrect argument type) : ", "",
                             result);
            t = t->right;
        }
        if (firstArgument->atom.symbol == "string-append") {
            string result = "\"";
            AbstractSyntaxTreeNode *temp = current->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult;
                try {
                    nextResult = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (nextResult->atom.type != AtomType::STRING)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (string-append with incorrect argument type) : ", "", nextResult);

                result += nextResult->atom.symbol.substr(1, nextResult->atom.symbol.size() - 2);
                temp = temp->right;
            }
            result += "\"";
            AbstractSyntaxTreeNode *resultNode =
                new AbstractSyntaxTreeNode(Atom(AtomType::STRING, result, -MOD, -MOD));
            garbageCollector.insert(resultNode);
            return resultNode;
        } else if (firstArgument->atom.symbol == "string>?") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (string>? with incorrect argument type) : ", "", result);
            string firstString = result->atom.symbol;
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult;
                try {
                    nextResult = exec(temp->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }
                if (nextResult->atom.type != AtomType::STRING)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (string>? with incorrect argument type) : ", "", nextResult);
                if (firstString <= nextResult->atom.symbol) return makeAtom("nil");
                firstString = nextResult->atom.symbol;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == "string<?") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (string<? with incorrect argument type) : ", "", result);
            string firstString = result->atom.symbol;
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::STRING)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (string<? with incorrect argument type) : ", "", nextResult);
                if (firstString >= nextResult->atom.symbol) return makeAtom("nil");
                firstString = nextResult->atom.symbol;
                temp = temp->right;
            }
            return makeAtom("#t");
        } else if (firstArgument->atom.symbol == "string=?") {
            AbstractSyntaxTreeNode *temp = current->right;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (string=? with incorrect argument type) : ", "", result);
            string firstString = result->atom.symbol;
            temp = temp->right;
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                AbstractSyntaxTreeNode *nextResult = exec(temp->left, level + 1, localSymbolTable);
                if (nextResult->atom.type != AtomType::STRING)
                    throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                                 "ERROR (string=? with incorrect argument type) : ", "", nextResult);
                if (firstString != nextResult->atom.symbol) return makeAtom("nil");
                firstString = nextResult->atom.symbol;
                temp = temp->right;
            }
            return makeAtom("#t");
        }
        return nullptr;
    }

    /*
        eqv?, equal?
    */
    bool isEqual(AbstractSyntaxTreeNode *result1, AbstractSyntaxTreeNode *result2) {
        if (result1 == nullptr && result2 == nullptr) return true;
        if (result1->atom.type != result2->atom.type) return false;
        if (result1->atom.symbol != result2->atom.symbol) return false;
        return isEqual(result1->left, result2->left) && isEqual(result1->right, result2->right);
    }
    AbstractSyntaxTreeNode *execEqivalenceTester(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "eqv?" && firstArgument->atom.symbol != "equal?") return nullptr;
        if (size(current->right) != 2)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
        AbstractSyntaxTreeNode *secondArgument = current->right->left;
        AbstractSyntaxTreeNode *thirdArgument = current->right->right->left;
        AbstractSyntaxTreeNode *result1;
        AbstractSyntaxTreeNode *result2;
        try {
            result1 = exec(secondArgument, level + 1, localSymbolTable);
            result2 = exec(thirdArgument, level + 1, localSymbolTable);
        } catch (Errors e) {
            if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "", e.node);
            }
            throw e;
        }
        if (firstArgument->atom.symbol == "eqv?") {
            if (result1->atom.type != result2->atom.type) return makeAtom("nil");
            if (isAtomButNotParen(result1->atom) && isAtomButNotParen(result2->atom) &&
                result1->atom.type != AtomType::STRING && result2->atom.type != AtomType::STRING) {
                if (result1->atom.symbol != result2->atom.symbol)
                    return makeAtom("nil");
                else
                    return makeAtom("#t");
            } else if (result1 == result2)
                return makeAtom("#t");
            return makeAtom("nil");
        } else if (firstArgument->atom.symbol == "equal?") {
            if (result1->atom.type != result2->atom.type) return makeAtom("nil");
            if (isEqual(result1, result2)) return makeAtom("#t");
            return makeAtom("nil");
        }
        return nullptr;
    }

    /*
        exec seq1 seq2 seq3 ...
        and return the last result
        Note: param current is NOT the root of AST Tree, current->left is seq1, current->right->left is seq2,
       ...
    */
    AbstractSyntaxTreeNode *execSequence(AbstractSyntaxTreeNode *current, int level,
                                         unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *temp = current;
        AbstractSyntaxTreeNode *result = nullptr;
        while (temp != nullptr && temp->atom.type != AtomType::NIL) {
            if (isAtomButNotParen(temp->atom)) {
                throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", parser.root);
            }
            temp = temp->right;
        }

        temp = current;

        while (temp != nullptr && temp->atom.type != AtomType::NIL) {
            try {
                result = exec(temp->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type != ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw e;
                } else if (temp->right == nullptr || temp->right->atom.type == AtomType::NIL) {
                    throw e;
                }
            }
            temp = temp->right;
        }

        if (result == nullptr)
            throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "", current);
        return result;
    }
    /*
        if, cond
    */
    AbstractSyntaxTreeNode *execConditionals(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "if" && firstArgument->atom.symbol != "cond") return nullptr;
        if (firstArgument->atom.symbol == "if") {
            int sizeOfArguments = size(current->right);
            if (sizeOfArguments < 2 || sizeOfArguments > 3)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
            AbstractSyntaxTreeNode *secondArgument = current->right->left;

            AbstractSyntaxTreeNode *conditionResult;
            try {
                conditionResult = exec(secondArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_TEST_CONDITION_ERROR,
                                 "ERROR (unbound test condition) : ", "", e.node);
                }
                throw e;
            }
            if (conditionResult->atom.type == AtomType::NIL) {
                if (sizeOfArguments == 3) { // have 'else'
                    AbstractSyntaxTreeNode *thirdArgument = current->right->right->right->left;
                    return exec(thirdArgument, level + 1, localSymbolTable);
                }
                throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "", current);
            } else {
                AbstractSyntaxTreeNode *thirdArgument = current->right->right->left;
                return exec(thirdArgument, level + 1, localSymbolTable);
            }
        } else if (firstArgument->atom.symbol == "cond") {
            // syntax: (cond (condition1 result1) (condition2 result2) ... (else result))
            // OR (cond condition1 result1)
            // Note: need to check if the else is a keyword or a symbol
            // only when all conditions are false, the else will exec
            int sizeOfArguments = size(current->right);
            if (sizeOfArguments < 1)
                throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
            AbstractSyntaxTreeNode *temp = current->right;
            // check errors
            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                if (size(temp->left) < 2)
                    throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
                if (isAtomButNotParen(temp->left->atom)) {
                    throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
                }
                temp = temp->right;
            }
            temp = current->right;

            while (temp != nullptr && temp->atom.type != AtomType::NIL) {
                if (size(temp->left) < 2)
                    throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
                if (isAtomButNotParen(temp->left->atom)) {
                    throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
                }
                AbstractSyntaxTreeNode *condition = temp->left->left;
                AbstractSyntaxTreeNode *result = temp->left->right;
                if (isAtomButNotParen(result->atom)) {
                    throw Errors(ErrorType::COND_FORMAT_ERROR, "ERROR (COND format) : ", "", current);
                }
                if (condition->atom.type == AtomType::SYMBOL && condition->atom.symbol == "else" &&
                    temp->right != nullptr && temp->right->atom.type == AtomType::NIL) {
                    return execSequence(result, level, localSymbolTable);
                }
                AbstractSyntaxTreeNode *conditionResult;
                try {
                    conditionResult = exec(condition, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_TEST_CONDITION_ERROR,
                                     "ERROR (unbound test condition) : ", "", e.node);
                    }
                    throw e;
                }
                if (conditionResult->atom.type != AtomType::NIL) {
                    return execSequence(result, level, localSymbolTable);
                }
                temp = temp->right;
            }
            throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "", current);
        }
        return nullptr;
    }

    /*
        begin
    */
    AbstractSyntaxTreeNode *execSequencingAndFunctional(
        AbstractSyntaxTreeNode *current, int level,
        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "begin") return nullptr;
        int sizeOfArguments = size(current->right);
        if (sizeOfArguments < 1)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " + firstArgument->atom.symbol);
        return execSequence(current->right, level, localSymbolTable); // Error is not handled here!!!!
    }

    /*
        quote, no exec
    */
    AbstractSyntaxTreeNode *execQuote(AbstractSyntaxTreeNode *current, int level,
                                      unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "quote" && firstArgument->atom.symbol != "'") return nullptr;
        if (size(current->right) != 1)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : quote");
        return current->right->left;
    }

    /*
        lambda,
        return:
        a function node, left is parameters, right is the sequence
    */
    AbstractSyntaxTreeNode *execLambda(AbstractSyntaxTreeNode *current, int level,
                                       unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "lambda") return nullptr;
        if (firstArgument->right != nullptr) {
            // evaluate the custom function
            AbstractSyntaxTreeNode *parameters =
                firstArgument->left; // Note that parameter is used in function definition,
            AbstractSyntaxTreeNode *arguments = current->right; // arguments is used in function call
            if (size(parameters) != size(arguments))
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : lambda");

            // Bind the parameters to the arguments
            unordered_map<string, AbstractSyntaxTreeNode *> newLocalSymbolTable;
            AbstractSyntaxTreeNode *temp_parameters = parameters;
            AbstractSyntaxTreeNode *temp_arguments = arguments;
            while (temp_parameters != nullptr && temp_parameters->atom.type != AtomType::NIL) {
                if (temp_parameters->left->atom.type != AtomType::SYMBOL)
                    throw Errors(ErrorType::LAMBDA_FORMAT_ERROR, "ERROR (LAMBDA format) : ", "", current);
                AbstractSyntaxTreeNode *result;
                try {
                    result = exec(temp_arguments->left, level + 1, localSymbolTable);
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                     e.node);
                    }
                    throw e;
                }

                newLocalSymbolTable[temp_parameters->left->atom.symbol] = result;
                temp_parameters = temp_parameters->right;
                temp_arguments = temp_arguments->right;
            }

            // execute sequences
            AbstractSyntaxTreeNode *result = nullptr;
            AbstractSyntaxTreeNode *sequence = firstArgument->right;
            while (sequence != nullptr && sequence->atom.type != AtomType::NIL) {
                try {
                    result = exec(sequence->left, level, newLocalSymbolTable);
                } catch (Errors e) {
                    if (e.type != ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw e;
                    } else if (sequence->right == nullptr || sequence->right->atom.type == AtomType::NIL) {
                        e.node = current;
                        throw e;
                    }
                }
                sequence = sequence->right;
            }

            return result;
        }
        if (size(current->right) < 2)
            throw Errors(ErrorType::LAMBDA_FORMAT_ERROR, "ERROR (LAMBDA format) : ", "", current);
        AbstractSyntaxTreeNode *secondArgument = current->right->left;
        if (secondArgument->atom.type != AtomType::NIL && secondArgument->atom.type != AtomType::DOT) {
            throw Errors(ErrorType::LAMBDA_FORMAT_ERROR, "ERROR (LAMBDA format) : ", "", current);
        } // (lambda x sexp) not (lambda (x) sexp)
        AbstractSyntaxTreeNode *symbols = secondArgument;
        while (symbols != nullptr && symbols->atom.type != AtomType::NIL) {
            if (symbols->left->atom.type != AtomType::SYMBOL) {
                throw Errors(ErrorType::LAMBDA_FORMAT_ERROR, "ERROR (LAMBDA format) : ", "", current);
            }
            symbols = symbols->right;
        }

        AbstractSyntaxTreeNode *result =
            new AbstractSyntaxTreeNode(Atom(AtomType::SYMBOL, "lambda", -MOD, -MOD));
        garbageCollector.insert(result);
        result->left = secondArgument;         // parameters
        result->right = current->right->right; // sequence
        result->isFunction = true;
        result->atom.isFunction = true;
        return result;
    }

    AbstractSyntaxTreeNode *execCustomFunction(AbstractSyntaxTreeNode *current, int level,
                                               unordered_map<string, AbstractSyntaxTreeNode *>
                                                   &localSymbolTable) { // execute user defined function like
                                                                        // (define (f x) (g x x) ...)
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *function = current->left;
        AbstractSyntaxTreeNode *parameters = function->left;
        AbstractSyntaxTreeNode *arguments = current->right;
        if (size(parameters) != size(arguments))
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                         "ERROR (incorrect number of arguments) : " +
                             function->atom.symbol); // TODO: may not Correct output!!!!

        // Bind the parameters to the arguments
        unordered_map<string, AbstractSyntaxTreeNode *> newLocalSymbolTable;
        AbstractSyntaxTreeNode *temp_parameters = parameters;
        AbstractSyntaxTreeNode *temp_arguments = arguments;
        while (temp_parameters != nullptr && temp_parameters->atom.type != AtomType::NIL) {
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(temp_arguments->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            newLocalSymbolTable[temp_parameters->left->atom.symbol] = result;
            temp_parameters = temp_parameters->right;
            temp_arguments = temp_arguments->right;
        }

        AbstractSyntaxTreeNode *result = nullptr;
        AbstractSyntaxTreeNode *sequence = function->right;
        while (sequence != nullptr && sequence->atom.type != AtomType::NIL) {
            try {
                result = exec(sequence->left, level, newLocalSymbolTable);
            } catch (Errors e) {
                if (e.type != ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw e;
                } else if (sequence->right == nullptr || sequence->right->atom.type == AtomType::NIL) {
                    e.node = current;
                    throw e;
                }
            }
            sequence = sequence->right;
        }

        if (result == nullptr)
            throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "", current);
        return result;
    }

    /*
        let ((symbols sexp) (symbols sexp) ...) sequence
    */
    AbstractSyntaxTreeNode *execLet(AbstractSyntaxTreeNode *current, int level,
                                    unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "let") return nullptr;
        if (size(current->right) < 2)
            throw Errors(ErrorType::LET_FORMAT_ERROR, "ERROR (LET format) : ", "", current);
        AbstractSyntaxTreeNode *secondArgument = current->right->left;
        AbstractSyntaxTreeNode *pair_sequence = secondArgument;
        while (pair_sequence != nullptr && pair_sequence->atom.type != AtomType::NIL) {
            if (size(pair_sequence->left) != 2)
                throw Errors(ErrorType::LET_FORMAT_ERROR, "ERROR (LET format) : ", "", current);
            if (pair_sequence->left->atom.type != AtomType::DOT)
                throw Errors(ErrorType::LET_FORMAT_ERROR, "ERROR (LET format) : ", "", current);
            if (pair_sequence->left->left->atom.type != AtomType::SYMBOL ||
                reservedWords.find(pair_sequence->left->left->atom.symbol) != reservedWords.end())
                throw Errors(ErrorType::LET_FORMAT_ERROR, "ERROR (LET format) : ", "", current);
            pair_sequence = pair_sequence->right;
        }

        unordered_map<string, AbstractSyntaxTreeNode *> newLocalSymbolTable = localSymbolTable;
        pair_sequence = secondArgument;
        while (pair_sequence != nullptr && pair_sequence->atom.type != AtomType::NIL) {
            AbstractSyntaxTreeNode *symbol = pair_sequence->left->left;
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(pair_sequence->left->right->left, 0, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::NO_RETURN_VALUE_ERROR_IN_FUNCTION,
                                 "ERROR (no return value) : ", "", e.node);
                }
                throw e;
            }
            newLocalSymbolTable[symbol->atom.symbol] = result;
            pair_sequence = pair_sequence->right;
        }

        AbstractSyntaxTreeNode *result = nullptr;
        AbstractSyntaxTreeNode *sequence = current->right->right;
        while (sequence != nullptr && sequence->atom.type != AtomType::NIL) {
            try {
                result = exec(sequence->left, level + 1, newLocalSymbolTable);
            } catch (Errors e) {
                if (e.type != ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw e;
                } else if (sequence->right == nullptr || sequence->right->atom.type == AtomType::NIL) {
                    e.node = current;
                    throw e;
                }
            }
            sequence = sequence->right;
        }
        if (result == nullptr)
            throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "", current);
        return result;
    }

    /*
        read, write, newline, symbol->string, number->string, display-string
    */
    AbstractSyntaxTreeNode *execIO(AbstractSyntaxTreeNode *current, int level,
                                   unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "read" && firstArgument->atom.symbol != "write" &&
            firstArgument->atom.symbol != "newline" && firstArgument->atom.symbol != "symbol->string" &&
            firstArgument->atom.symbol != "number->string" && firstArgument->atom.symbol != "display-string")
            return nullptr;

        if (firstArgument->atom.symbol == "read") {
            if (size(current->right) != 0)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : read");
            AbstractSyntaxTreeNode *result;
            SyntaxAnalyzer parser;
            LexicalAnalyzer scanner;
            vector<Atom> tokenList;
            try {
                scanner.lineNumber = 1;
                Atom startToken = scanner.getToken();
                parser.isSExpCorrect(startToken, scanner);
                tokenList = scanner.tokenList;
                parser.buildAbstractSyntaxTree(tokenList);
                result = parser.root;
            } catch (Errors e) {
                AbstractSyntaxTreeNode *errorNode =
                    new AbstractSyntaxTreeNode(Atom(AtomType::STRING, "\"" + e.message + "\"", -MOD, -MOD));
                garbageCollector.insert(errorNode);
                if (e.type == ErrorType::UNEXPECTED_TOKEN_ERROR) try {
                        scanner.skipToNextLine(e.unexpectedToken.back());
                    } catch (Errors e) {
                        cout << "\n> " << e.what() << endl;
                    }
                return errorNode;
            }

            return result;
        } else if (firstArgument->atom.symbol == "write") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : write");
            AbstractSyntaxTreeNode *result = exec(current->right->left, level + 1, localSymbolTable);
            parser.printAbstractSyntaxTreeAtom(result, 0, false);
            return result;
        } else if (firstArgument->atom.symbol == "newline") {
            if (size(current->right) != 0)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : newline");
            cendl;
            return makeAtom("nil");
        } else if (firstArgument->atom.symbol == "symbol->string") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : symbol->string");
            AbstractSyntaxTreeNode *result = exec(current->right->left, level + 1, localSymbolTable);
            if (result->atom.type != AtomType::SYMBOL)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (symbol->string with incorrect argument type) : ", "", result);
            string resultString = "\"" + result->atom.symbol + "\"";
            AbstractSyntaxTreeNode *resultNode =
                new AbstractSyntaxTreeNode(Atom(AtomType::STRING, resultString, -MOD, -MOD));
            garbageCollector.insert(resultNode);
            return resultNode;
        } else if (firstArgument->atom.symbol == "number->string") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : number->string");
            AbstractSyntaxTreeNode *result = exec(current->right->left, level + 1, localSymbolTable);
            if (result->atom.type != AtomType::INT && result->atom.type != AtomType::FLOAT)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (number->string with incorrect argument type) : ", "", result);
            string resultString = "\"" + result->atom.symbol + "\"";
            AbstractSyntaxTreeNode *resultNode =
                new AbstractSyntaxTreeNode(Atom(AtomType::STRING, resultString, -MOD, -MOD));
            garbageCollector.insert(resultNode);
            return resultNode;
        } else if (firstArgument->atom.symbol == "display-string") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : display-string");
            AbstractSyntaxTreeNode *result = exec(current->right->left, level + 1, localSymbolTable);
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (display-string with incorrect argument type) : ", "", result);
            cout << result->atom.symbol.substr(1, result->atom.symbol.size() - 2);
            return result;
        }

        return nullptr;
    }

    /*
        create-error-object, error-object?
    */
    AbstractSyntaxTreeNode *execError(AbstractSyntaxTreeNode *current, int level,
                                      unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "create-error-object" &&
            firstArgument->atom.symbol != "error-object?")
            return nullptr;
        if (firstArgument->atom.symbol == "create-error-object") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : create-error-object");
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(current->right->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::STRING)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (create-error-object with incorrect argument type) : ", "", result);
            AbstractSyntaxTreeNode *errorNode =
                new AbstractSyntaxTreeNode(Atom(AtomType::ERROR, result->atom.symbol, -MOD, -MOD));
            garbageCollector.insert(errorNode);
            return errorNode;
        } else if (firstArgument->atom.symbol == "error-object?") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : error-object?");
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(current->right->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::ERROR) return makeAtom("nil");
            return makeAtom("#t");
        }
        return nullptr;
    }

    AbstractSyntaxTreeNode *execEval(AbstractSyntaxTreeNode *current, int level,
                                     unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "eval") return nullptr;
        if (size(current->right) != 1)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR, "ERROR (incorrect number of arguments) : eval");
        AbstractSyntaxTreeNode *result = exec(current->right->left, level + 1, localSymbolTable);
        return exec(result, 0, localSymbolTable);
    }

    AbstractSyntaxTreeNode *execSet(AbstractSyntaxTreeNode *current, int level,
                                    unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "set!") return nullptr;
        if (size(current->right) != 2)
            throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR, "ERROR (incorrect number of arguments) : set!");
        AbstractSyntaxTreeNode *secondArgument = current->right->left;
        AbstractSyntaxTreeNode *thirdArgument = current->right->right->left;
        if (secondArgument->atom.type != AtomType::SYMBOL ||
            reservedWords.find(secondArgument->atom.symbol) != reservedWords.end())
            throw Errors(ErrorType::SET_FORMAT_ERROR, "ERROR (SET! format) : ", "", current);

        AbstractSyntaxTreeNode *result;

        if (isAtomButNotParen(thirdArgument->atom) && thirdArgument->atom.type != AtomType::SYMBOL) {
            result = evaluateTree(copyNode(thirdArgument), localSymbolTable);
        } else if (thirdArgument->atom.type == AtomType::DOT &&
                   thirdArgument->left->atom.symbol == "lambda") {
            result = exec(thirdArgument, level + 1, localSymbolTable);
            result->atom.isFunction = true;
        } else {
            try {
                result = exec(thirdArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::NO_RETURN_VALUE_ERROR_IN_FUNCTION,
                                 "ERROR (no return value) : ", "", e.node);
                }
                throw e;
            }
        }

        if (localSymbolTable.find(secondArgument->atom.symbol) != localSymbolTable.end()) {
            localSymbolTable[secondArgument->atom.symbol] = result;
        } else {
            globalSymbolTable[secondArgument->atom.symbol] = result;
        }
        return result;
    }

    AbstractSyntaxTreeNode *execVerbose(AbstractSyntaxTreeNode *current, int level,
                                        unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (current == nullptr) return nullptr;
        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (firstArgument->atom.symbol != "verbose" && firstArgument->atom.symbol != "verbose?")
            return nullptr;
        if (firstArgument->atom.symbol == "verbose") {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : verbose");
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(current->right->left, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::UNBOUND_PARAMETER_ERROR, "ERROR (unbound parameter) : ", "",
                                 e.node);
                }
                throw e;
            }
            if (result->atom.type != AtomType::NIL && result->atom.type != AtomType::T)
                throw Errors(ErrorType::ARGUMENT_TYPE_ERROR,
                             "ERROR (verbose with incorrect argument type) : ", "", result);
            if (result->atom.type == AtomType::NIL) {
                verbose = false;
            } else {
                verbose = true;
            }
            return result;
        } else if (firstArgument->atom.symbol == "verbose?") {
            if (size(current->right) != 0)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : verbose?");
            if (verbose) return makeAtom("#t");
            return makeAtom("nil");
        }
        return nullptr;
    }

    AbstractSyntaxTreeNode *exec(AbstractSyntaxTreeNode *current, int level,
                                 unordered_map<string, AbstractSyntaxTreeNode *> &localSymbolTable) {
        if (isAtomButNotParen(current->atom)) {
            if (current->atom.type != AtomType::SYMBOL) {
                return current;
            }

            if (localSymbolTable.find(current->atom.symbol) != localSymbolTable.end()) {
                return localSymbolTable[current->atom.symbol];
            }

            if (globalSymbolTable.find(current->atom.symbol) == globalSymbolTable.end()) {
                throw Errors(ErrorType::UNBOUND_ERROR, "ERROR (unbound symbol) : " + current->atom.symbol);
            }

            return globalSymbolTable[current->atom.symbol]; // TODO: return the value of the symbol
        }

        if (current->left == nullptr) return current; // atom

        if (!isList(current)) {
            throw Errors(ErrorType::NON_LIST_ERROR, "ERROR (non-list) : ", "", current);
        }

        AbstractSyntaxTreeNode *firstArgument = current->left;
        if (isAtomButNotParen(firstArgument->atom)) {
            if (firstArgument->atom.type != AtomType::SYMBOL) { // is atom but not symbols
                throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR,
                             "ERROR (attempt to apply non-function) : ", "", firstArgument);
            }
            // TODO: check if it is a function name

            AbstractSyntaxTreeNode *result = execTopLevelFunctions(current, level, localSymbolTable);
            if (result != nullptr)
                return result;
            else if (firstArgument->atom.symbol == "define") {
                if (current->right->left->atom.type == AtomType::DOT) {
                    cout << (verbose ? current->right->left->left->atom.symbol + " defined\n" : "");
                } else {
                    cout << (verbose ? current->right->left->atom.symbol + " defined\n" : "");
                }
                return makeAtom("");
            }
            result = execConstructors(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execPartAccessors(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execPrimitivePredicates(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execBasicArithmetic(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execLogical(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execComparator(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execStringOperation(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execEqivalenceTester(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execConditionals(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execSequencingAndFunctional(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execQuote(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execLambda(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execLet(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execIO(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execError(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execEval(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execSet(current, level, localSymbolTable);
            if (result != nullptr) return result;
            result = execVerbose(current, level, localSymbolTable);
            if (result != nullptr) return result;

            // Is it a known symbol?
            if (globalSymbolTable.find(firstArgument->atom.symbol) == globalSymbolTable.end() &&
                localSymbolTable.find(firstArgument->atom.symbol) == localSymbolTable.end()) {
                throw Errors(ErrorType::UNBOUND_ERROR,
                             "ERROR (unbound symbol) : " + firstArgument->atom.symbol);
            }

            // Is it a known function?
            if (localSymbolTable.find(firstArgument->atom.symbol) != localSymbolTable.end()) {
                if (!localSymbolTable[firstArgument->atom.symbol]->isFunction) {
                    throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR,
                                 "ERROR (attempt to apply non-function) : ", "",
                                 localSymbolTable[firstArgument->atom.symbol]);
                }
                AbstractSyntaxTreeNode *result = makeAtom(".");
                result->left = localSymbolTable[firstArgument->atom.symbol];
                result->right = current->right;
                try {
                    if (result->left->atom.symbol == "lambda") {
                        return execLambda(result, level, localSymbolTable);
                    } else if (result->left->left == nullptr) {
                        return exec(result, level, localSymbolTable); // (define a car)
                    } else {
                        return execCustomFunction(result, level,
                                                  localSymbolTable); // (define (f x) (g x x) ...)
                    }
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "",
                                     current);
                    }
                    throw e;
                }
            } else if (globalSymbolTable[firstArgument->atom.symbol]
                           ->isFunction) { // (define a car) or (define (f x) (g x x) ...)
                AbstractSyntaxTreeNode *result = makeAtom(".");
                result->left = globalSymbolTable[firstArgument->atom.symbol]; // function name and parameters
                result->right = current->right;                               // user passed arguments

                try {
                    if (result->left->atom.symbol == "lambda") {
                        return execLambda(result, level, localSymbolTable);
                    } else if (result->left->left == nullptr) {
                        return exec(result, level, localSymbolTable); // (define a car)
                    } else {
                        return execCustomFunction(result, level,
                                                  localSymbolTable); // (define (f x) (g x x) ...)
                    }
                } catch (Errors e) {
                    if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                        throw Errors(ErrorType::NO_RETURN_VALUE_ERROR, "ERROR (no return value) : ", "",
                                     current);
                    }
                    throw e;
                }

            } else {
                throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR,
                             "ERROR (attempt to apply non-function) : ", "",
                             globalSymbolTable[firstArgument->atom.symbol]);
            }

        } else if (firstArgument->atom.type == UNKNOWN || firstArgument->atom.type == DOT) {
            // TODO: it is ((. . .) . . .), should evaluate the first . . . and then apply the rest
            if (firstArgument->left != nullptr && firstArgument->left->atom.type == AtomType::QUOTE) {
                return exec(firstArgument, level + 1, localSymbolTable);
            }
            AbstractSyntaxTreeNode *result;
            try {
                result = exec(firstArgument, level + 1, localSymbolTable);
            } catch (Errors e) {
                if (e.type == ErrorType::NO_RETURN_VALUE_ERROR) {
                    throw Errors(ErrorType::NO_RETURN_VALUE_ERROR_IN_FUNCTION,
                                 "ERROR (no return value) : ", "", e.node);
                }
                throw e;
            }

            if (localSymbolTable.find(result->atom.symbol) != localSymbolTable.end()) {
                if (!localSymbolTable[result->atom.symbol]->isFunction) {
                    throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR,
                                 "ERROR (attempt to apply non-function) : ", "", result);
                }
                AbstractSyntaxTreeNode *nextTree = copyNode(current);
                nextTree->left = result;
                return exec(nextTree, level + 1, localSymbolTable);
            }
            if (globalSymbolTable.find(result->atom.symbol) == globalSymbolTable.end() ||
                !globalSymbolTable[result->atom.symbol]->isFunction) {
                throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR,
                             "ERROR (attempt to apply non-function) : ", "", result);
            }
            // current->left = result;
            AbstractSyntaxTreeNode *nextTree = copyNode(current);
            nextTree->left = result;
            return exec(nextTree, level + 1, localSymbolTable);
            /// return exec(result, level + 1);
        } else if (firstArgument->atom.type == QUOTE) {
            if (size(current->right) != 1)
                throw Errors(ErrorType::NUMBER_OF_ARGUMENT_ERROR,
                             "ERROR (incorrect number of arguments) : quote");

            return current->right->left;
        }

        else {
            throw Errors(ErrorType::APPLY_NON_FUNCTION_ERROR, "ERROR (attempt to apply non-function) : ", "",
                         firstArgument);
        }
        return nullptr;
    }
} executor;

signed main() {
    // CHIWAWA;
    // GETOUT;

    int testNum = 1;
    cin >> testNum;

    cout << "Welcome to OurScheme!" << endl;
    cin.ignore();
    cout << "\n";
    do {
        cout << "> ";

        try {
            executor.init();
            Atom startToken = scanner.getToken();
            parser.isSExpCorrect(startToken);
            parser.buildAbstractSyntaxTree();
            if (parser.isExit()) {
                cendl;
                break;
            }
            unordered_map<string, AbstractSyntaxTreeNode *> localSymbolTable;
            AbstractSyntaxTreeNode *result = executor.exec(parser.root, 0, localSymbolTable);
            parser.printAbstractSyntaxTreeAtom(result);
            if (result != nullptr && !result->atom.symbol.empty()) cendl;

            if (scanner.eof)
                throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");

            scanner.reset();
            parser.clearAbstractSyntaxTree();

        } catch (Errors e) {
            if (e.type == ErrorType::EOF_ERROR) {
                cout << e.what() << endl;
                break;
            } else
                cout << e.what();

            if (e.type == ErrorType::DEFINE_FORMAT_ERROR || e.type == ErrorType::ARGUMENT_TYPE_ERROR ||
                e.type == ErrorType::COND_FORMAT_ERROR || e.type == ErrorType::NON_LIST_ERROR ||
                e.type == ErrorType::NO_RETURN_VALUE_ERROR || e.type == ErrorType::APPLY_NON_FUNCTION_ERROR ||
                e.type == ErrorType::LAMBDA_FORMAT_ERROR || e.type == ErrorType::LET_FORMAT_ERROR ||
                e.type == ErrorType::UNBOUND_PARAMETER_ERROR ||
                e.type == ErrorType::UNBOUND_TEST_CONDITION_ERROR ||
                e.type == ErrorType::UNBOUND_CONDITION_ERROR ||
                e.type == ErrorType::NO_RETURN_VALUE_ERROR_IN_FUNCTION) {
                parser.printAbstractSyntaxTreeAtom(e.node);
                cendl;
            }

            else
                cendl;

            if (e.type == ErrorType::UNEXPECTED_TOKEN_ERROR) try {
                    scanner.skipToNextLine(e.unexpectedToken.back());
                } catch (Errors e) {
                    cout << "\n> " << e.what() << endl;
                    break;
                }

            scanner.errorreset();
            parser.clearAbstractSyntaxTree();
            try {
                scanner.skipWhiteSpaces();
                scanner.skipLineComment(false);
            } catch (Errors e) {
                cout << e.what() << endl;
                break;
            }
        }

        cendl;

    } while (1);

    executor.clear();

    cout << "Thanks for using OurScheme!" << endl;
    // cerr << "Time: " << (double)clock() / (double)CLOCKS_PER_SEC << '\n';
    return 0;
}