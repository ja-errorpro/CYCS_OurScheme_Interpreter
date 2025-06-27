# 2021 春季 PL 專案 (OurScheme) - 專案三

> **截止日 :** 2021年6月27日 (星期日) 午夜前

---

## 專案目標

在這個專案中，你需要擴展 `EvalSExp()`，使其能夠求值用戶定義的函式。

為了達成這個目標，你必須先擴展 `DEFINE` 的實作，讓用戶可以在呼叫函式之前先定義它。

你還需要允許透過 `let` 結構來建立和使用「區域定義」。

同時，也應該允許透過 `lambda` 來使用「匿名函式」。

換句話說，專案三的主要焦點在於三個特殊的「形式」 (forms)：`let`、`lambda` 和 `define`。

此外，你還必須處理錯誤情況（見下文第二部分）。

---

## 第一部分 - 基本要求

在正式介紹專案三之前，我們必須先釐清一個概念：

**誰是函式？誰不是函式？如果不是函式，那是什麼？**

在 OurScheme 中有十個保留字。以下是這些保留字的列表（根據我們的教科書，一個「保留字」是系統保留使用的詞）：

-   `quote`
-   `and`
-   `or`
-   `begin`
-   `if`
-   `cond`
-   `define`
-   `lambda`
-   `set!`
-   `let`

`let`、`lambda` 和 `define` 是上述保留字中的三個。它們不是函式。每當出現一個保留字時，系統應該檢查相關程式碼片段的語法。

雖然以這十個保留字中任何一個開頭的 S-expressions 實際上是「形式」而不是函式，但其中一些仍然可能回傳值。因此，我們也將這些「形式」稱為「函式形式」(functional forms)。

本學期的 OurScheme 專案不會作類似以下要求（但將來的 OurScheme 專案會）：

```scheme
> define ; 或 'quote' 或 'begin' 或 ... (總共十個情況)
DEFINE format

> (define abc quote) ; 或 'define' 或 'begin' 或 ...
QUOTE format
```

---

### ※ let

`let` 的語法如下：

```scheme
( let ( ... ) ......... )
```

其中：

-   **(a)** `...` 是一個 S-expressions 序列，每個 S-expression 的形式為 `( SYMBOL S-expression )`。
-   **(b)** `.........` 是一個非空的 S-expressions 序列。

換句話說，`let` 至少有兩個參數。

它的第一個參數是一個包含零或多個配對的列表，其中每個配對都必須是 `( SYMBOL S-expression)` 的形式。

`let` 的運作方式如下：

-   `...` 部分定義了帶有綁定的區域符號。
    -   例如，如果 `( ... )` 是：
        ```scheme
        ( ( x 5 ) 
          ( y '(1 2 3))
        )
        ```
    -   那麼，會定義兩個區域符號 `x` 和 `y`，並且 `x` 綁定到原子 `5`，而 `y` 綁定到列表 `(1 2 3)`。

-   `.........` 是正常的 S-expressions。這些 S-expressions 的特點是：
    -   **(i)** 「LET 定義的」區域變數（即 `x` 和 `y`）可以出現在這些 S-expressions 中，系統知道它們的綁定。
    -   **(ii)** `.........` 中最後一個 S-expression 的求值結果被視為整個 LET 表達式的求值結果。

**範例：**

```scheme
> (clean-environment)
environment cleaned

> ( let ( (x 3) (y '(1 2 3))
          )
          (cons 1 '(4 5))       ; 這會被求值；但沒有用
          (cons x (cdr y))      ; 這個的值就是 LET 的值
    )
( 3
  2
  3
)

> x
ERROR (unbound symbol) : x 
```

如果 `let` 的語法有任何錯誤，系統應該印出：`ERROR (let format)`

**錯誤範例：**

