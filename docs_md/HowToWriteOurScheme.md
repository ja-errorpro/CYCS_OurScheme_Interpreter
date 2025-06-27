# 如何撰寫 OurScheme

> **最後修改日期 :** 2017年2月22日

---

## 程式碼主體架構

### 專案一的主體架構

```cpp
Print 'Welcome to OurScheme!'

repeat
  Print '> '
  ReadSExp(exp);
  if no error
    then PrintSExp(exp);
  else 
    PrintErrorMessage();
until (使用者輸入 '(exit)' 或 遇到檔案結尾)

Print 'Thanks for using OurScheme!' or EOF error message
```

### 專案二至四的主體架構

```cpp
Print 'Welcome to OurScheme!'

repeat
  Print : '> '
  ReadSExp( s_exp );
  if no error
    then result <- EvalSExp( s_exp );
         if error
           PrintErrorMessage();
         else
           PrintSExp( result );
  else PrintErrorMessage();
until 使用者輸入 (exit) 或 遇到檔案結尾

Print 'Thanks for using OurScheme!' or EOF error message
```

---

## 一、讀取一個 S-expression

首先，嘗試讀取一個 S-expression。

### 終端符 (Terminal)

-   `LEFT-PAREN`  // `(`
-   `RIGHT-PAREN` // `)`
-   `INT`         // 例如：`'123'`, `'+123'`, `'-123'`
-   `STRING`      // "This is an example of a string." (字串不跨行)
    -   OurScheme 的字串有類似 C/Java `printf()` 的跳脫概念，但僅限於 `\n`, `\"`, `\t` 與 `\\`。如果 `\` 字元之後的字元不是 `n`, `"`, `t`, 或 `\`，此 `\` 字元就無特殊意義（只是一個普通字元）。
-   `DOT`         // `.`
-   `FLOAT`       // 例如：`'123.567'`, `'123.'`, `'.567'`, `'+123.4'`, `'-.123'`
-   `NIL`         // `'nil'` 或 `'#f'`，但不是 `'NIL'` 或 `'nIL'`
-   `T`           // `'t'` 或 `'#t'`，但不是 `'T'` 或 `'#T'`
-   `QUOTE`       // `''`
-   `SYMBOL`      // 一個連續的可印出字元序列，它不是數字，且不包含 `(`, `)`, 單引號, 雙引號和空白字元；符號是區分大小寫的。

**注意：**

-   除了字串，token 由以下「分隔符」分隔：
    -   (a) 一個或多個空白字元
    -   (b) `(` (注意：`(` 本身就是一個 token)
    -   (c) `)` (注意：`)` 本身就是一個 token)
    -   (d) 單引號字元 (`''`) (注意：這本身就是一個 token)
    -   (e) 雙引號字元 (`"`) (注意：這開始一個 STRING)
    -   (f) 分號 (`;`) (注意：這是一行註解的開始)

**範例：**

-   `'3.25'` 是一個 `FLOAT`。
-   `'3.25a'` 是一個 `SYMBOL`。
-   `'a.b'` 是一個 `SYMBOL`。
-   `'#f'` 是 `NIL`。
-   `'#fa'` (或 `'a#f'`) 是一個 `SYMBOL`。

**注意：**

-   `.` 可以有多種意義：它可以是 `FLOAT` 的一部分、`SYMBOL` 的一部分，或是一個 `DOT`。只有當它「單獨存在」時，它才表示 `DOT`。
-   `#` 也可以有兩種意義：它可以是 `NIL` (或 `T`) 的一部分，或 `SYMBOL` 的一部分。只有當它是 `'#t'` 或 `'#f'` 且「單獨存在」時，它才是 `NIL` (或 `T`) 的一部分。

### EBNF 語法

```ebnf
<S-exp> ::= <ATOM> 
            | LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
            | QUOTE <S-exp>
            
<ATOM>  ::= SYMBOL | INT | FLOAT | STRING 
            | NIL | T | LEFT-PAREN RIGHT-PAREN
```

一旦讀取 S-expression 的嘗試失敗，包含錯誤 token 的那一行將被忽略。從下一個輸入行開始讀取 S-expression。

---

## 二、語法檢查

在求值之前，務必檢查使用者輸入的語法，確保它是一個 S-expression。

使用者輸入可能出現的四種語法錯誤的相關訊息（範例）如下：

```
ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<<
ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<<
ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y
ERROR (no more input) : END-OF-FILE encountered
```

---

## 三、eval() 的錯誤處理流程 (Python 偽代碼)

> **注意：** 一旦發生錯誤，對 `eval_sexp()` 的呼叫就會立即終止並引發例外。

