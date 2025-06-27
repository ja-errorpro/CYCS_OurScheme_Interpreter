# PL OurScheme 專案 - 專案二

> **截止日 :** 6/27(日) 午夜 (23:59)

> **注意:** 專案一的部分測試輸入可能會在專案二中再次出現。例如，專案一的輸入 `(1 2 3)` 在專案二中可能變為 `'(1 2 3)` 或 `(quote (1 2 3))`。

---

## 1. 專案一回顧

在專案一中，你已經完成了以下工作：

-   **掃描器 (Scanner):** 建立了一個掃描器層，負責將用戶輸入的字元流轉換為 Token 流。
-   **解析器 (Parser):** 建立了一個解析器層，負責讀取 Token 流，檢查語法，並建立對應的樹狀資料結構。
-   **Pretty-Printer:** 編寫了一個能以預定格式印出 S-expression 的工具。
-   **錯誤處理:** 實作了語法錯誤的偵測與回報。
-   **系統主迴圈:** 組織了直譯器的基本運作流程。

---

## 2. 專案二新增功能

在專案二中，你需要在現有基礎上擴展你的系統，以實現以下功能：

-   **求值 (Evaluation):** 能夠對所有「原始表達式」(primitive expressions) 進行求值。
-   **變數定義:** 支援 `define` (但暫不支援函數定義與使用)。
-   **條件處理:** 支援 `if` 和 `cond`。
-   **序列處理:** 支援 `begin` 和函數組合 (functional composition)。

### 新的系統主迴圈

你的主程式現在看起來應該像這樣：

```cpp
Print : 'Welcome to OurScheme!'

repeat
  Print : '> '
  ReadSExp( inSExp );

  if no syntax error then
    EvalSExp( inSExp, resultSExp );
    if evaluation error then
      PrintEvaluationError();
    else // 沒有求值錯誤
      PrintSExp( resultSExp );
    end-then // 沒有語法錯誤
  else // 語法錯誤
    PrintSyntaxError();

until user has just entered (exit) or EOF encountered

if ( END-OF-FILE encountered ) // 並且不是「用戶輸入了 '(exit)'」
  Print 'ERROR (no more input) : END-OF-FILE encountered'

Print '\n'
Print : 'Thanks for using OurScheme!'
```

> **重要提示:**
> 錯誤的判斷（怎樣的寫法應該算什麼樣的 error？）是以 `HowToWriteOurScheme.doc` 文件為準。

---

## 3. 需要實作的內建函式 (Primitives)

括號內的數字指的是該函式可接受的參數數量。

### 1. 建構器 (Constructors)
-   `cons` (2)
-   `list` (>= 0)

### 2. 繞過預設求值
-   `quote` (1)
-   `'` (1)

### 3. 符號綁定
-   `define` (2)
    -   一旦一個符號被定義（或「綁定」），用戶可以輸入這個符號，系統將返回其綁定的值。
    -   不允許用戶重新定義系統內建的函式，如 `cons`, `car`, `cdr` 等。

### 4. 部分存取器 (Part Accessors)
-   `car` (1)
-   `cdr` (1)

### 5. 內建判斷式 (Primitive Predicates)
-   `atom?` (1)
-   `pair?` (1)
-   `list?` (1)
-   `null?` (1)
-   `integer?` (1)
-   `real?` (1)
-   `number?` (1) - 在 OurScheme 中, `real?` 等同於 `number?`，但在 Scheme 中則不然（因為有複數）。
-   `string?` (1)
-   `boolean?` (1)
-   `symbol?` (1)

### 6. 基本算術、邏輯與字串操作
-   `+` (>= 2)
-   `-` (>= 2)
-   `*` (>= 2)
-   `/` (>= 2)
-   `not` (1)
-   `and` (>= 2) - *注意：在求值 `and` 或 `or` 時，某些參數表達式可能不會被求值 (短路求值)。使用 Petite Scheme 來了解其意涵。*
-   `or` (>= 2)
-   `>` (>= 2)
-   `>=` (>= 2)
-   `<` (>= 2)
-   `<=` (>= 2)
-   `=` (>= 2)
-   `string-append` (>= 2)
-   `string>?` (>= 2)
-   `string<?` (>= 2)
-   `string=?` (>= 2)

