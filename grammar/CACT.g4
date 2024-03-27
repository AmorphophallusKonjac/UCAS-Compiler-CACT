grammar CACT;

@header {
    #include <vector>
    #include <string>
    #include "../src/CACT.h"
    #include "../src/symbolTable.h"
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
    locals [DataType dataType]
    : lValue
    | number 
    | LeftParen expression RightParen
    ;

unaryExpression
    locals [DataType dataType]
    : primaryExpression
    | unaryOperator unaryExpression
    | Identifier LeftParen (functionRParams)? RightParen
    ;

functionRParams
    : expression (',' expression)*
    ;

unaryOperator
    : '+'
    | '-'
    | '!'
    ;

/*以下优先级是往下递减的，A->B op B，则B的优先级比A的优先级要高 */
//unaryExpression的优先级比较高
multiplicativeExpression
    locals [DataType dataType]
    : unaryExpression (op=('*' | '/' | '%') unaryExpression)*
    ;

additiveExpression
    locals [DataType dataType]
    : multiplicativeExpression (op=('+' | '-') multiplicativeExpression)*
    ;

relationalExpression
    locals [DataType dataType]
    : additiveExpression (op=('<' | '>' | '<=' | '>=') additiveExpression)?
    ;

equalityExpression
    locals [DataType dataType]
    : relationalExpression (op=('==' | '!=') relationalExpression)?
    ;

logicalAndExpression
    locals [DataType dataType]
    : equalityExpression (op='&&' equalityExpression)*
    ;

logicalOrExpression
    locals [DataType dataType]
    : logicalAndExpression (op='||' logicalAndExpression)*
    ;

/*****************************以上*****************************/

expression
    locals [DataType dataType]
    : additiveExpression
    | BooleanConstant
    ;
//expression是最基础的表达式

constantExpression
    locals [DataType dataType]
    : number
    | BooleanConstant
    ;

condition
    locals [DataType dataType]
    : logicalOrExpression
    ;


/*declaration部分 */
declaration
    locals [BlockInfo * thisblockinfo]
    : constantDeclaration
    | variableDeclaration
    ;

constantDeclaration
    locals [DataType dataType]
    : Const basicType constantDefinition (',' constantDefinition)* ';'
    ;

constantDefinition
    locals [DataType dataType,
            std::vector<int> arraySize]
    : Identifier (LeftBracket IntegerConstant RightBracket)* Assign constantInitValue
    ;

constantInitValue
    locals [DataType dataType,
            std::vector<int> arraySize]
    : constantExpression
    | LeftBrace (constantInitValue (',' constantInitValue)*)? RightBrace//这里可能考虑的是对数组进行赋值
    ;                                                                           

variableDeclaration
    locals [DataType dataType]
    : basicType variableDefinition (',' variableDefinition)* ';'
    ;

variableDefinition
    locals [DataType dataType,
            std::vector<int> arraySize]
    : Identifier (LeftBracket IntegerConstant RightBracket)* (Assign constantInitValue)?
    ;
/*declaration部分 */
/*CACT中声明的时候对于const必须显式的进行赋值 */

statement
    : compoundStatement
    | expressionStatement
    | selectionStatement
    | iterationStatement
    | jumpStatement
    ;

compoundStatement//复合语句
    : LeftBrace blockItemList? RightBrace
    ;

blockItemList
    : blockItem+
    ;

blockItem
    : statement
    | declaration
    ;
//一个blockItem要么是一个语句，要么是一个声明

expressionStatement
    : expression? ';'
    | lValue Assign expression ';'
    ;
//表达式语句

lValue
    : Identifier (LeftBracket expression RightBracket)* 
    ;
//lvalue一般是针对数组

selectionStatement
    : If LeftParen condition RightParen statement (Else statement)?
    ;
//暂且不考虑else if

iterationStatement
    : While LeftParen condition RightParen statement
    ;
//暂且不考虑for循环

jumpStatement
    : (
        Continue
        | Break
        | Return expression?
    ) ';'
    ;
//jump

translationUnit
    locals [BlockInfo * thisblockinfo]
    : externalDeclaration+
    ;

externalDeclaration
    locals [BlockInfo * thisblockinfo]
    : functionDefinition
    | declaration
    ;

functionDefinition
    locals [BlockInfo * thisblockinfo ,
            FuncSymbolInfo * thisfuncinfo]
    : functionType Identifier LeftParen functionFParams? RightParen compoundStatement
    ;

functionFParams
    locals [FuncSymbolInfo * thisfuncinfo,
            std::vector < SymbolInfo * > paramList]
    : functionFParam (',' functionFParam)*
    ;

functionFParam
    locals [FuncSymbolInfo * thisfuncinfo,
            SymbolInfo * Fparam]
    : basicType Identifier (LeftBracket IntegerConstant? RightBracket (LeftBracket IntegerConstant? RightBracket)*)?
    ;

number
    locals [DataType dataType]
    : IntegerConstant   # IntegerConstant
    | FloatingConstant  # FloatingConstant
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
