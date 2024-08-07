# CACT compiler

UCAS 编译原理实验课大作业。一个 CACT 语言编译器（CACT 语言是 C 语言的子集）。前端基于 ANTLR 实现。IR 基于 LLVM IR 做了简化。后端目标为
RISC-V。

优化算法实现了 mem2reg、局部/公共子表达式删除、强度削弱、常数传递、循环不变量外提、k 着色寄存器分配、死代码删除。

实验报告位于 reports 文件夹内

注意：
> CACT 实验难度远远高于 LLVM 实验。如果对于编译没有兴趣，不推荐选择 CACT 实验，这将花费大量的时间

如果下定决心选择 CACT 实验，希望本仓库可以提供一个基础的指南，请随意作为参考，但请勿直接复制！

## build and run

### using antlr4 to generate files from grammar file

```bash
java -jar ../deps/antlr-4.13.1-complete.jar -Dlanguage=Cpp CACT.g4 -visitor -no-listener
```

### compile the cmake project

```bash
mkdir -p build
cd build
cmake ..
make -j
./compiler ../test/samples_lex_and_syntax/00_true_main.cact
```

## 组员

<a href="https://github.com/AmorphophallusKonjac/UCAS-Compiler-CACT/contributors">
  <img src="https://contrib.rocks/image?repo=AmorphophallusKonjac/UCAS-Compiler-CACT" />
</a>