```scheme
> (let (car '(1 2 3))  ; let 的第一個參數應該是一個配對列表
                         ; 此外，應該有第二個參數
    )
ERROR (let format)

> (let ((x 3 4)) 5     ; LET 的第一個參數應該是一個配對列表
                         ; '(x 3 4)' 不是一個可接受的配對
    )
ERROR (let format)

> (let ((x 3)
         ) 
         5     
    )
5

> (let ( ( (car '(x y z)) ; LET 的第一個參數應該是一個配對列表
             3              
           )                ; 此外，每個配對的第一個元素必須是一個符號
         )                  
         5
    )
ERROR (let format)
           
> (let ()             ; 第一個參數後面應該至少有一個 S-expression
    )                   
ERROR (let format)

> (let () 5           
    )
5

> (let ( ( x (cons 5) ) ; 問題不在 LET 格式
         )
         ( + x x )
    )
ERROR (incorrect number of arguments) : cons

> (let ((x (1 2 3))) 5)  ; LET 格式正確
ERROR (attempt to apply non-function) : 1
```

### ※ lambda

`lambda` 的語法是：

```scheme
( lambda ( 零個或多個符號 ) 一個或多個 S-expressions )
```

-   一個 lambda 表達式定義一個（無名的）函式。這個 lambda 表達式的求值結果會回傳它所定義的函式。
-   一個 lambda 表達式有兩個或更多參數。
-   第一個參數是一個包含零個或多個符號的列表（這些符號是 lambda 表達式所定義函式的參數）。
-   一個或多個 S-expressions 組成了函式的主體。

**範例：**

```scheme
> (clean-enviornment)
environment cleaned

> ( lambda )
ERROR (lambda format)

> ( lambda x y z )
ERROR (lambda format)

> ( lambda (x) y z       ; 一個 lambda 表達式的求值
                            ; 會產生一個函式的內部表示
     )                      
#<procedure lambda>

> ( lambda () 5 )
#<procedure lambda>

> ( ( lambda () (+ 5 5) (+ 5 6) 
       )
     )
11

> ( ( lambda () (+ 5 5) (+ c 6)
       )
       8
     )
ERROR (incorrect number of arguments) : lambda expression
```

### ※ define

`define` 的語法是：

```scheme
( define SYMBOL S-expression )
```

或

```scheme
( define ( SYMBOL 零個或多個符號 ) 一個或多個 S-expressions )
```

此外，一個 DEFINE 表達式必須出現在最上層（即，它不能是一個「內部」表達式）。

-   第一種 define 表達式定義一個符號的綁定。我們在之前的專案中已經看過如何使用這種 define 表達式。透過適當使用 lambda 表達式，我們也可以使用第一種 define 表達式來定義函式。
-   第二種 define 表達式只用於定義函式。

**範例：**

```scheme
> (clean-environment)
environment cleaned

> ( define a 2 )
a defined

> ( define f ( lambda (x) (+ x x c) ) ; 'f' 的綁定被定義為
     )                                 ; 一個函式的內部表示
f defined

> f
#<procedure lambda>

> (f a)
ERROR (unbound symbol) : c

> ( define c 10 )
c defined

> (f a)
14

> ( define ( g x ) (h x x) )
g defined

> (g 3)
ERROR (unbound symbol) : h

> ( define (h x y) (+ x 20 a))
h defined

> (g c)
32

> (define cadr (lambda (x) (car (cdr x))))
cadr defined

> (cadr '(1 2 3 4))
2
```

---

## 第二部分 - 錯誤處理 (「no return value」錯誤)

現在介紹專案三（與專案四）的一個重要執行期錯誤：`no return-value`。

1.  **Total function vs. Partial function**
    -   我們熟悉的函式都是 **total functions**，即只要所有參數都可接受，函式就保證回傳一個值。
    -   但事實上還有 **partial functions**，即即使所有參數都可接受，函式也未必保證回傳一個值。
    -   例：`(define (F x) (cond ((> x 5) x)))`，所以 `(F 3)` 沒有回傳值。

2.  在 OurScheme 中，我們處理的是 partial functions。

3.  有時，一個函式呼叫不回傳值是可以接受的。
    -   例：`(begin (F 3) 5)`，雖然 `(F 3)` 不回傳值，但這是可以的，因為 `(F 3)` 是否回傳值並不重要。