為了更容易理解 `eval()` 的判斷流程，這裡提供一份以 Python 風格寫成的偽代碼。

```python
def eval_sexp(s_exp, environment):
    """
    在給定的環境中對 S-expression 進行求值。
    這是一個求值邏輯的偽代碼表示。
    """

    # 步驟 1: 處理原子 (Atom)
    if is_atom(s_exp):
        if is_symbol(s_exp):
            # 對於符號，在環境中查找其綁定
            binding = environment.lookup(s_exp)
            if binding is None:
                raise Exception(f"ERROR (unbound symbol) : {s_exp}")
            return binding
        else:
            # 對於其他原子 (數字、字串、#t、nil)，直接回傳它們本身
            return s_exp

    # 步驟 2: 處理列表 (函式呼叫或特殊形式)
    if not is_list(s_exp):
        raise Exception(f"ERROR (non-list) : {pretty_print(s_exp)}")

    # 空列表是原子 (nil)，應該在步驟 1 被處理。如果在這裡遇到，表示有問題。
    if is_empty(s_exp):
        raise Exception("ERROR: 無法對空列表進行求值。")

    operator = s_exp.first()
    operands = s_exp.rest()

    # 步驟 3: 處理特殊形式 (Special Forms)
    # 特殊形式有自己的求值規則，不會先對所有運算元求值。
    if is_symbol(operator):
        if operator == 'quote':
            return operands.first()  # 不對運算元求值
        elif operator == 'if':
            # (if test-expr then-expr else-expr)
            # ... 此處應有 if 的特殊求值邏輯 ...
            pass
        elif operator == 'define':
            # (define symbol value) 或 (define (func params) body)
            # ... 此處應有 define 的特殊邏輯，檢查格式並更新環境 ...
            pass
        # ... 此處應處理其他特殊形式，如 cond, let, and, or ...

    # 步驟 4: 處理標準函式呼叫
    # 首先，對操作符進行求值以獲得一個程序 (procedure)
    procedure = eval_sexp(operator, environment)

    # 檢查結果是否確實是一個可執行的程序
    if not is_procedure(procedure):
        raise Exception(f"ERROR (attempt to apply non-function) : {pretty_print(procedure)}")

    # 接著，對所有的運算元進行求值
    evaluated_operands = []
    for operand in operands:
        evaluated_operands.append(eval_sexp(operand, environment))

    # 最後，將程序應用於已求值的運算元
    return apply_procedure(procedure, evaluated_operands)


def apply_procedure(procedure, evaluated_operands):
    """
    將一個程序應用於一個已求值的運算元列表。
    這是一個偽代碼表示。
    """
    # 檢查參數數量是否正確
    if not procedure.arity_matches(len(evaluated_operands)):
        raise Exception(f"ERROR (incorrect number of arguments) : {procedure.name}")

    # 對於內建函式，檢查參數類型是否正確
    if is_primitive(procedure):
        for operand, expected_type in zip(evaluated_operands, procedure.expected_types):
            if not is_type(operand, expected_type):
                raise Exception(f"ERROR ({procedure.name} with incorrect argument type) : {pretty_print(operand)}")

    # 執行程序
    # 對於使用者定義的函式，這會涉及建立一個新的環境並對函式主體進行求值。
    result = procedure.execute(evaluated_operands)

    # 檢查是否有應回傳而未回傳的情況
    if result is NO_RETURN_VALUE:
        raise Exception(f"ERROR (no return value) : {procedure.name}")

    return result
```

### 其他錯誤訊息

如果你的系統遇到一個 `eval` 演算法中未規範的錯誤，你應該輸出：`ERROR : aaa`，其中 `aaa` 是使用者輸入中「出問題的那個被求值的函式的第一個參數」。

### 關於 Value 和 Binding

Lisp 和 Scheme 堅持一個概念：**沒有「value」！只有「binding」！**

-   Symbol 的 binding 可能是一個 S-expression，也可能是一個內部函式。
-   求值一個「非符號的原子」的結果是那個原子。
-   求值一個 symbol 的結果是那個 symbol 的 binding。
-   求值一個 list 的結果是應用「此 list 第一個參數的求值結果」（應該是一個內部函式）於「此 list 其他參數的求值結果」。

**注意：** 我們不允許改變「內建符號」的 binding！

### 預期參數類型

-   `car`/`cdr`：cons-cell
-   `define`：第一個參數是 symbol 或 symbol list
-   `lambda`：第一個參數是 symbol list
-   `let`：第一個參數是 (symbol S-expression) 的配對列表
-   算術/比較運算子：數字
-   字串函式：字串
-   `set!`：第一個參數是 symbol
-   `display-string`：字串
