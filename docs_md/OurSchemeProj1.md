# 2021 春季 PL 專案 (OurScheme) - 專案一

> **截止日 :** 6/27(日) 午夜 (23:59)

---

## 1. 核心任務

你要實作一個互動式的 Scheme 直譯器，它會讀取 S-expressions，建立對應的資料結構，然後以「pretty-print」的格式將其印出。

### 執行流程

```cpp
// 'expr' 是一個指向鏈結串列結構的指標，
// 鏈結串列結構是根據使用者輸入建出來的

Print 'Welcome to OurScheme!'
Print '\n'
Print '> '

repeat

  ReadSExp(expr);

  PrintSExp(expr); // 你必須以 "pretty print" 方式印出資料結構

  Print '> '

until 讀到 (exit) 或 EOF

if ( 讀到 EOF )
  Print 'ERROR (no more input) : END-OF-FILE encountered'

Print '\n'
Print 'Thanks for using OurScheme!' // 行末空白可忽略
```

---

## 2. OurScheme 語法

### 終端 (Token)

| 種類        | 描述                                                                                                              |
| ----------- | ----------------------------------------------------------------------------------------------------------------- |
| `LEFT-PAREN`  | `(`                                                                                                               |
| `RIGHT-PAREN` | `)`                                                                                                               |
| `INT`         | 例如 `'123'`, `'+123'`, `'-123'`                                                                                   |
| `STRING`      | `"string's (example)."`<br>(字串不跨行)                                                                          |
| `DOT`         | `.`                                                                                                               |
| `FLOAT`       | `'123.567'`, `'123.'`, `'.567'`, `'+123.4'`, `'-.123'`                                                              |
| `NIL`         | `'nil'` 或 `'#f'`, 但 `'NIL'` 跟 `'nIL'` 不算                                                                       |
| `T`           | `'t'` 或 `'#t'`, 但 `'T'` 跟 `'#T'` 不算                                                                            |
| `QUOTE`       | `'`                                                                                                              |
| `SYMBOL`      | 連續且不為純數字、字串、#t、nil<br>不包含 `(`, `)`, `'`, `"`, `;` 及空白字元的可視字元 <br>符號需區分大小寫      |

### Tokenization 規則

除了在字串內部，token 會被以下分隔符號切割：

-   **(a)** 至少一個空白字元
-   **(b)** `(` (注意: `(` 本身就是一個 token)
-   **(c)** `)` (注意: `)` 本身就是一個 token)
-   **(d)** 單引號字元 (`'`) (注意: 本身就是一個 token)
-   **(e)** 雙引號字元 (`"`) (注意: 它是字串的開頭)
-   **(f)** 分號 `;` (注意: 它是行註解的開頭)

#### Tokenization 範例

| 種類   | 範例                                 |
| ------ | ------------------------------------ |
| `FLOAT`  | `'3.25'`, `'.25'`, `'+.25'`, `'-.25'`, `'+3.'` |
| `INT`    | `'3'`, `'+3'`, `'-3'`                      |
| `SYMBOL` | `'3.25a'`                              |
| `SYMBOL` | `'a.b'`                                |
| `NIL`    | `'#f'`                                 |
| `SYMBOL` | `'#fa'`                                |

### 重要注意事項

-   **浮點數輸出:** 當系統要印出浮點數時，需印出小數點後三位。如 : `3.000`, `-17.200`, `0.125`, `-0.500`。
    -   C 語言請使用 `printf( "%.3f", ...)` 而 Java 使用 `String.format( "%.3f", ...)`。

-   **`.` (點) 字元:** 可能為浮點數或符號的一部份，只有單獨出現時才被視為 `DOT`。

-   **`#` 字元:** 可能為布林值 (`NIL`, `T`) 的一部份，只有當 `#t` 或 `#f` 單獨出現時才被視為布林值。

