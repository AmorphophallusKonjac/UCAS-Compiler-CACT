import subprocess
import os
# clang .c -S, local non-riscv test
# when testing your own compiler, use the compiler path to replace clang in the c-riscv stage
# and use the riscv-unknown-elf-gcc to get executable xxx
# and use ```spike pk xxx``` to execute
lib_path = "/Users/zhangshuoming/Teaching/CACT/CACT_demo1/cactio/build"

# only used for clang, for our own compiler, no need
declarations = """#include <stdio.h>
#include <stdbool.h>
void print_int(int x);
void print_double(double x);
void print_float(float x);
void print_bool(bool x);
int get_int();
double get_double();
float get_float();"""

def clean():
    for f in os.listdir("."):
        if f.endswith(".exe") or f.endswith(".s") or f.endswith("_exe.out") or f.endswith("_temp.c"):
            os.remove(f)
            
# c->cact
def rename():
    for f in os.listdir("."):
        if f.endswith(".c"):
            os.rename(f, f[:-2] + ".cact")

# cact->c
def rename2():
    for f in os.listdir("."):
        if f.endswith(".cact"):
            os.rename(f, f[:-5] + ".c")

def test():
    # search all file ends with .c, sort them alphabetically
    succ_files = []
    c_files = [f for f in os.listdir(".") if f.endswith(".c")]
    print(len(c_files))
    i = 0
    err = 0
    for c_file in sorted(c_files):
        print(i)
        c_temp = c_file[:-2] + "_temp.c"
        # print(c_file)
        # read c_file
        with open(c_file, 'r') as f:
            c_code = f.read()
        # write to temp file, add declarations
        # for own compiler test, no need for this step
        with open(c_temp, 'w') as f:
            f.write(declarations)
            f.write("\n")
            f.write(c_code)
        in_file = c_file[:-2] + ".in"
        s_file = c_file[:-2] + ".s"
        exe_file = c_file[:-2] + ".exe"
        out_file = exe_file + "_exe.out"
        out_ref = c_file[:-2] + ".out"
        # clang c_file -S -o s_file
        # ./compiler c_file -S -o s_file
        ret = subprocess.run(["clang", c_temp, "-S", "-o", s_file])
        # delete temp file
        os.remove(c_temp)
        if ret.returncode != 0:
            print("Failed to compile:", c_file)
            i += 1
            err += 1
            continue
        # clang s_file -Llib_path -lcactio -o exe_file
        # riscv-unknown-elf-gcc s_file -Llib_path -lcactio -o exe_file
        ret = subprocess.run(["clang", s_file, "-L" + lib_path, "-lcactio", "-o", exe_file])
        if ret.returncode != 0:
            print("Failed to link:", c_file)
            i += 1
            err += 1
            continue
        # print("success")
        # grab return code and stdout
        
        # spike pk ./exe_file
        
        # if exists in_file, feed to stdin
        if os.path.exists(in_file):
            ret = subprocess.run(["./"+exe_file], stdin=open(in_file, 'r'), stdout=subprocess.PIPE)
        else:
            ret = subprocess.run(["./"+exe_file], stdout=subprocess.PIPE)
        # save return code to file
        with open(out_file, 'w') as f:
            f.write(ret.stdout.decode())
            f.write(str(ret.returncode))
            f.write("\n")
        # compare with reference
        ret = subprocess.run(["diff", out_file, out_ref])
        if ret.returncode != 0:
            print("Failed to compare:", c_file)
            i += 1
            err += 1
            continue
        succ_files.append(c_file)
        i += 1
        

    print("All tests passed:", len(c_files)-err, "out of", len(c_files))
    print(succ_files)




if __name__ == "__main__":
    rename()
    # test()
    # clean()