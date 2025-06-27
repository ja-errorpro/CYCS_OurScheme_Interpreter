# 2021 春季 PL 專案 (OurScheme) - 專案四

> **截止日 :** 2021年6月27日 (星期日) 午夜前

---

## 專案目標

在這個專案中，你需要擴展你的 OurScheme 直譯器，使其能夠接受以下函式呼叫：`create-error-object`, `error-object?`, `read`, `write`, `display-string`, `newline`, `eval` 和 `set!`。

以下是這些函式的描述。

### `create-error-object` 和 `error-object?`

-   將 `ERROR` 加入到你允許的資料類型列表中，使其不僅有 `INT`, `FLOAT`, `STRING` 等，還有 `ERROR`。
-   `create-error-object` 是你應該加入到系統中的一個函式，它接受一個字串參數，並回傳一個 error-object。
-   `error-object?` 是一個布林值、單一參數的函式，用於測試給定的參數是否為一個 error-object。
-   一個 error-object「包含」一個字串（一個適當措辭的錯誤訊息），並且其「行為」就像一個 string-object。error-object 和 string-object 之間只有兩個區別：
    1.  `error-object?` 對於 error-object 回傳 `#t`，而對於 string-object 回傳 `nil`。
    2.  string-objects 可以被讀入，而 error-objects 不能。Error-objects 只能透過呼叫 `create-error-object` 來明確建立。如下所述，呼叫函式 `read` 也可能導致隱含地建立一個 error-object。

### `read`

-   `read` 是主要的輸入函式。它會嘗試先讀取下一個輸入的 S-expression，然後回傳該 S-expression 的（內部表示）。`read` 不接受任何參數。
-   如果有任何輸入錯誤，`read` 會回傳一個 error-object。這個 error-object 中「包含」的錯誤訊息將與 `ReadSExpression()` 在處理相同輸入時所印出或回傳的訊息相同（但是，不會有「結尾的換行符」）。
-   注意，如果 `read` 遇到的輸入錯誤是檔案結尾，`read` 回傳的 error-object 將包含字串：`ERROR : END-OF-FILE encountered when there should be more input`。

**快速摘要：**

-   `read` 在被呼叫時，會回傳兩種「東西」之一：
    -   一個正常 S-expression 的（內部表示），或
    -   一個「包含」字串（錯誤訊息）的 error-object。
-   一個 error-object 的行為就像一個 string-object。它「包含」一個錯誤訊息。我們可以使用 `error-object?` 來檢查 `read` 是否回傳了一個 error-object。如果 `read` 回傳的是一個 error-object，我們也可以將該 error-object 與 `"ERROR : END-OF-FILE encountered when there should be more input"` 進行比較，以判斷 `read` 遇到的輸入錯誤是否為檔案結尾錯誤。

**範例：**

```scheme
> (define a (read)) (1 2 3)
a defined

> (error-object? a)
nil

> a
( 1
  2
  3
)

> (define a (read))( 1 3 5 . 7 s )
a defined

> (error-object? a)
#t

> (equal? 
   a 
   "ERROR : END-OF-FILE encountered when there should be more input")
nil

> a
"ERROR (unexpected character) : line 1 column 13 character 's'"
```

### `write`, `display-string`, `newline`, `symbol->string`, `number->string`

-   有三個輸出函式：`write`, `display-string`, `newline`。
-   `write` 接受一個參數，該參數應該是一個合法的 S-expression 的（內部表示）。`write` 以與 `PrintSExpression()` 完全相同的方式印出給定的參數。
-   `write` 印出任何 `STRING` 物件時會包含引號，而 `display-string` 印出相同的 `STRING` 物件時則不印出引號。
-   `display-string` 的（唯一）參數只能是 string 與 error-object（因為函式名稱是 `display-string`）。
-   `newline` 只是在輸出中印出一個換行符。
-   至於回傳值，`write` 回傳其參數作為回傳值，`display-string` 也回傳其參數作為回傳值，而 `newline` 只回傳 `nil`。
-   `symbol->string` 接受一個 symbol 並回傳一個僅為字串的 S-expression。
-   `number->string` 接受一個數字並回傳該數字的「字串格式」的 S-expression。例如：`( number->string 23 )` 應該回傳 `"23"`，而 `( number->string 23.0 )` 應該回傳 `"23.000"`。（如果數字不是整數，只需使用 `printf( "%.3f", ...)` 或 `String.format( "%.3f", ...)` 來取得其字串形式）。

**範例：**

```scheme
> (write '(1 2 3))
( 1
  2
  3
)( 1
  2
  3
)

> (write "hi")
"hi""hi"

> (display-string "hi")
hi"hi"

> (display-string (write "hi"))
"hi"hi"hi"

> (write (display-string "hi"))
hi"hi""hi"

> (newline)

nil

> (begin (write "hi") (newline) (display-string "hi") (newline))
"hi"
hi
nil

> (symbol->string 'Hi)
"Hi"

> (string-append (symbol->string 'Hi) " is a good thing")
"Hi is a good thing"

> (string-append (number->string 45) " is a number")
"45 is a number"
```

