#!/bin/bash

#常用宏
COMPILER="./build/compiler"
TEST_DIR="./test/samples_lex_and_syntax/"
TEST_NUM=27

TEST00="00_true_main.cact"
TEST01="01_false_hex_num.cact"
TEST02="02_true_octo.cact"
TEST03="03_false_bracket.cact"
TEST04="04_true_multi_dim_array.cact"
TEST05="05_false_number.cact"
TEST06="06_false_hex_num.cact"
TEST07="07_false_global_exp.cact"
TEST08="08_false_int_num_decl.cact"
TEST09="09_false_val_name.cact"
TEST10="10_false_array_visit.cact"
TEST11="11_false_if_else.cact"
TEST12="12_true_comment.cact"
TEST13="13_false_nested_comment.cact"
TEST14="14_true_sample.cact"
TEST15="15_true_syntax_false_semantic.cact"
TEST16="16_false_if_else.cact"
TEST17="17_true_multi_dim_fparam.cact"
TEST18="18_false_continuous_equation.cact"
TEST19="19_false_val_init.cact"
TEST20="20_false_val_init_op.cact"
TEST21="21_false_token.cact"
TEST22="22_true_func.cact"
TEST23="23_false_val_init_func.cact"
TEST24="24_false_array_size_func.cact"
TEST25="25_false_nested_func_def.cact"
TEST26="26_true_multi_dim_const.cact"


#tests数组
tests=($TEST00 $TEST01 $TEST02 $TEST03 $TEST04 $TEST05 
       $TEST06 $TEST07 $TEST08 $TEST09 $TEST10 $TEST11 
       $TEST12 $TEST13 $TEST14 $TEST15 $TEST16 $TEST17 
       $TEST18 $TEST19 $TEST20 $TEST21 $TEST22 $TEST23 
       $TEST24 $TEST25 $TEST26)

#$START_TEST00

#运行脚本时带参数则运行对应的test(可带多个test)，否则将运行所有的test
if [ ${#} -ge 1 ]; then
    for i in "$@";
    do 
        echo "test $i start..."
        $COMPILER $TEST_DIR${tests[$i]}
        echo ""
    done
else
    for((i=0; i<$TEST_NUM; i++))
    do 
        echo "test $i start..."
        $COMPILER $TEST_DIR${tests[i]}
        echo ""
    done
fi