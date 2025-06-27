# 中原資工 Programming Language(PL) OurScheme Interpreter in C++ 作業檔案

```
聲明：
1. 此 Repository 僅用於存放作業紀錄。
2. 請不要 Copy-Paste，所有程式碼皆已格式化風格與架構，部分程式碼在效能上進行了各種加速優化，同時也有些程式碼會大幅拖慢效能。
  經過系統測試後結果將與一般寫法差距非常明顯，直接 Copy-Paste 後果自負。
```

- 用最簡單的話說，這份作業就是在自幹 scheme 程式語言的直譯器
- 以下是使用 `gemini-2.5-pro` 生成的專案描述，如果要了解更多請自行參考 docs 目錄。

### Project 1: S-Expression 解析與打印

- 實現一個互動式介面，讀取使用者輸入的 S-expression。
- **語法分析**：
  - 支援 ATOM，包含：`SYMBOL`, `INT`, `FLOAT`, `STRING`, `NIL`, `T`。
  - 支援 `LEFT-PAREN`, `RIGHT-PAREN`, `DOT`, `QUOTE`。
  - 處理 S-expression 結構，包括巢狀 list 和 dotted pair。
- **Pretty Print**：將解析後的 S-expression 以美觀、易讀的格式重新印出。
- **錯誤處理**：
  - 處理基本的語法錯誤，例如：未預期的 token、未閉合的引號。
  - 發生錯誤時，能跳過該行並從下一行繼續。
- **註解**：支援以 `;` 開頭的單行註解。

### Project 2: 核心功能與內建函式

- **求值器 (Evaluator)**：擴充解釋器以支援 S-expression 的求值。
- **內建函式 (Primitives)**：
  - **建構器**: `cons`, `list`
  - **求值控制**: `quote` (`'`)
  - **變數綁定**: `define`
  - **存取器**: `car`, `cdr`
  - **型別判斷 (Predicates)**: `atom?`, `pair?`, `list?`, `null?`, `integer?`, `real?`, `number?`, `string?`, `boolean?`, `symbol?`
  - **數值運算**: `+`, `-`, `*`, `/`
  - **邏輯運算**: `not`, `and`, `or`
  - **比較運算**: `>`, `>=`, `<`, `<=`, `=`
  - **字串處理**: `string-append`, `string>?`, `string<?`, `string=?`
  - **等價判斷**: `eqv?`, `equal?`
- **流程控制**:
  - **順序執行**: `begin`
  - **條件判斷**: `if`, `cond`
- **環境管理**: `clean-environment` 用於清除使用者定義的變數。
- **錯誤處理**：擴充錯誤處理機制，涵蓋求值階段的錯誤，如未綁定符號、參數數量錯誤、型別錯誤等。

### Project 3: 自訂函式與環境作用域

- **自訂函式**:
  - 支援使用 `(define (func-name params) ...)` 的語法來定義函式。
  - 支援 `lambda` 匿名函式。
- **環境與作用域**:
  - 實現 `let` 來建立局部變數綁定。
  - 正確處理全域變數與區域變數（函式參數、`let` 變數）的作用域。
- **錯誤處理**:
  - 新增對「無回傳值 (no return value)」錯誤的處理。
- **輸出控制**:
  - 新增 `verbose` 和 `verbose?` 來控制 `define` 和 `clean-environment` 是否顯示提示訊息。

### Project 4: I/O、可變狀態與元編程

- **I/O 操作**:
  - `read`: 從輸入讀取一個 S-expression。
  - `write`: 以 pretty-print 格式印出 S-expression。
  - `display-string`: 印出字串（不包含雙引號）。
  - `newline`: 印出換行符。
- **元編程 (Metaprogramming)**:
  - `eval`: 對一個 S-expression 求值。
- **可變狀態 (Mutable State)**:
  - `set!`: 修改一個已存在變數的綁定，或是同 `define` 一樣建立新的變數綁定。
- **錯誤物件**:
  - `create-error-object`: 建立一個錯誤物件。
  - `error-object?`: 判斷一個物件是否為錯誤物件。
- **型別轉換**:
  - `symbol->string`: 將 symbol 轉為 string。
  - `number->string`: 將 number 轉為 string。
