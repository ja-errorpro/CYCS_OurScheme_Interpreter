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
#include <sstream>
#include <stack>
#include <string>
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
    INT = 0x4a8763
};

class Atom {
   public:
    int type;
    string symbol;
    int line;
    int column;
    Atom() {
        type = AtomType::UNKNOWN;
        symbol = "";
    }

    Atom(int type, string symbol, int line, int column) {
        this->type = type;
        this->symbol = symbol;
        this->line = line;
        this->column = column;
    }
};

enum ErrorType { UNKNOWN_ERROR = 0, EOF_ERROR, UNEXPECTED_TOKEN_ERROR, NO_CLOSING_QUOTE_ERROR };

class Errors {
    // exception class
   public:
    ErrorType type;
    string message;
    string unexpectedToken;
    Errors(ErrorType e, string message, string unexpectedToken = "") {
        this->type = e;
        this->message = message;
        this->unexpectedToken = unexpectedToken;
    }
    string what() { return message; }
};

class CompileErr0rDebugger { // This class only used to debug this program
   public:
    template <typename T>
    void print(T t) {
        cout << t << endl;
    }

    template <typename T, typename... Args>
    void print(T t, Args... args) {
        cout << t << " ";
        print(args...);
    }

    template <typename T, typename... Args>
    void printd(T t, Args... args) {
        cerr << "\033[1;33m[DEBUG] \033[0m";
        cerr << t << " ";
        initializer_list<int>{(cerr << args << " ", 0)...};
        cerr << endl;
    }

