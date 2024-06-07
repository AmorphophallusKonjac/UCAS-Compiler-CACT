import subprocess
import os
"""
replace the lib_path to (where libcactio.a is located)
replace the compler_path to (where compiler is located)
"""
lib_path ="/home/teacher/zhangshuoming/test_riscv/cactio"
compiler_path="riscv64-unknown-elf-gcc"

def clean():
    for f in os.listdir("."):
        if f.endswith(".exe") or f.endswith(".s") or f.endswith(".c") or f.endswith("_exe.out") or f.endswith("_temp.c"):
            os.remove(f)

# if use riscv64-unknown-elf-gcc to compile, add these declarations
declarations = """#include <stdio.h>
#include <stdbool.h>
void print_int(int x);
void print_double(double x);
void print_float(float x);
void print_bool(bool x);
int get_int();
double get_double();
float get_float();
"""

def test(use_gcc = False):
    # search all file ends with .c, sort them alphabetically
    succ_files = []
    cact_files = [f for f in os.listdir(".") if f.endswith(".cact")]
    print(len(cact_files))
    i = 0
    err = 0
    for cact_file in sorted(cact_files):
        print(i)
        # print(cact_file)
        # read cact_file
        # write to temp file, add declarations
        # for own compiler test, no need for this step
        in_file = cact_file[:-5] + ".in"
        c_file = cact_file[:-5] + ".c"
        s_file = cact_file[:-5] + ".s"
        exe_file = cact_file[:-5] + ".exe"
        out_file = exe_file + "_exe.out"
        out_ref = cact_file[:-5] + ".out"
        # ./compiler cact_file -S -o s_file
        if use_gcc:
            # .c file content is 
            with open(cact_file, 'r') as f:
                c_code = f.read()
            # write to temp file, add declarations
            # for own compiler test, no need for this step
            with open(c_file, 'w') as f:
                f.write(declarations)
                f.write("\n")
                f.write(c_code)
            ret = subprocess.run(["riscv64-unknown-elf-gcc", c_file, "-S", "-o", s_file])
        else:
            ret = subprocess.run([compiler_path, cact_file, "-S", "-o", s_file])
        if ret.returncode != 0:
            print("Failed to compile:", cact_file)
            i += 1
            err += 1
            continue
        # riscv-unknown-elf-gcc s_file -Llib_path -lcactio -o exe_file
        ret = subprocess.run(["riscv64-unknown-elf-gcc", s_file, "-L" + lib_path, "-lcactio", "-o", exe_file])
        if ret.returncode != 0:
            print("Failed to link:", cact_file)
            i += 1
            err += 1
            continue        
        # spike pk ./exe_file
        # if exists in_file, feed to stdin
        if os.path.exists(in_file):
            ret = subprocess.run(["spike", "pk", "./"+exe_file], stdin=open(in_file, 'r'), stdout=subprocess.PIPE)
        else:
            ret = subprocess.run(["spike", "pk", "./"+exe_file], stdout=subprocess.PIPE)
        # save return code to file
        with open(out_file, 'w') as f:
            f.write(ret.stdout.decode())
            f.write(str(ret.returncode))
            f.write("\n")
        # compare with reference
        # use string compare instead of diff
        # read out_file
        with open(out_file, 'r') as f:
            out_content = f.read()
        with open(out_ref, 'r') as f:
            out_ref_content = f.read()
        if out_content != out_ref_content:
            print("Failed to compare:", cact_file)
            i += 1
            err += 1
            continue
        # ret = subprocess.run(["diff", out_file, out_ref])
        # if ret.returncode != 0:
        #     print("Failed to compare:", cact_file)
        #     i += 1
        #     err += 1
        #     continue
        succ_files.append(cact_file)
        i += 1
        

    print("All tests passed:", len(cact_files)-err, "out of", len(cact_files))
    print(succ_files)




if __name__ == "__main__":
    test(True)
    clean()