### 7. 等價性測試
-   `eqv?` (2)
-   `equal?` (2)

### 8. 序列與函數組合
-   `begin` (>= 1)

### 9. 條件式
-   `if` (2 or 3) - *注意：在求值 `if` 或 `cond` 時，某些子表達式可能不會被求值（這就是條件表達式的意義）；請使用 Petite Scheme 來檢查。*
-   `cond` (>= 1)

### 10. 環境清理
-   `clean-environment` (0) - 此指令將清空用戶的所有定義。

---

## 4. I/O 完整範例

```scheme
Welcome to OurScheme!

> ; 1. 一個串列（或者說，一個點對）被建構出來。

(cons 3 4)     ; 對兩個物件的操作
( 3
  .
  4
)

> (cons 3
        nil
  )            ; '(3 . nil)' = '(3)'
( 3
)

> (cons 3
        ()
  )            ; 同樣的事情
( 3
)

> (CONS 3 4)      ; Scheme 區分大小寫
ERROR (unbound symbol) : CONS

> (cons hello 4)
ERROR (unbound symbol) : hello

> hello
ERROR (unbound symbol) : hello

> (CONS hello there)
ERROR (unbound symbol) : CONS

> (cons 1 2 3)
ERROR (incorrect number of arguments) : cons

> ; 2. 「繞過」S-exp 的預設解釋

(3 4 5)
ERROR (attempt to apply non-function) : 3

> '(3 4 5)
( 3
  4
  5
)

> (quote (3 (4 5)))
( 3
  ( 4
    5
  )
)

> (cons 3 
        (4321 5))
ERROR (attempt to apply non-function) : 4321

> (cons 3 '(4321 5))
( 3
  4321
  5
)

> (list 3 (4 5))
ERROR (attempt to apply non-function) : 4

> (list 3 '(4 5))
( 3
  ( 4
    5
  )
)

> (list 3 
        '(4 5) 
        6 
        '(7 8))
( 3
  ( 4
    5
  )
  6
  ( 7
    8
  )
)

> ; 2. 為一個物件賦予一個（符號）名稱

; 重新探討 DEFINE 的意義 ("令")

; 基本上，DEFINE 在一個符號和一個 S-expression 之間建立一個（暫時的）綁定

; DEFINE 在一個名稱和一個內部資料結構之間建立綁定

abc
ERROR (unbound symbol) : abc

> (define a 5)   ; "令 a 為 5"; 讓我們把"那個東西"又稱為'a'
a defined

> a              ; 'a' 是某個東西的名稱嗎？
5

> (define x '((3 4) 5))   ; 讓我們把"那個東西"又稱為'x'
x defined

> x              ; 'x' 是某個東西的名稱嗎？
( ( 3
    4
  )
  5
)

> ; 結合 (1), (2) 和 (3)
(define hello '(1 2 . 3))
hello defined

> hello
( 1
  2
  .
  3
)

> (cons hello 
        4
  )       
( ( 1
    2
    .
    3
  )
  .
  4
)

> (cons hello 
        '(4)
  )       
( ( 1
    2
    .
    3
  )
  4
)

> (define hello "CYCU ICE (1 2 3)")
hello defined

> (cons hello 
        '(400 (5000 600) 70)
  )       
( "CYCU ICE (1 2 3)"
  400
  ( 5000
    600
  )
  70
)

> (define there "Number One!")
there defined

> (cons hello there)
( "CYCU ICE (1 2 3)"
  .
  "Number One!"
)

> (define hello '(1 2 . (3)))
hello defined

> (list 3 4)
( 3
  4
)

> ( list hello
         4
  ) 
( ( 1
    2
    3
  )
  4
)

> ; 3. 每當一個函式被呼叫時，它的參數會先被求值。
;    然而，如果一個待求值的串列的第一個符號
;    一開始就沒有綁定到一個函式，求值過程
;    就會停止，並發出適當的錯誤訊息。

> (f 3 b)
ERROR (unbound symbol) : f

> (cons 3 b)
ERROR (unbound symbol) : b

> (cons 3 a)
( 3
  .
  5
)

> (a 3 a)
ERROR (attempt to apply non-function) : 5

> (define a '(3 4))
a defined

> (cons 5 a)
( 5
  3
  4
)

> a
( 3
  4
)

> ; 4. 一個串列（或點對）的不同部分可以被
;    單獨存取

(car '(3 4))    ; 一個點對的「左邊部分」
3

> (car '((3 4) 5)  )
( 3
  4
)

> (car '((3 4) 5 . 6)  )
( 3
  4
)

> (car '((3 4) . 5)  )
( 3
  4
)

> (car a)
3

> (car WarAndPeace!)
ERROR (unbound symbol) : WarAndPeace!

> (cdr '((3 4) 5)  )  ; 一個點對的「右邊部分」
( 5
)

> (cdr '((3 4) "Happy New Year!" . 6)  )
( "Happy New Year!"
  .
  6
)

> (cdr '((3 4) . "Merry Christmas!")  )
"Merry Christmas!"

> (cdr a)
( 4
)

> 
; 透過混合使用 CAR 和 CDR，可以存取串列的不同部分

(car (cdr '((3 4) 5)   ))
5

> (car (cdr '((3 4) 5 . 6)  ))
5

> (car (cdr '((3 4) 5  6  7)   ))
5

> (cdr (cdr '((3 4) 5  6  7)   )
  )     
( 6
  7
)

> (car 3)
ERROR (car with incorrect argument type) : 3

> (car 3 4)
ERROR (incorrect number of arguments) : car

> (car 3 . 5)
ERROR (non-list) : ( car
  3
  .
  5
)

> ; 5. 內建判斷式 (一個判斷式是一個回傳
;      "true" 或 "false" 的函式；依照慣例，一個判斷式的名稱
;      應該以 '?' 作為後綴)

> (atom? 3)
#t

> (atom? '(1 . 2))
nil

> (pair? 3)      ; 其他 Lisp 方言沒有 PAIR；它們有 ATOM
nil

> (pair? '(3 4))
#t
                 
> (pair? '(3 . 4))
#t

> (pair? "Hello, there!")
nil

> (list? 3)
nil

> (list? '(1 2 3))
#t

> (list? '(1 2 . 3))
nil

> (null? ())     ; 這是空串列嗎？
#t

> (null? #f)
#t

> (null? '(3 . 4))
nil

> (integer? 3)
#t

> (integer? +3)
#t

> (integer? 3.4)
nil

> (integer? -.4)
nil

> (real? 3)
#t

> (real? 3.4)
#t

> (real? .5)
#t

> (number? 3) ; 在 OurScheme 中，是實數若且唯若為數字
#t

> (number? 3.4) ; 但在其他 Scheme 方言中，可能存在複數
#t

> (string? "Hi") ; 因此，在其他 Scheme 方言中，一個數字可能不是實數
#t

> (string? +3.4)
nil

> (boolean? #t)
#t

> (boolean? ())
#t

> (boolean? #f)
#t

> (boolean? '(3 . 4))
nil

> (symbol? 'abc)
#t

> (symbol? 3)
nil

> (number? America)
ERROR (unbound symbol) : America

> (define America '(U. S. A.))
America defined

> (symbol? America)
nil

> (pair? America)
#t

> (pair? American)
ERROR (unbound symbol) : American

> (boolean? America)
nil

> (pair? Europe 4)
ERROR (incorrect number of arguments) : pair?

> (pair? America Europe)
ERROR (incorrect number of arguments) : pair?

> (define Europe 'hi)
Europe defined

> (pair? America Europe)
ERROR (incorrect number of arguments) : pair?

> (define a . 5)
ERROR (non-list) : ( define
  a
  .
  5
)

> (define a) ; 參數數量問題
ERROR (DEFINE format) : ( define
  a
)

> (define a 10 20)
ERROR (DEFINE format) : ( define
  a
  10
  20
)

> (define cons 5) ; 嘗試重新定義一個系統內建函式
ERROR (DEFINE format) : ( define
  cons
  5
)

> 
; 6. 基本算術、邏輯和字串操作

> (+ 3 7)
10

> (+ 3 7 10 25)
45

> (- 3 7)
-4

> (- 3 7 10 25)
-39

> (/ 5 2)  ; 整數除法
2

> (/ 5 2.0) ; 浮點數除法；一個浮點數總是印出小數點後 3 位
2.500

> (/ 2 3.0) ; 在 C 中使用 printf( "%.3f", ...)，在 Java 中使用 String.format( "%.3f", ...)
0.667

> (- 3.5 5) 
-1.500

> (* 3 4)
12

> (* 3 "Hi")
ERROR (* with incorrect argument type) : "Hi"

> (* 3)
ERROR (incorrect number of arguments) : *

> (* 3 4 5)
60

> (* 1 2 3 4 5)
120

> (- 1 2 3 4 5)
-13

> (define a 5)
a defined

> (/ 15 a)
3

> (/ 7 a)
1

> (/ 15.0 3)
5.000

> (/ 30 5 0) ; 在執行除法前總是檢查「除以零」
ERROR (division by zero) : /

> (+ 15.125 4)
19.125

> (not #t)
nil

> (> 3 2)
#t

> (> 3.125 2)
#t

> (>= 3.25 2)
#t

> (< 3.125 2)
nil

> (<= 3.125 2)
nil

> (= 2 2)
#t

> (= 2 a)
nil

> (> a a)
nil

> (+ a a a)
15

> (string-append "Hello," " there!")
"Hello, there!"

> (string-append "Hello," " there!" " Wait!")
"Hello, there! Wait!"

> (string>? "az" "aw")
#t

> (string<? "az" "aw")
nil

> (string=? "az" "aw")
nil

> (string=? "az" (string-append "a" "z"))
#t

> (string>? "az" "aw" "ax")
nil

> (string<? "az" "aw" "ax")
nil

> (string=? "az" "aw" "ax")
nil

> (string>? "az" "aw" "atuv")
#t

> (string>? 15 "hi")
ERROR (string>? with incorrect argument type) : 15

> (+ 15 "hi")
ERROR (+ with incorrect argument type) : "hi"

> (string>? "hi" "there" a)
ERROR (string>? with incorrect argument type) : 5

> (string>? "hi" "there" about)
ERROR (unbound symbol) : about

> (string>? "hi" "there" about a)
ERROR (unbound symbol) : about

> ; 7. eqv? 和 equal?

; eqv? 只有在兩個被比較的物件是原子（字串除外）
; 或者兩個被比較的物件「佔用相同的記憶體空間」時
; 才回傳 "true"。

; equal? 則是一般的相等性比較概念。

(eqv? 3 3)
#t

> a
( 3
  4
)

> (eqv? a a)
#t

> (eqv? a '(3 4))
nil

> (equal? a '(3 4))
#t

> (define b a)
b defined

> (eqv? a b)
#t

> (define c '(3 4))
c defined

> (eqv? a c)
nil

> (equal? a c)
#t

> (eqv? '(3 4) '(3 4))
nil

> (eqv? "Hi" "Hi")
nil

> (equal? a a)
#t

> (equal? '(3 4) '(3 4))
#t

> (equal? "Hi" "Hi")
#t

> ; 一些函式組合
(not (pair? 3))
#t

> (define a 5)
a defined

> ( and        ; 'and' 要嘛回傳第一個被求值為 nil 的
    (pair? 3)  ; 表達式的結果，要嘛回傳最後一個
    a          ; 表達式的結果
  )                          
nil

> ( and #t a )
5

> ( or         ; 'or' 要嘛回傳第一個不被求值為 nil 的
    a          ; 表達式的結果，要嘛回傳最後一個
    (null? ()) ; 表達式的結果
  )
5

> ;
  ; 在我們討論序列和函式組合之前，
  ; 先來談談條件式
  ;
  ;             9. 條件式
  ;

(if (> 3 2) 'good 'bad)
good

> (define a 5)
a defined

> (if a 'good 'bad) ; 注意：'if' 可以只接受兩個參數
good

> (if #t 30)
30

> (if #f 20)
ERROR (no return value) : ( if
  nil
  20
)

> (if (not a) 'good 'bad)
bad

> (define a nil)
a defined

> (if a '(1 2) '(3 4))
( 3
  4
)

> (if (not a) '((1 (2) 1) 1) '((3) (4 3)))
( ( 1
    ( 2
    )
    1
  )
  1
)

> (define b 4)
b defined

> ; 'else' 在 OurScheme（或者說 Scheme）中是一個關鍵字（而不是保留字）
  ; 根據我們的教科書（Sebesta），一個關鍵字只有在某些特殊情境下
  ; 才有特殊意義
  ; （翻譯：當這個詞出現在非特殊情境時，它只是一個「普通詞」）
  ; 'else' 只有在作為 'cond' 最後一個條件的第一個元素時才有特殊意義；
  ; 在所有其他情況下，'else' 被視為一個普通符號
  
(cond ((> 3 b) 'bad)
      ((> b 3) 'good)
      (else "What happened?") ; 這個 'else' 有特殊意義；
)                             ; 在這裡它表示「在所有其他情況下」
good

> (cond ((> 3 b) 'bad)
        (else 'good)          ; 這個 'else' 被視為一個普通符號
        (else "What happened"); 這個 'else' 被視為一個關鍵字
  )
ERROR (unbound symbol) : else

> (define else #f)
else defined

> (cond ((> 3 b) 'bad)
        (else 'good)          ; 普通符號 'else' 被綁定到 nil
        (else "What happened"); 這個 'else' 表示「在所有其他情況下」
  )
"What happened"

> (cond ((> 3 b) 'bad)
        ((> b 5) 'bad)
        (else "What happened?")
  )
"What happened?"

> (cond ((> 3 4) 'bad)
        ((> 4 5) 'bad)
  )
ERROR (no return value) : ( cond
  ( ( >
      3
      4
    )
    ( quote
      bad
    )
  )
  ( ( >
      4
      5
    )
    ( quote
      bad
    )
  )
)

> (cond ((> 3 4) 'bad)
        ((> 4 3) 'good)
  )
good

> (cond ((> y 4) 'bad)
        ((> 4 3) 'good)
  )
ERROR (unbound symbol) : y

> (cond)
ERROR (COND format) : ( cond
)

> (cond #t 3)
ERROR (COND format) : ( cond
  #t
  3
)

> (cond (#t 3))
3

> (cond (#f 3))
ERROR (no return value) : ( cond
  ( nil
    3
  )
)

> (cond (#t (3 4)))
ERROR (attempt to apply non-function) : 3

> (cond (#f (3 4)) 5)
ERROR (COND format) : ( cond
  ( nil
    ( 3
      4
    )
  )
  5
)

> (cond (#f (3 4)) (5 6))
6

> (cond (#f (3 4)) ("Hi" (cons 5) . 6))
ERROR (COND format) : ( cond
  ( nil
    ( 3
      4
    )
  )
  ( "Hi"
    ( cons
      5
    )
    .
    6
  )
)

> (cond (#f (3 4)) ("Hi" (cons 5) 6))
ERROR (incorrect number of arguments) : cons

> (cond (#f (3 4)) ("Hi" (cons 5 6) 7))
7

> ;
  ; 8. 序列和函式組合
  ;
  ;    可能比這裡給出的更複雜

(define d 20)
d defined

> d
20

> (if #t 3 5)
3

> (begin
   3  4  5)
5

> (begin
   3 4 d)
20

> (begin
   (+ 3 5)
   (- 4 5)
   (* d d)
  )
400

> (define a 20)
a defined

> (define b 40)
b defined

> (+ d
     ( if (> a b)
          (+ a (* a b))
          (- b (+ a a))
     )
  )
20

> (+ d
     ( if (> a b)
          (+ a (* a b))
          ( begin
            (- b (+ a a))
            70
          )
     )
  )
90

> (if #t (begin 3 4 5) (begin 6 7))
5

> (if #t (3 4 5) (6 7))
ERROR (attempt to apply non-function) : 3

> (if #f (3 4 5) (6 7))
ERROR (attempt to apply non-function) : 6

> (cond ((> 5 3) 'good 'better 'best) (#t 'OK?)   )
best

> ;
  ; 10. clean-environment 清理（用戶定義的）環境
  ;

(clean-environment)
environment-cleaned

> a
ERROR (unbound symbol) : a

> (define a 5)
a defined

> a
5

> (clean-environment)
environment cleaned

> a
ERROR (unbound symbol) : a

> ;
  ; 11. 一個符號的綁定可以是一個函式，它也是一個原子
  ;
  
cons ; 符號 'cons' 的綁定是一個原始名稱為 'cons' 的函式
#<procedure cons>

> (atom? cons)
#t

> (define myCons cons) ; 讓 'myCons' 的綁定成為 'cons' 的綁定
myCons defined

> myCons ; 'myCons' 的綁定是一個原始名稱為 'cons' 的函式
#<procedure cons>

> (define a (myCons car cdr))
a defined

> a
( #<procedure car>
  .
  #<procedure cdr>
)

> (car a)
#<procedure car>

> (cdr a)
#<procedure cdr>

> (define a (list car cdr))
a defined

> (car a)
#<procedure car>

> (cdr a)
( #<procedure cdr>
)

> ((car a) (cons car cdr)) ; 把函式想像成一個像 3 一樣的「值」
#<procedure car>

> (  ((car a) (cons car cdr)) ; 像這樣的測試資料不會出現
     '((10 20) (30 40) . 50)  ; 直到問題 6, 7, 13, 14, 15 和 16
  )
( 10
  20
)

> 
(   
    exit   
           )      

Thanks for using OurScheme!
```