    string AtomTypeToString(int type) {
        switch (type) {
            case AtomType::NIL:
                return "NIL";
            case AtomType::DOT:
                return "DOT";
            case AtomType::NUMBER:
                return "NUMBER";
            case AtomType::FLOAT:
                return "FLOAT";
            case AtomType::STRING:
                return "STRING";
            case AtomType::SYMBOL:
                return "SYMBOL";
            case AtomType::T:
                return "T";
            case AtomType::QUOTE:
                return "quote";
            case AtomType::LEFT_PAREN:
                return "(";
            case AtomType::RIGHT_PAREN:
                return ")";
            case AtomType::INT:
                return "INT";
            default:
                return "UNKNOWN";
        }
    }
} debugger;

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
            // cerr << "!";
            columnNumber = 0;
            lineNumber++;
            // cerr << "line: " << lineNumber << endl;
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

        char c = getNextChar();
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
                if (c == EOF)
                    throw Errors(ErrorType::EOF_ERROR, "ERROR (no more input) : END-OF-FILE encountered");
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

            c = getNextChar();
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
        if (s == "QUOTE" || s == "'") return AtomType::QUOTE;
        if (s == "(") return AtomType::LEFT_PAREN;
        if (s == ")") return AtomType::RIGHT_PAREN;
        if (s[0] == '"') return AtomType::STRING;

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
        // cerr << "Symbol: " << s << " column: " << columnNumber << endl;
        atom.column = columnNumber - s.size() + 1;
        return atom;
    }

    /*
     Only takes a peek at the next token,
     but not get it.
    */
    string peekToken() {
        if (peekTarget != "") return peekTarget;
        string token = "";

        // cerr << "nws\n";
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

        peekTarget = token;

        return token;
    }

    Atom getToken() {
        if (peekTarget == "") peekToken();
        Atom atom = StringToAtom(peekTarget);
        // debugger.printd("Token: ", atom.symbol, " Type: ", debugger.AtomTypeToString(atom.type),
        //                " Line: ", atom.line, " Column: ", atom.column);
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

    struct AbstractSyntaxTreeNode {
        Atom atom;
        AbstractSyntaxTreeNode *left, *right;
        AbstractSyntaxTreeNode() {
            atom = Atom(AtomType::UNKNOWN, "UNKNOWN", -MOD, -MOD);
            left = right = nullptr;
        }
        AbstractSyntaxTreeNode(Atom atom) {
            this->atom = atom;
            left = right = nullptr;
        }
    } *root;

    void buildAbstractSyntaxTree(AbstractSyntaxTreeNode *&current, vector<Atom> tokenList,
                                 int &tokenListindex) {
        if (tokenListindex >= (int)tokenList.size()) return;
        if (current == nullptr) current = new AbstractSyntaxTreeNode();
        // cerr << tokenListindex << tokenList[tokenListindex].symbol << endl;
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
                // cerr << "d:" << tokenListindex << tokenList[tokenListindex].symbol << endl;
                buildAbstractSyntaxTree(temp->right->left, tokenList, tokenListindex);
                temp = temp->right;
            }
            // cerr << "o:" << tokenListindex << " " << tokenList[tokenListindex].symbol << endl;
            if (tokenListindex < (int)tokenList.size() && tokenList[tokenListindex].type == AtomType::DOT) {
                current->atom = Atom(AtomType::DOT, ".", tokenList[tokenListindex].line,
                                     tokenList[tokenListindex].column);
                tokenListindex++;
                buildAbstractSyntaxTree(temp->right, tokenList, tokenListindex);
                tokenListindex++;
                // cerr << "i:" << tokenListindex << " " << tokenList[tokenListindex].symbol << endl;
            } else {
                temp->right = new AbstractSyntaxTreeNode(Atom(AtomType::NIL, "nil", -MOD, -MOD));
                if (tokenListindex < (int)tokenList.size() - 1) tokenListindex++;
            }

        } else if (tokenList[tokenListindex].type == AtomType::QUOTE) {
            // QUOTE <S-exp>
            // 1. put QUOTE to left child
            // 2. move to right child, build next S-exp in "current" left child
            // 3. finally, put nil-node into "current" right child

            current->left = new AbstractSyntaxTreeNode(tokenList[tokenListindex]);
            tokenListindex++;
            current->right = new AbstractSyntaxTreeNode();
            buildAbstractSyntaxTree(current->right->left, tokenList, tokenListindex);
            current->right->right = new AbstractSyntaxTreeNode(Atom(AtomType::NIL, "nil", -MOD, -MOD));
        } else {
            // <ATOM>
            // just put the atom into current node
            current->atom = tokenList[tokenListindex];
            tokenListindex++;
        }
    }

   public:
    SyntaxAnalyzer() { root = nullptr; }
    ~SyntaxAnalyzer() { clearAbstractSyntaxTree(root); }

    void clearAbstractSyntaxTree(AbstractSyntaxTreeNode *current) {
        if (current == nullptr) return;
        clearAbstractSyntaxTree(current->left);
        clearAbstractSyntaxTree(current->right);
        delete current;
        current = nullptr;
    }

    void clearAbstractSyntaxTree() {
        clearAbstractSyntaxTree(root);
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
        debugger.print(current->atom.symbol);
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
        else
            cout << atom.symbol;
    }

    void printRightLeftChildren(AbstractSyntaxTreeNode *current, int alignSpaces) {
        if (current == nullptr) return;
        AbstractSyntaxTreeNode *right = current->right;
        while (right != nullptr) {
            if (right->atom.type == AtomType::UNKNOWN) {
                AbstractSyntaxTreeNode *left = right->left;
                if (left->atom.type == AtomType::UNKNOWN ||
                    left->atom.type == AtomType::DOT) { // todo: test this case if it is correct
                    printSpaces(alignSpaces + 2);
                    printAbstractSyntaxTree(left, alignSpaces + 2);
                } else {
                    printSpaces(alignSpaces + 2);
                    printAtom(left->atom);
                    cendl;
                }
                if (right->right != nullptr && right->right->atom.type != AtomType::NIL &&
                    right->right->atom.type != AtomType::DOT &&
                    right->right->atom.type != AtomType::UNKNOWN) {
                    printSpaces(alignSpaces + 2);
                    cout << ".";
                    cendl;
                }
            } else if (right->atom.type == AtomType::DOT) {
                AbstractSyntaxTreeNode *left = right->left;
                if (left->atom.type == AtomType::UNKNOWN ||
                    left->atom.type == AtomType::DOT) { // todo: test this case if it is correct
                    printSpaces(alignSpaces + 2);
                    printAbstractSyntaxTree(left, alignSpaces + 2);
                } else {
                    printSpaces(alignSpaces + 2);
                    printAtom(left->atom);
                    cendl;
                }
                if (right->right != nullptr && right->right->atom.type != AtomType::NIL &&
                    right->right->atom.type != AtomType::DOT &&
                    right->right->atom.type != AtomType::UNKNOWN) {
                    printSpaces(alignSpaces + 2);
                    cout << ".";
                    cendl;
                }
            } else if (right->atom.type != AtomType::NIL) { // atom
                // cerr << "atom\n";
                printSpaces(alignSpaces + 2);
                printAtom(right->atom);

                cendl;
            } else {
                // cerr << (right->atom.symbol) << endl;
                printRightLeftChildren(right, alignSpaces + 2);
            }
            right = right->right;
        }
    }

    void printAbstractSyntaxTree(AbstractSyntaxTreeNode *current = nullptr, int alignSpaces = 0) {
        // like pretty print in python
        if (current == nullptr) current = root;
        if (current == nullptr) return;
        if (current->atom.type == AtomType::UNKNOWN || current->atom.type == AtomType::DOT) {
            cout << "( ";
            AbstractSyntaxTreeNode **left = &current->left;
            AbstractSyntaxTreeNode **right = &current->right;
            if ((*left)->atom.type == AtomType::QUOTE) { // left child is quote
                printAtom((*left)->atom);                // quote
                cendl;
                // printSpaces(alignSpaces + 2);
                /*if ((*right)->atom.type == AtomType::NIL) {
                    printAtom((*right)->atom);
                    cendl;
                } else if ((*right)->atom.type == AtomType::UNKNOWN ||
                           (*right)->atom.type == AtomType::DOT) { // '(1 2 3)
                    printAbstractSyntaxTree((*right)->left, alignSpaces + 2);
                    // cendl;
                }*/
            } else if ((*left)->atom.type == AtomType::UNKNOWN ||
                       (*left)->atom.type == AtomType::DOT) { // left child is .
                printAbstractSyntaxTree((*left), alignSpaces + 2);
                // printRightLeftChildren(current, alignSpaces);
            } else {
                printAtom((*left)->atom);
                cendl;
                // cerr << "right: " << (*right)->atom.symbol << endl;
                if (current->atom.type == AtomType::DOT && (*right)->atom.type != AtomType::NIL &&
                    (*right)->atom.type != AtomType::DOT && (*right)->atom.type != AtomType::UNKNOWN) {
                    printSpaces(alignSpaces + 2);
                    cout << ".";
                    cendl;
                }
            }
            printRightLeftChildren(current, alignSpaces);
            printSpaces(alignSpaces);
            cout << ")";
        } else {
            if (current->atom.type != AtomType::NIL ||
                (current->atom.type == AtomType::NIL && current == root)) {
                printSpaces(alignSpaces);
                printAtom(current->atom);
            }
        }
        cendl;
    }
    bool isAtomButNotLeftParen(Atom atom) {
        return atom.type == AtomType::SYMBOL || atom.type == AtomType::INT || atom.type == AtomType::FLOAT ||
               atom.type == AtomType::STRING || atom.type == AtomType::NIL || atom.type == AtomType::T;
    }
    bool isSExpCorrect(Atom startAtom) {
        if (isAtomButNotLeftParen(startAtom)) return true; // Note: () is a valid atom
        if (startAtom.type == AtomType::LEFT_PAREN) {
            string nextToken = scanner.peekToken();
            int nextAtomType = scanner.StringToAtom(nextToken).type;
            if (nextAtomType == AtomType::RIGHT_PAREN) {
                scanner.getToken();
                return true;
            }

            Atom nextAtom = scanner.getToken(); // Is this another S-exp or DOT or RIGHT_PAREN?
            if (nextAtomType == AtomType::DOT) {
                throw Errors(ErrorType::UNEXPECTED_TOKEN_ERROR,
                             "ERROR (unexpected token) : atom or '(' expected when token at Line " +
                                 to_string(nextAtom.line) + " Column " + to_string(nextAtom.column) +
                                 " is >>" + nextAtom.symbol + "<<",
                             nextAtom.symbol);
            }

            while (nextAtom.type != AtomType::RIGHT_PAREN && nextAtom.type != AtomType::DOT) {
                isSExpCorrect(nextAtom);
                nextAtom = scanner.getToken();
            }
            if (nextAtom.type == AtomType::DOT) {
                nextAtom = scanner.getToken();
                isSExpCorrect(nextAtom);
                nextAtom = scanner.getToken();
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
            Atom nextAtom = scanner.getToken();
            isSExpCorrect(nextAtom);
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

signed main() {
    // CHIWAWA;
    // GETOUT;

    int testNum = 1;
    cin >> testNum;

    cout << "Welcome to OurScheme!" << endl;
    cin.ignore();
    do {
        cout << "\n> ";

        try {
            // cerr << "Start Skip White Spaces\n";
            Atom startToken = scanner.getToken();
            // cerr << "Start Check SExp\n";
            parser.isSExpCorrect(startToken);
            parser.buildAbstractSyntaxTree();
            if (parser.isExit()) {
                cendl;
                break;
            }
            parser.printAbstractSyntaxTree();
            /// cerr << "===";
            // parser.printAll();
            // cendl;
            scanner.reset();
            parser.clearAbstractSyntaxTree();

        } catch (Errors e) {
            if (e.type == ErrorType::EOF_ERROR) {
                cout << e.what() << endl;
                break;
            } else
                cout << e.what() << endl;

            if (e.type != ErrorType::NO_CLOSING_QUOTE_ERROR) try {
                    scanner.skipToNextLine(e.unexpectedToken.back());
                } catch (Errors e) {
                    cout << "\n> " << e.what() << endl;
                    break;
                }

            scanner.errorreset();
        }

        // scanner.SkipWhiteSpacesExceptReturn(false);
        // scanner.skipLineComment(false);

    } while (1);

    cout << "Thanks for using OurScheme!" << endl;
    // cerr << "Time: " << (double)clock() / (double)CLOCKS_PER_SEC << '\n';
    return 0;
}