-   **字串跳脫字元:** OurScheme 的 string 只支援 `\n`, `\t`, `\"`, 與 `\\` 這四個跳脫序列。
    -   如果 `\` 字元之後的字元不是 `n`, `t`, `"`, 或 `\`，此 `\` 字元就無特殊意義。
    -   **合法字串範例:**
        -   `"There is an ENTER HERE>>\nSee?!"`
        -   `"Use '\"' to start and close a string."`
        -   `"OurScheme allows the use of '\\n', '\\t' and  '\\\"' in a string."`
        -   `"Please enter YES\NO below this line >\n"`
        -   `"You need to handle >>\\<<"`
        -   `"You also need to handle >>\\"<<"`
        -   `"When you print '\a', you should get two chars: a backslash and an 'a'"`

### OurScheme EBNF 語法

```ebnf
<S-exp> ::= <ATOM>
            | LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
            | QUOTE <S-exp>

<ATOM>  ::= SYMBOL | INT | FLOAT | STRING
            | NIL | T | LEFT-PAREN RIGHT-PAREN
```

> **錯誤處理:** 一旦讀入 S-exp 失敗，該行包括錯誤 token 在內的所有內容都要被忽略。系統要從下一行開始讀取下一個 S-exp。

### 語言核心概念

1.  **S-expression (S-exp):** OurScheme 的基本程式建構，可能是一個 atom、串列或點對。
2.  **Atom:** 可能是整數、浮點數、字串或符號。
3.  **符號 (Symbol):** 區分大小寫，由非分隔符的可視字元組成。
4.  **常數 (Constant):**
    -   `t` 或 `#t` 代表 "true"。系統只會印出 `#t`。
    -   `nil`, `#f`, 或 `()` 代表 "false"。系統只會印出 `nil`。
5.  **點對 (Dotted Pair):** 格式為 `(SS1 . S2)`，其中 `SS1` 是一個 S-exp 序列。
    -   `(S1 S2 . S3)` 等價於 `(S1 . (S2 . S3))`。
6.  **串列 (List):** 串列 `(S1 S2 ... Sn)` 是 `(S1 . (S2 . (...(Sn . nil)))...)))` 的簡寫。
7.  **引用 (Quote):** `'...` 和 `(quote ...)` 是等價的。
8.  **註解 (Comment):** 行註解起於 `;` 終於換行。

---

## 3. I/O 完整範例

(假設這是交互式介面)