---

## 5. 專案二錯誤總整理

### 專案一的錯誤

以下在專案一中出現的語法錯誤，在專案二中依舊可能出現：

-   `ERROR (unexpected token) : atom or '(' expected when token at Line X Column Y is >>...<<`
-   `ERROR (unexpected token) : ')' expected when token at Line X Column Y is >>...<<`
-   `ERROR (no closing quote) : END-OF-LINE encountered at Line X Column Y`
-   `ERROR (no more input) : END-OF-FILE encountered`

### 專案二新增的求值錯誤

```scheme
> (cons 3 . 5)
ERROR (non-list) : ( cons
  3
  .
  5
)

> (cons 3 (cons 3 . 5))
ERROR (non-list) : ( cons
  3
  .
  5
)

> (cons 3)
ERROR (incorrect number of arguments) : cons

> (exit 0)
ERROR (incorrect number of arguments) : exit

> (car 3)
ERROR (car with incorrect argument type) : 3

> (3 5)
ERROR (attempt to apply non-function) : 3

> (if #f 3)
ERROR (no return value) : ( if
  nil
  3
)

> (cond (#f 3) (#f 4))
ERROR (no return value) : ( cond
  ( nil
    3
  )
  ( nil
    4
  )
)

> noSuchThing
ERROR (unbound symbol) : noSuchThing

> (cons noSuchThing noOtherThingEither)
ERROR (unbound symbol) : noSuchThing

> (/ 30 5 0)
ERROR (division by zero) : /

> (define a) ; 參數數量問題
ERROR (DEFINE format) : ( define
  a
)

> (define a 10 20)
ERROR (DEFINE format) : ( define
  a
  10
  20
)

> (define cons 5) ; 嘗試重新定義一個系統內建函式
ERROR (DEFINE format) : ( define
  cons
  5
)

> ;
  ; >>ERROR (COND format)<< 只會在問題 13, 14
  ; 和 16 中進行測試
  ;

(clean-environment)
environment cleaned

> (cond ((> y 4) 'bad)
        ((> 4 3) 'good)
  )
ERROR (unbound symbol) : y

> (cond)
ERROR (COND format) : ( cond
)

> (cond #t 3)
ERROR (COND format) : ( cond
  #t
  3
)

> (cond (#f 3))
ERROR (no return value) : ( cond
  ( nil
    3
  )
)

> (cond (#t (3 4)))
ERROR (attempt to apply non-function) : 3

> (cond (#f (3 4)) 5)
ERROR (COND format) : ( cond
  ( nil
    ( 3
      4
    )
  )
  5
)

> (cond (#f (3 4)) ("Hi" (cons 5) . 6))
ERROR (COND format) : ( cond
  ( nil
    ( 3
      4
    )
  )
  ( "Hi"
    ( cons
      5
    )
    .
    6
  )
)

> (cond (#f (3 4)) ("Hi" (cons 5) 6))
ERROR (incorrect number of arguments) : cons
```