4.  **在什麼情況下，不回傳值是一個錯誤？**
    -   **(a)** 當一個函式被呼叫時，它所有的實際參數都必須求值為一個綁定。
    -   **(b)** 當一個 `IF` 或 `COND` 被求值且其測試條件被求值時，該測試條件的求值必須產生一個綁定。
    -   **(c)** 當一個 `AND` 或 `OR` 被求值且其條件被求值時，該條件的求值必須產生一個綁定。
    -   **(d)** 當 `DEFINE` 或 `SET!` 或 `LET` 被求值時，「待賦值」的部分必須求值為一個綁定。
    -   **(e)** 當一個函式或函式形式在最上層被求值時，它必須求值為一個綁定。

5.  **當需要回傳值但沒有值回傳時顯示的錯誤訊息**
    -   **(a)** 如果一個函式的實際參數沒有求值為一個綁定：`>>ERROR (unbound parameter) : <實際參數的程式碼><<`
    -   **(b)** 如果 `IF` 或 `COND` 的測試條件沒有求值為一個綁定：`>>ERROR (unbound test-condition) : <測試條件的程式碼><<`
    -   **(c)** 如果 `AND` 或 `OR` 的條件沒有求值為一個綁定：`>>ERROR (unbound condition) : <條件的程式碼><<`
    -   **(d)** 如果 `DEFINE`、`SET!` 或 `LET` 的「待賦值」部分沒有求值為一個綁定：`>>ERROR (no return value) : <「待賦值」的程式碼><<`
    -   **(e)** 如果在最上層求值的函式或函式形式沒有求值為一個綁定：`>>ERROR (no return value) : <在最上層輸入的程式碼><<`
    -   **(f)** 如果 `( ( ... ) ... )` 形式的 S-expression 中，`( ... )` 的求值沒有產生綁定：`>>ERROR (no return value) : <( ... ) 的 pretty print 形式>`

> **例外:** `DEFINE` 和 `CLEAN-ENVIRONMENT`。當 `DEFINE` 或 `CLEAN-ENVIRONMENT` 被求值時，它們不回傳任何綁定。系統會印出 `a defined` 或 `environment cleaned`。

---

## 第三部分 - 處理 DEFINE 和 CLEAN-ENVIRONMENT 資訊輸出的額外函式

你需要再實作兩個函式：

-   `(verbose nil)` vs. `(verbose #t)`
-   `(verbose?)`

`verbose` 模式控制系統在求值 `DEFINE` 或 `CLEAN-ENVIRONMENT` 時是否印出訊息。

**範例：**

```scheme
> (verbose?)   ; verbose 模式是否開啟？
#t

> (define a 5)
a defined

> (clean-environment)
environment cleaned

> (verbose nil) ; 關閉 verbose 模式
nil

> (verbose?)
nil

> (define a 5)

> (clean-environment)

> (verbose 5) ; 重新開啟 verbose 模式
#t
```

---

## 第四部分 - 專案三題目設計

新的（2017）「專案三」的題目安排如下：

-   **(1)~(5)** 無錯誤，第五題的隱藏數據是前四題的隱藏數據的「加總」。
-   **(6)~(10)** 有錯誤，第十題的隱藏數據是前四題的隱藏數據的「加總」。

除了「由簡到繁」之外，有關 `cond`, `if`, `lambda`, `and`, `or`, `let` 的測試數據安排，是依照以下原則：

1.  `define` + `lambda` (基本) - 包含：`COND` `IF` `BEGIN` `AND` `OR`
2.  `define` + `lambda` (複雜) - `COND` `IF` `BEGIN` `AND` `OR` (巢狀呼叫)
3.  (2) + 函式組合 // 函式呼叫函式
4.  (3) + `let` (區域 vs. 全域)
5.  (4) + 巢狀區域 vs. 全域 + (1)~(4) 集大成
6.  (1) + 錯誤測試
7.  (2) + 錯誤測試
8.  (3) + 錯誤測試
9.  (4) + 錯誤測試
10. (5) + 錯誤測試 + (6)~(9) 集大成
11. (5) + 專案二集大成測試 (無錯誤情況)
12. (10) + 專案二集大成測試 (錯誤情況)