```scheme
Welcome to OurScheme!

> (1 . (2 . (3 . 4)))
( 1
  2
  3
  .
  4
)

> (1 . (2 . (3 . nil)))
( 1
  2
  3
)

> (1 . (2 . (3 . ())))
( 1
  2
  3
)

> (1 . (2 . (3 . #f)))
( 1
  2
  3
)

> 13
13

> 13.
13.000

> +3
3

> +3.
3.000

> -3
-3

> -3.
-3.000

> a
a

> t
#t

> #t
#t

> nil
nil

> ()
nil

> #f
nil

> (t () . (1 2 3))
( #t
  nil
  1
  2
  3
)

> (t . nil . (1 2 3))
ERROR (unexpected token) : ')' expected when token at Line 1 Column 10 is >>.<<

> "There is an ENTER HERE>>\nSee?!"
"There is an ENTER HERE>>
See?!"

> "Use '\"' to start and close a string."
"Use '"' to start and close a string."

> "OurScheme allows the use of '\\n', '\\t' and  '\\\"' in a string."
"OurScheme allows the use of '\n', '\t' and  '\"' in a string."

> "Please enter YES\NO below this line >\n"
"Please enter YES\NO below this line >
"

> "You need to handle >>\\<<"   "You also need to handle >>\\"<<"
"You need to handle >>\\<<"

> "You also need to handle >>"<<"

> ((1 2 3) . (4 . (5 . nil)))
( ( 1
    2
    3
  )
  4
  5
)

> ((1 2 3) . (4 . (5 . ())))
( ( 1
    2
    3
  )
  4
  5
)

> (12.5       . (4 . 5))
( 12.500
  4
  .
  5
)

> (10 12.())   ; 等同於 : ( 10 12. () )
( 10
  12.000
  nil
)

> (10 ().125)   ; 等同於 : ( 10 () .125 )
( 10
  nil
  0.125
)

> ( 1 2.5)
( 1
  2.500
)

> ( 1 2.a)
( 1
  2.a
)

> (1 2.25.5.a)
( 1
  2.25.5.a
)

> (12 (    . 3))
ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 10 is >>.<<

> "Hi"
"Hi"

> "(1 . 2   . 3)"
"(1 . 2   . 3)"

> (((1 . 2)
    .
    ((3 4)
       .
       (5 . 6)
      )
   )
   . (7 . 8)
  )
( ( ( 1
      .
      2
    )
    ( 3
      4
    )
    5
    .
    6
  )
  7
  .
  8
)

> ()
nil

> ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 1 is >>)<<

> (Hi there ! How are you ?)
( Hi
  there
  !
  How
  are
  you
  ?
)

> (Hi there! How are you?)
( Hi
  there!
  How
  are
  you?
)

> (Hi! (How about using . (Lisp (instead of . C?))))
( Hi!
  ( How
    about
    using
    Lisp
    ( instead
      of
      .
      C?
    )
  )
)

> (Hi there) (How are you)
( Hi
  there
)

> ( How
  are
  you
)

> (Hi
           .
           (there  .(     ; 注意，在 '.' 和 '(' 之間可能沒有空格
                          
           How is it going?))
           )
( Hi
  there
  How
  is
  it
  going?
)

> ; 注意：我們剛剛介紹了註解的使用。
  ; ';' 開始一個註解直到行尾。
  ; ReadSExp() 在讀取 S-expression 時應該跳過註解。

(1 2 3) )
( 1
  2
  3
)

> ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 2 is >>)<<

> (1 2
   3) (4 5
( 1
  2
  3
)

>       6)
( 4
  5
  6
)

>        '(Hi
           .
           (there  .(     ; 注意，在 '.' 和 '(' 之間可能沒有空格
                          
           How is it going?))
           )
( quote
  ( Hi
    there
    How
    is
    it
    going?
  )
)

> '(1 2 3) )
( quote
  ( 1
    2
    3
  )
)

>  ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 2 is >>)<<

> '(1 2 3) .25
( quote
  ( 1
    2
    3
  )
)

> 0.250

>    (
   exit  ; 目前為止，你的系統只理解 'exit'

      )  ; 當程式看到 '(exit)' 時會終止

Thanks for using OurScheme!

---

## 4. PAL 自動化測試要求

PAL 系統會使用檔案進行 I/O 測試。

### 範例輸入 (`input1`)

```scheme
1
a   ; 行註解由 ';' 開始，直到行尾
3   ; 你的系統應該能夠跳過所有行註解
(cons 3 5)  ; 一旦印出輸出，它會印一個空行
            ; 系統首先印出 '> ', 然後開始獲取
              ; 用戶的輸入，直到遇到非預期字元
   (    (     ; 或用戶輸入了一個完整的 S-expression
              ;
Hi "!" How    ; 注意應遵守「最長匹配優先」原則
              ; 例如，如果用戶輸入 'How'，
. "are you?"  ; 你應該得到 'How' 而不是 'H' 或 'Ho'

      )   "Fine.  Thank you."

 )    ( 3 .   ; 如果在 S-expression 結束的同一行上，
            ; 用戶又開始了另一個輸入，那麼
              ; 系統也會開始處理第二個輸入，
       .      ; 但會先印出第一個輸入的輸出
( 1 2 ) ( 3 4 ) 5
; 以上是系統如何處理「同一行多個輸入」的範例
  ; 重點是：用戶可能在系統印出 '> ' 之前
  ; 就已經開始輸入了

  (exit     ; 這是跳出用戶-系統對話的方式
            ; 下面，'Thanks' 前面有一個換行符
 )
```

### 預期輸出 (`output1`)

```text
Welcome to OurScheme!

> a

> 3

> ( cons
  3
  5
)

> ( ( Hi
    "!"
    How
    .
    "are you?"
  )
  "Fine.  Thank you."
)

> ERROR (unexpected token) : atom or '(' expected when token at Line 4 Column 8 is >>.<<

> ( 1
  2
)

> ( 3
  4
)

> 5

>
Thanks for using OurScheme!
```

**注意:** PAL 會忽略輸出中每行結尾的空白字元。

---

## 5. 錯誤處理總整理

總共有四個可能會出現的錯誤：

1.  `ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<<`
2.  `ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<<`
3.  `ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y`
4.  `ERROR (no more input) : END-OF-FILE encountered`

### 錯誤範例

> 註：互動式 I/O 無法得到 EOF 錯誤

```scheme
Welcome to OurScheme!

> (1 2 . ; 這是一個註解
) ; 又是註解
ERROR (unexpected token) : atom or '(' expected when token at Line 2 Column 1 is >>)<<

