# Language Grammar

This grammar is written in an LL-friendly EBNF form suitable for a recursive-descent parser.

```text
program         → statement* EOF

statement       → var_decl
                | fn_decl
                | return_stmt
                | print_stmt
                | if_stmt
                | while_stmt
                | expr_stmt

var_decl        → "var" IDENTIFIER "=" expression ";"
fn_decl         → "function" IDENTIFIER "(" params? ")" block
return_stmt     → "return" expression? ";"
print_stmt      → "print" expression ";"
if_stmt         → "if" "(" expression ")" block ("else" block)?
while_stmt      → "while" "(" expression ")" block
expr_stmt       → expression ";"

block           → "{" statement* "}"
params          → IDENTIFIER ("," IDENTIFIER)*

expression      → assignment
assignment      → IDENTIFIER "=" assignment
                | equality

equality        → comparison ( ("==" | "!=") comparison )*
comparison      → term ( ("<" | ">" | "<=" | ">=") term )*
term            → factor ( ("+" | "-") factor )*
factor          → unary ( ("*" | "/") unary )*
unary           → ("!" | "-") unary
                | primary

primary         → NUMBER | STRING | IDENTIFIER
                | "(" expression ")"
                | "true" | "false" | "null"
                | call

call            → primary "(" arguments? ")"
arguments       → expression ("," expression)*
```