### `eval`

-   `eval` 接受一個參數，並對給定的 S-expression（的內部表示）進行求值。
-   在內部，`eval` 只是呼叫 `EvalSExpression()`。

**範例：**

```scheme
> '(car '(1 2 3))
( car
  ( quote
    ( 1
      2
      3
    )
  )
)

> (eval '(car '(1 2 3)))
1

> (eval "Hi")
"Hi"

> (eval '(fun '(1 2 3)))
ERROR (unbound symbol) : fun
```

### `set!`

-   除了 `define`, `clean-environment`, `car`, `cdr`, `cons` 等內建函式之外，還要再加一個 `set!` 指令。
-   `set!` 基本上是 `define` 的另一個版本，它接受兩個參數，第一個參數應該是一個 symbol，不進行求值；第二個參數是任何一個 S-expression，需要進行求值。
-   以 `(set! a '(1 2 3))` 為例，這個指令會把 `a` 這個 symbol 的綁定設為 `(1 2 3)` 這個 list。
-   `set!` 與 `define` 的主要不同之處在於：
    1.  `set!` 有回傳值，而且此回傳值是「被設定的那個值」（此思維來自 C 語言）。
    2.  `set!` 可以出現在一個（我們要進行求值的）S-expression 的任何層級（`define` 只能出現在一個 S-expression 的最上層）。

**範例：**

```scheme
> (set! a '(1 2 3))
( 1
  2
  3
)

> a
( 1
  2
  3
)

> (cons (set! a '(4 5))
        a
  )
( ( 4
    5
  )
  4
  5
)

> (define (f aaa) (set! aaa (+ aaa 40)) (set! bbb 100) (+ aaa bbb))
f defined

> (f 10)
150
```

---

## 補充說明

### 之一：到底要做啥？

基本上，你要實作四個目標函式：`read`, `write`, `eval`, `set!`。

但光是 `write` 並不符合輸出的需求，所以還要有 `display-string` 與 `newline`；同時，有時需要在字串之中加入 symbol-name 與 number，所以還需要 `symbol->string` 與 `number->string`。

最後，由於加入了 `error-object` 的概念，所以也要有 `create-error-object` 與 `error-object?` 來配合運作。

算起來總共是要實作十個函式。

`read` 基本上只是呼叫 `ReadSExpression()`，`write` 基本上只是呼叫 `PrintSExpression()`，而 `eval` 基本上也只是呼叫 `EvalSExpression()`。`set!` 則是 `define` 的「孿生兄弟」。

如果你的程式架構配置妥當，你將不需要加太多程式碼。

### 之二：`read` 所應回傳的 error message

-   基本上，（輸入）錯誤的發生，在於 `read` 讀到不該出現的 token（即語法錯誤）。當這狀況發生時，`read` 應該知道此 token（的第一個字元）的行與列，從而產生錯誤訊息。
-   一個比較特殊的狀況是字串還沒「閉合」就已出現換行。此時的錯誤訊息是：`ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y`。
-   一旦發生以上所述錯誤，該行就完全跳過，系統會把使用者的下一行視為之後的輸入。
-   由於是用輸入檔給輸入，所以有可能發生「使用者還沒輸入完一個 S-expression 就沒輸入了」的狀況，此時的錯誤訊息是：`ERROR (no more input) : END-OF-FILE encountered`。

### 之三：「哪個 input 字元是誰讀的？」

當系統在讀取輸入時，有兩種可能：

1.  系統剛印了 `> `，現在要讀入一個 S-expression。讀入後，系統會對其求值並印出結果。
2.  系統（因為求值某段程式碼的關係）正在執行 `read`，所以現在要讀一個 S-expression。此 S-expression 被讀入後，就會被當作是系統對 `read` 求值的結果。

原則上，系統在讀入任一個 S-expression 之後，下一次要讀入 S-expression 時，就是從「上一個 S-expression 的最後一個字元」的下一個字元開始處理。

### 之四：更多關於 error-object

基本上，一個 error-object 內含一個字串 `>>"..."<<`。

-   對這個 error-object 使用 `write`，輸出是 `>>"..."<<`。
-   對這個 error-object 使用 `display-string`，輸出是 `>>...<<`。

若在提示符層級求值一個 S-expression 而得到一個 error-object，此 error-object 是以 `write` 的方式印出來。

**範例：**

```scheme
> (define a (create-error-object "This is an error!"))
a defined

> (error-object? a)
#t

> a
"This is an error!"

> (begin (write a) 5)
"This is an error!"5

> (begin (display-string a) 5)
This is an error!5

> (car a)
ERROR (car with incorrect argument type) : "This is an error!"
```