> .
ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 1 is >>.<<

> 

   . 34 56
ERROR (unexpected token) : atom or '(' expected when token at Line 3 Column 4 is >>.<<

> (1 2 . ;
34 56 ) ; 看到了嗎?
ERROR (unexpected token) : ')' expected when token at Line 2 Column 4 is >>56<<

> ( 1 2 (3
4
    )
   .   "Hi, CYCU-ICE
ERROR (no closing quote) : END-OF-LINE encountered at Line 4 Column 21

> (23 56 "How do you do?
ERROR (no closing quote) : END-OF-LINE encountered at Line 1 Column 23

> "
ERROR (no closing quote) : END-OF-LINE encountered at Line 1 Column 2

> (exit 0)
( exit
  0
)

> (exit)

Thanks for using OurScheme!
```

---

## 6. S-expression 輸出規則 (Pretty-Print)

```text
if s 是一個 atom
  輸出 s 後面接上 '\n'
       注意 : 對於 'nil', '()' 和 '#f', 總是印出 'nil'.
       注意 : 對於 '#t' 和 't', 總是印出 '#t'.

else { // s 是 '(' s1 s2 ... sn [ '.' snn ] ')' 的格式

  令 M 為當前這行已經輸出的字元數

  輸出 '(', 輸出一個空格, 輸出 s1
  輸出 M+2 個空格, 輸出 s2
  ...
  輸出 M+2 個空格, 輸出 sn
  if 遇到 '.' 且 sn 後面有 snn
    輸出 M+2 個空格, 輸出 '.', 輸出 '\n'
    輸出 M+2 個空格, 輸出 snn
  輸出 M 個空格, 輸出 ')', 輸出 '\n'
}
```

### Pretty-Print 範例

**輸入:**
```scheme
(((1 . 2) (3 4) 5 . 6) 7 . 8)
```

**輸出:**
```scheme
( ( ( 1
      .
      2
    )
    ( 3
      4
    )
    5
    .
    6
  )
  7
  .
  8
)
```

---

## 7. Q & A

### Q1: `'(1 2 3)` 的輸出應該是什麼？

**A:**
-   **專案一:** 只讀取和印出，不進行求值。`'` 被視為 `(quote ...)`。
    ```scheme
    > '(1 2 3)
    ( quote
      ( 1
        2
        3
      )
    )
    ```
-   **專案二:** 會進行求值，結果是 list 本身。
    ```scheme
    > '(1 2 3)
    ( 1
      2
      3
    )
    ```

### Q2: `> .` 是錯誤嗎？ `abc"abc` 呢？

**A:**
-   `> .` 是錯誤。`.` 在 S-expression 的開頭是一個非預期的 token。
    ```scheme
    ERROR (unexpected token) : atom or '(' expected when token at Line 1 Column 1 is >>.<<
    ```
-   `abc"abc` 會被解析為符號 `abc` 後面跟著一個未閉合的字串。
    ```scheme
    > abc"abc
    abc
    > ERROR (no closing quote) : END-OF-LINE encountered at line 1, column 5
    ```
-   `abc'abc` 會被解析為符號 `abc` 和 `(quote abc)`。
    ```scheme
    > abc'abc
    abc
    > ( quote
      abc
    )
    ```

### Q3: 在一個合法的 S-expression 之後，行號是如何決定的？

**A:** 只有當一個完整的 S-expression 同一行後面出現了「下一個 S-expression 的有效字元」時，這一行才被視為下一次輸入的第一行。如果該行剩下的部分只有空白、tab 或註解，那麼下一次輸入將從新的一行開始計算行號。
