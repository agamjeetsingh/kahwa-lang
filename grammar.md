file = { declaration }
declaration = class-decl | function-decl | variable-decl | typedef-decl ;

# Class
class = { modifier } "class" identifier [":" type { "," type }] "{" { class-member } "}"
class-member = method-decl | variable-decl | constructor-decl

modifier = "static" | "public" | "private" | "protected" | "open" | "final" | "abstract" ;

# Types
type = identifier ["<" type { "," type } ">"]

variable-decl = { modifier } variable-decl
function-decl = { modifier } function-decl
param-list = param { "," param } ;
param = type identifier ;

block = "{" { stmt } "}"

stmt =
variable-decl
| expression-stmt
| return-stmt
| block
| if-stmt
| while-stmt
| for-stmt
| ";"
;

# Misc

typedef = "typedef" identifier type ";"
