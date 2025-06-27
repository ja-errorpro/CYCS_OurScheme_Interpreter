# 遞迴下降解析 (Recursive Descent Parsing) 入門

## 掃描器 (Scanner)

### 撰寫 `GetToken()` 的注意事項

-   **從 `stdin` 讀取時，一定要一次讀取一個字元！** 千萬不要一次讀取一個字串。

1.  用一個函式（姑且稱之為 `F1`）來「取得下一個字元」。此函式要負責追蹤所取得字元的行號和列號。

2.  用一個函式（姑且稱之為 `F2`）來「取得下一個非空白字元」。此函式要負責「跳過註解」。（`F2` 當然應該在需要取得「下一個字元」時呼叫 `F1`）。

3.  呼叫 `F2` 以取得下一個非空白字元。此字元便為「下一個 token」的開始。現在檢查此字元，判斷此「下一個 token」是哪種類型，然後呼叫對應的函式（`F3`, `F4`, `F5`）來讀取「下一個 token」的「剩餘部分」。

現在我們已得到此「下一個 token」的全部內容了。

-   **a. 最長匹配原則 (Longest match principle)**
-   **b. 從 `stdin` 或 `vector` 讀取** (注意 C vector 的使用及 PAL 的風格檢查)
-   **c. `PeekToken()` vs. `GetToken()`**

---

## 解析器 (Parser)

**先進行語法檢查（包括詞法/語法錯誤偵測），之後再進行求值（包括變數宣告）。**

### 遞迴下降解析演算法範例

這是一個遞迴下降解析的演算法範例。

**原始語法：**

```
<BooleanExp> ::= <Exp> = <Exp>
<Exp>        ::= <term> | <Exp> + <term> | <Exp> - <term>
<term>       ::= <factor> | <term> * <factor> | <term> / <factor>
<factor>     ::= NUM | IDENT | (<Exp>)
```

**重寫語法（消除左遞迴和左因子後）：**

```
<BooleanExp> ::= <Exp> = <Exp>
<Exp>        ::= <term> {+ <term> | - <term>}
<term>       ::= <factor> {* <factor> | / <factor>}
<factor>     ::= NUM | IDENT | (<Exp>)
```

> **注意：**
> 你應該有一個掃描器（詞法分析器；`GetToken()`），它總是回傳 `EOF` 或 `NUM`, `IDENT`, `EQUAL`, `PLUS` 等。如果它回傳 `NUM` 或 `IDENT`，那麼它也應該回傳對應的值（在 `NUM` 的情況下）或符號（在 `IDENT` 的情況下）。`GetToken()` 的作用是從目前的輸入開始，跳過「空白字元」並取得下一個數字或識別碼（如果有的話）。如果沒有下一個輸入 token，`GetToken()` 回傳一個 `EOF`。
>
> 你也應該有一個 `PeekToken()`，它只「窺視」輸入的 token 但不「取得」它。

### 虛擬碼 (Pseudo-code)

```cpp
// <BooleanExp> ::= <Exp> = <Exp> 
void BooleanExp(var Bool correct)
{
  TOLERANCE = 0.01;
    
  Exp(exp1Correct, exp1Value);
  if (!exp1Correct) {
    correct = false;
    return;
  }
  
  GetToken(tokenType, tokenValue);
  if (tokenType != EQUAL) {
    correct = false;
    return;
  }
  
  Exp(exp2Correct, exp2Value);
  if (!exp2Correct) {
    correct = false;
    return;
  }
  
  GetToken(tokenType, tokenValue);
  if (tokenType != EOF) {
    correct = false;
    return;
  }
    
  // 輸入中確實有 <exp>=<exp>
  if (abs(exp1Value - exp2Value) <= TOLERANCE) {
    correct = true;
  } else {
    correct = false;
  }
} // BooleanExp()
 
// <Exp> ::= <term> {+ <term> | - <term>}
void Exp(var Bool correct, var float value)
{
  Term(term1Correct, term1Value);
  if (!term1Correct) {
    correct = false;
    value = 0.0;
    return;
  }
  
  do {
    PeekToken(tokenType, tokenValue);
    if (tokenType == EOF || (tokenType != PLUS && tokenType != MINUS)) {
      correct = true;
      value = term1Value;
      return;
    }
  
    // 第一個 term 後面有 '+' 或 '-'
    GetToken(tokenType, tokenValue);  // tokenType 是 PLUS 或 MINUS
  
    Term(term2Correct, term2Value);
    if (!term2Correct) {
      correct = false;
      value = 0.0;
      return;
    }
    
    // 第二個 term 也正確
    correct = true;
  
    if (tokenType == PLUS) {
      term1Value = term1Value + term2Value;
    } else {
      term1Value = term1Value - term2Value;
    }
  } while (true);
} // Exp()
 
// <term> ::= <factor> {* <factor> | / <factor>}
void Term(var Bool correct, var float value)
{
  Factor(factor1Correct, factor1Value);
  if (!factor1Correct) {
    correct = false;
    value = 0.0;
    return;
  }
  
  do {
    PeekToken(tokenType, tokenValue);
    if (tokenType == EOF || (tokenType != MULTIPLICATION && tokenType != DIVISION)) {
      correct = true;
      value = factor1Value;
      return;
    }
  
    // 第一個 factor 後面有 '*' 或 '/'
    GetToken(tokenType, tokenValue);  // tokenType 是 MULTIPLICATION 或 DIVISION
  
    Factor(factor2Correct, factor2Value);
    if (!factor2Correct) {
      correct = false;
      value = 0.0;
      return;
    }
    
    // 第二個 factor 也正確
    correct = true;
  
    if (tokenType == MULTIPLICATION) {
      factor1Value = factor1Value * factor2Value;
    } else {
      factor1Value = factor1Value / factor2Value;
    }
  } while (true);
} // Term()
 
// <factor> ::= NUM | IDENT | (<Exp>)
void Factor(var Bool correct, var float value)
{
  GetToken(tokenType, tokenValue);
  
  if (tokenType != NUM && tokenType != IDENT && tokenType != LEFT_PAREN) {
    correct = false;
    value = 0.0;
    return;
  }
    
  if (tokenType == NUM) {
    correct = true;
    value = tokenValue;
    return;
  } else if (tokenType == IDENT) {
    correct = true;
    value = tokenValue; // ????? 這裡需要從符號表查找值
    return;
  } else { // tokenType == LEFT_PAREN
    Exp(expCorrect, expValue);
    
    if (!expCorrect) {
      correct = false;
      value = 0.0;
      return;
    } else { // expCorrect; 但仍需檢查 RIGHT_PAREN
      GetToken(tokenType, tokenValue);
      
      if (tokenType != RIGHT_PAREN) {
        correct = false;
        value = 0.0;
        return;
      } else {
        correct = true;
        value = expValue;
        return;
      } // tokenType == RIGHT_PAREN
    } // expCorrect
  } // tokenType == LEFT_PAREN  
} // Factor()
```
