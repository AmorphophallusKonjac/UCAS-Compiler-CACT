grammar CACT;

@header {
    #include <vector>
    #include <string>
}

/****** parser ******/

compilationUnit
    : translationUnit? EOF
    ;

functionType
    : 'void'
    | basicType
    ;

basicType
    : 'int'
    | 'bool'
    | 'float'
    | 'double'
    ;

primaryExpression
    : lValue
    | number 
    | '(' expression ')'
    ;

unaryExpression
    : primaryExpression
    | unaryOperator unaryExpression
    | Identifier '(' (functionRParams)? ')'
    ;

functionRParams
    : expression (',' expression)*
    ;

unaryOperator
    : '+'
    | '-'
    | '!'
    ;

multiplicativeExpression
    : unaryExpression (op=('*' | '/' | '%') unaryExpression)*
    ;

additiveExpression
    : multiplicativeExpression (op=('+' | '-') multiplicativeExpression)*
    ;

relationalExpression
    : additiveExpression (op=('<' | '>' | '<=' | '>=') additiveExpression)?
    ;

equalityExpression
    : relationalExpression (op=('==' | '!=') relationalExpression)?
    ;

logicalAndExpression
    : equalityExpression (op='&&' equalityExpression)*
    ;

logicalOrExpression
    : logicalAndExpression (op='||' logicalAndExpression)*
    ;

expression
    : additiveExpression 
    | BooleanConstant
    ;

constantExpression
    : number
    | BooleanConstant
    ;

condition
    : logicalOrExpression
    ;

declaration
    : constantDeclaration
    | variableDeclaration
    ;

constantDeclaration
    : 'const' basicType constantDefinition (',' constantDefinition)* ';'
    ;

constantDefinition
    : Identifier ('[' IntegerConstant ']')* '=' constantInitValue
    ;

constantInitValue
    : constantExpression
    | '{' (constantInitValue (',' constantInitValue)*)? '}'
    ;

variableDeclaration
    : basicType variableDefinition (',' variableDefinition)* ';'
    ;

variableDefinition
    : Identifier ('[' IntegerConstant ']')* ('=' constantInitValue)?
    ;

statement
    : compoundStatement
    | expressionStatement
    | selectionStatement
    | iterationStatement
    | jumpStatement
    ;

compoundStatement
    : '{' blockItemList? '}'
    ;

blockItemList
    : blockItem+
    ;

blockItem
    : statement
    | declaration
    ;

expressionStatement
    : expression? ';'
    | lValue '=' expression ';'
    ;

lValue
    : Identifier ('[' expression ']')* 
    ;

selectionStatement
    : If '(' expression ')' statement (Else statement)?
    ;

iterationStatement
    : While '(' expression ')' statement
    ;

jumpStatement
    : (
        Continue
        | Break
        | Return expression?
    ) ';'
    ;

translationUnit
    : externalDeclaration+
    ;

externalDeclaration
    : functionDefinition
    | declaration
    ;

functionDefinition
    : functionType Identifier '(' functionFParams? ')' compoundStatement
    ;

functionFParams
    : functionFParam (',' functionFParam)*
    ;

functionFParam
    : basicType Identifier ('[' IntegerConstant? ']' ('[' IntegerConstant? ']')*)?
    ;

number
    : IntegerConstant
    | FloatingConstant
    ;

/****** lexer  ******/

IntegerConstant
    : DecimalConstant
    | OctalConstant
    | HexadecimalConstant
    ;

FloatingConstant
    : FractionalConstant ExponentPart? FloatingSuffix?
    | DigitSequence ExponentPart FloatingSuffix?
    ;

BooleanConstant
    : True
    | False
    ;

Bool
    : 'bool'
    ;

Break
    : 'break'
    ;

Const
    : 'const'
    ;

Continue
    : 'continue'
    ;

Double
    : 'double'
    ;

Else
    : 'else'
    ;

False
    : 'false'
    ;

Float
    : 'float'
    ;

If
    : 'if'
    ;

Int
    : 'int'
    ;

Return
    : 'return'
    ;

True
    : 'true'
    ;

Void
    : 'void'
    ;

While
    : 'while'
    ;

LeftParen
    : '('
    ;

RightParen
    : ')'
    ;

LeftBracket
    : '['
    ;

RightBracket
    : ']'
    ;

LeftBrace
    : '{'
    ;

RightBrace
    : '}'
    ;

Less
    : '<'
    ;

LessEqual
    : '<='
    ;

Greater
    : '>'
    ;

GreaterEqual
    : '>='
    ;

Plus
    : '+'
    ;

Minus
    : '-'
    ;

Star
    : '*'
    ;

Div
    : '/'
    ;

Mod
    : '%'
    ;

AndAnd
    : '&&'
    ;

OrOr
    : '||'
    ;

Not
    : '!'
    ;

Semi
    : ';'
    ;

Comma
    : ','
    ;

Assign
    : '='
    ;

Equal
    : '=='
    ;

NotEqual
    : '!='
    ;

Dot
    : '.'
    ;

Identifier
    : Nondigit (Nondigit | Digit)*
    ;

fragment Nondigit
    : [a-zA-Z_]
    ;

fragment Digit
    : [0-9]
    ;

fragment DecimalConstant
    : NonzeroDigit Digit*
    ;

fragment OctalConstant
    : '0' OctalDigit*
    ;

fragment HexadecimalConstant
    : HexadecimalPrefix HexadecimalDigit+
    ;

fragment HexadecimalPrefix
    : '0' [xX]
    ;

fragment NonzeroDigit
    : [1-9]
    ;

fragment OctalDigit
    : [0-7]
    ;

fragment HexadecimalDigit
    : [0-9a-fA-F]
    ;

fragment FractionalConstant
    : DigitSequence? '.' DigitSequence
    | DigitSequence '.'
    ;

fragment ExponentPart
    : [eE] Sign? DigitSequence
    ;

fragment Sign
    : [+-]
    ;

fragment DigitSequence
    : Digit+
    ;

fragment FloatingSuffix
    : [fF]
    ;

/****** skips  ******/
Whitespace
    : [ \t]+ -> channel(HIDDEN)
    ;

Newline
    : ('\r' '\n'? | '\n') -> channel(HIDDEN)
    ;

BlockComment
    : '/*' .*? '*/' -> channel(HIDDEN)
    ;

LineComment
    : '//' ~[\r\n]* -> channel(HIDDEN)
    ;