## Regex

'\b' -> word boundary
'\B' -> non-word boundary

```
KEYWORD:        \bvar\b|\bbegin\b|\bend\b
COMMA:          ,
SEMICOLON:      ;
ASSIGN:         =
PERIOD:         \.\B
NUM:            ([0-9]*)(\.([0-9]+))?
PLUS:           +
MINUS:          -
MUL:            *
DIV:            /
LBRACE => (
RBRACE => )
ID => \b[A-Za-z][A-Za-z0-9]*\b
```

