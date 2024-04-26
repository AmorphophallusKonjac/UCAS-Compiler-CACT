import random

header = '''
#include "stdint.h"
#include "stdio.h"
#include "math.h"

uint32_t next;
uint64_t start_time, start_cycle, start_instnum;

struct ioval {
    uint8_t elemty : 2;
    uint8_t print  : 1;
    union {
        int i;
        float f;
        double d;
    };
};

struct ioval iovals[] = {
'''

libcact = r'''
};

void timer_check(){
    uint64_t time, cycle, instnum; 
    int is_end = ( (sizeof(iovals) / sizeof(struct ioval)) == (next+1) );

    if( (0 == next) || is_end ){
        asm("rdcycle %0\n\t"
            : "=r"(cycle) );
        asm("rdtime %0\n\t"
            : "=r"(time) );
        asm("rdinstret %0\n\t"
            : "=r"(instnum) );

        if( is_end ){
            printf("Execution Success! [%lu]time [%lu]kInsts [%lu]kCycles\n", 
                ((time - start_time) >> 10), ((instnum - start_instnum) >> 10), ((cycle - start_cycle) >> 10) );
        } else {
            start_time    = time;
            start_cycle   = cycle;
            start_instnum = instnum;
        }
    }
}

void dump_error(){
    printf("wrong IO: expect ");
    if( iovals[next].print ){
        printf("print ");
    } else {
        printf("get ");
    }

    if( 0 == iovals[next].elemty ){
        printf("int %d , ", iovals[next].i);
    } else
    if( 1 == iovals[next].elemty ){
        printf("float %f , ", iovals[next].f);
    } else
    if( 2 == iovals[next].elemty ){
        printf("double %f , ", iovals[next].d);
    } else {
        if( iovals[next].i ){
            printf("bool true, ");
        } else {
            printf("bool false, ");
        }
    }
}

void print_int(int i){
    if( (0 == iovals[next].elemty) || (1 == iovals[next].print) || (i == iovals[next].i) ){
        timer_check();
    } else {
        dump_error();
        printf("found print int %d\n", iovals[next].i);
    }
    next ++;
}

void print_float(float fl){
    float diff;
    if( (1 == iovals[next].elemty) || (1 == iovals[next].print) ){
        diff = fl - iovals[next].f;
        if( diff < 0 ){
            diff = -diff;
        }

        if( diff < 1e-2 ){
            timer_check();
        } else {
            printf("float accuracy not met: expected %f, found %f\n", iovals[next].f, fl);
        }
    } else {
        dump_error();
        printf("found print float %f\n", iovals[next].f);
    }
    next ++;
}

void print_double(double d){
    float diff;
    if( (2 == iovals[next].elemty) || (1 == iovals[next].print) ){
        diff = fabs(d - iovals[next].d);
        if( diff < 1e-4 ){
            timer_check();
        } else {
            printf("double accuracy not met: expected %f, found %f\n", iovals[next].d, d);
        }
    } else {
        dump_error();
        printf("found print double %f\n", iovals[next].d);
    }
    next ++;
}

void print_bool(int b){
    if( (3 == iovals[next].elemty) || (1 == iovals[next].print) || (b == iovals[next].i) ){
        timer_check();
    } else {
        dump_error();
        if( b ){
            printf("found print bool true\n");
        } else {
            printf("found print bool false\n");
        }
    }
    next ++;
}

int get_int(){
    int ret = iovals[next].i;
    if( (0 == iovals[next].elemty) && (0 == iovals[next].print) ){
        timer_check();
        next ++;
        return ret;
    } else {
        dump_error();
        printf("found get int %d.\n", ret);
    }
    next ++;
    return 0;
}

float get_float(){
    float ret = iovals[next].f;
    if( (1 == iovals[next].elemty) && (0 == iovals[next].print) ){
        timer_check();
        next ++;
        return ret;
    } else {
        dump_error();
        printf("found get float %f.\n", ret);
    }
    next ++;
    return 0.0f;
}

double get_double(){
    double ret = iovals[next].d;
    if( (2 == iovals[next].elemty) && (0 == iovals[next].print) ){
        timer_check();
        next ++;
        return ret;
    } else {
        dump_error();
        printf("found get double %f.\n", ret);
    }
    next ++;
    return 0.0;   
}
'''

class CACTExample:
    def __init__(self, name):
        self.name = name
        self.glob_rands = []
        self.iovars = ""
    
    def print_int(self, i):
        self.iovars += "\t{ .elemty = 0, .print = 1, .i = %s },\n" % str(i)

    def print_float(self, f):
        self.iovars += "\t{ .elemty = 1, .print = 1, .f = %s },\n" % str(f)

    def print_double(self, d):
        self.iovars += "\t{ .elemty = 2, .print = 1, .d = %f },\n" % d
    
    def print_bool(self, b):
        self.iovars += "\t{ .elemty = 3, .print = 1, .i = %s },\n" % ("1" if b else "0")    
    
    def get_int(self, i):
        self.iovars += "\t{ .elemty = 0, .print = 0, .i = %s },\n" % str(i) 
    
    def get_float(self, f):
        self.iovars += "\t{ .elemty = 1, .print = 0, .f = %s },\n" % str(f)

    def get_double(self, d):
        self.iovars += "\t{ .elemty = 2, .print = 0, .d = %s },\n" % str(d)

    def register_array(self, basety, ident, dims, values, is_const = False):
        if len(values) > 0:  
            sz = 1
            for d in dims:
                sz *= d

            assert( sz == len(values) )
        
        self.glob_rands.append({
            "const"  : is_const,
            "basety" : basety,
            "ident"  : ident,
            "dims"   : dims,
            "values" : values
        })
    
    def make_example(self):
        import os
        os.makedirs( os.getcwd() + "/" + self.name )

        getattr(self, 'mockio_gen')()

        with open(self.name + "/" + self.name + ".cact", 'w') as f:
            for v in self.glob_rands:
                if v["const"]:
                    f.write("const ")
                
                f.write(v["basety"] + "\t" + v["ident"])
                for d in v["dims"]:
                    f.write("[%d]" % d)

                vv = v["values"]
                lv = len(v["values"])
                
                is_int = ("int" == v["basety"])
                is_float = ("float" == v["basety"])
                
                if lv > 0:
                    f.write(" = {")
                    for i in range(lv):
                        if i > 0:
                            f.write(r", ")

                        if is_int:
                            f.write("%d" % vv[i])
                        elif is_float:
                            f.write("%ff" % vv[i])
                        else:
                            f.write("%f" % vv[i])
                    
                    f.write(r"}")
                
                f.write(r";")
                f.write('\n')
            
            f.write(getattr(self, 'srcbody_gen')())
    
        libsrc  = header + self.iovars + libcact
        with open(self.name + "/libcactio.c", 'w', -1 ) as f:
            f.write(libsrc)
        
        os.system("cd %s && riscv64-unknown-elf-gcc -O2 -c libcactio.c -o libcactio.o && riscv64-unknown-elf-ar -r libcactio.a libcactio.o && rm -f libcactio.o" % self.name )


lfs_src = """
int lenLongestFibSubseq(int arrSize){
    int i = 1, ret = 0;
    while( i < arrSize ){
        int j;
        j = i+1;
        while( j < arrSize ){
            if( arr[j] > (2*arr[i]) ){
                break;
            }

            int target, left, right, prev, mid;
            target = arr[j] - arr[i];
            left   = 0; 
            right  = i - 1;
            prev   = -1;

            while( left <= right ){
                mid = (right - left) / 2 + left;
                if( arr[mid] == target ){
                    prev  = mid;
                    break;
                } else
                if( arr[mid] < target ){
                    left  = mid + 1;
                } else {
                    right = mid - 1;
                }
            }

            if( prev >= 0 ){
                dp[i][j] = dp[prev][i] + 1;
                if( dp[i][j] < 3 ){
                    dp[i][j] = 3;
                }
                if( dp[i][j] > ret){
                    ret = dp[i][j];
                }
            }
            j = j + 1;
        }
        i = i + 1;
    }

    return ret;
}

int main(){
    int s, r;
    s = get_int();

    r = lenLongestFibSubseq(s);
    print_int(r);

    return 0;
}
"""


class SearchExample(CACTExample):
    def __init__(self, name, bound):
        super(SearchExample, self).__init__(name)
        d = bound // 3
        self.register_array("int", "dp", [d,d], [] )

        self.vec = random.sample(range(bound), d)
        self.vec.sort()
        self.register_array("int", "arr", [d], self.vec)
    
    def mockio_gen(self):
        res = []
        dp = []
        d = len(self.vec)
        for _ in range( d ):
            res.append(0)
            a = []
            for _ in range( d ):
                a.append(0)
            
            dp.append(a)
        
        for i in range(1, d-1 ):
            for j in range(i+1, d):
                if self.vec[j] > (self.vec[i] << 1):
                    break

                target = self.vec[j] - self.vec[i]
                left   = 0
                right  = i - 1
                prev   = -1

                while left <= right:
                    mid = (right + left) >> 1
                    if self.vec[mid] == target:
                        prev  = mid
                        break
                    elif self.vec[mid] < target:
                        left  = mid + 1
                    else:
                        right = mid - 1
                
                if prev >= 0:
                    dp[i][j] = dp[prev][i] + 1
                    if dp[i][j] < 3:
                        dp[i][j] = 3
                    
                    if dp[i][j] > res[j]:
                        res[j] = dp[i][j]
        
        m = 0
        for i in range(1, d):
            if res[i] > res[i-1]:
                m = i
            else:
                res[i] = res[i-1]
        
        m = m + (d-m) // 2
        super(SearchExample, self).get_int(m)
        super(SearchExample, self).print_int(res[m])

    def srcbody_gen(self):
        return lfs_src


tim_src = '''
void binary_insert(int nums[], int left_low, int right_low, int right_high ){
    int i, j, l, r, m, target;
    i = right_low;
    while( i <= right_high ){
        target = nums[i];
        l = left_low;
        r = i - 1;
        if( target < nums[r] ){
            while( l <= r ){
                m = l + ((r - l) / 2);
                if( target < nums[m] ){
                    r = m - 1;
                } else {
                    l = m + 1;
                }
            }

            j = i - 1;
            while( j >= l ){
                nums[j+1] = nums[j];
                j = j - 1;
            }
            nums[l] = target;
        }
        i = i + 1;
    }
}

void merge_sort( int nums[], int left_low, int right_low, int right_high ){
    int i, j, next;
    i = left_low;
    j = right_low;
    next = 0;

    while( i < right_low && j <= right_high ){
        if( nums[i] <= nums[j] ){
            swap[next] = nums[i];
            i = i + 1;
        } else {
            swap[next] = nums[j];
            j = j + 1;
        }
        next = next + 1;
    }

    while( i < right_low ){
        swap[next] = nums[i];
        i = i + 1;
        next = next + 1;
    }

    while( j <= right_high ){
        swap[next] = nums[j];
        j = j + 1;
        next = next + 1;
    }

    i = left_low;
    while( i <= right_high ){
        nums[i] = swap[i - left_low];
        i = i + 1;
    }
}

void simplified_tim_sort(int nums[], int tail){
    int tmp, merge_min, iter, i, j, stack_top, t;

    merge_min = tail + 1;
    while( merge_min > 64 ){
        merge_min = merge_min / 2;
    }
    merge_min = merge_min + 1;

    iter = -1;
    stack_top = 0;
    while( iter < tail ){
        iter = iter + 1;
        stack[ stack_top ] = iter;

        if( iter < tail ){
            if( nums[iter] <= nums[iter + 1] ){
                while( iter < tail && nums[iter] <= nums[iter + 1] ){
                    iter = iter + 1;
                }
            } else {
                while( iter < tail && nums[iter] > nums[iter + 1] ){
                    iter = iter + 1;
                }
                
                i = stack[ stack_top ];
                j = iter;
                while( i < j ){
                    tmp = nums[i];
                    nums[i] = nums[j];
                    nums[j] = tmp;

                    i = i + 1;
                    j = j - 1;
                }
            }
        }

        if( stack_top > 0 && iter < stack[ stack_top-1 ] + merge_min ){
            binary_insert( nums, stack[ stack_top-1 ], stack[ stack_top ], iter );
            stack_top = stack_top - 1;
        }

        while( stack_top > 0 ){
            if( ((iter-stack[ stack_top ]) * 2) <= (stack[ stack_top ] - stack[ stack_top-1 ]) ){
                break;
            }
            
            merge_sort( nums, stack[ stack_top-1 ], stack[ stack_top ], iter );
            stack_top = stack_top - 1;
        } 

        stack_top = stack_top + 1;
    }

    while( stack_top > 1 ){
        merge_sort( nums, stack[ stack_top-2 ], stack[ stack_top-1 ], iter );
        stack_top = stack_top - 1;
    }
}

int main(){
    int i = 0, tail;
    tail = get_int();
    simplified_tim_sort(array, tail);
    while( i <= tail ) {
        print_int(array[i]);
        i = i + 1;
    }
    return 0;
}
'''

class TimSortExample(CACTExample):
    def __init__(self, name, d):
        super(TimSortExample, self).__init__(name)
        self.register_array("int", "stack", [d+10], [] )
        self.register_array("int", "swap", [d+10], [] )

        bound = (d // 7) * 2
        self.array = []
        for _ in range(d):
            self.array.append( random.randint(-bound, bound) )
        
        self.register_array("int", "array", [d], self.array)

    def mockio_gen(self):
        d = len(self.array)
        r = random.randint( d // 2, d )
        super(TimSortExample, self).get_int(r)

        testarr = self.array[0:(r+1)]
        assert( len(testarr) == (r+1) )

        testarr.sort()
        for i in range(r+1):
            super(TimSortExample, self).print_int(testarr[i])
    
    def srcbody_gen(self):
        return tim_src


mat_src = r'''
void matrix_multiply(int d1, int d2, int d3){
    int i, j, k;
    i = 0;
    while( i < d1 ){
        k = 0;
        while( k < d3 ){
            j = 0;
            while( j < d2 ){
                C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
                j = j + 1;
            }
            k = k + 1;
        }
        i = i + 1;
    }
}

void matrix_stencil(int d1, int d2){
    int i, j;
    i = 1;
    while( i < (d1 - 1) ){
        j = 1;
        while( j < (d2 - 1) ){
            E[i][j] = D[i][j] + D[i][j-1] + D[i][j+1] + D[i-1][j] + D[i+1][j];
            j = j + 1;
        }
        i = i + 1;
    }
}

int main(){
    int x, y, z, s, i, j;
    float f;
    x = get_int();
    y = get_int();
    z = get_int();
    s = get_int();

    matrix_multiply(x, y, z);
    matrix_stencil(x, y);

    i = 0;
    while( i < s ){
        f = C[ J[i] ][ K[i] ];
        print_float( f );
        print_double( E[ J[i] ][ K[i] ] );
        i = i + 1;
    }

    return 0;
}
'''

class MatrixExample(CACTExample):
    def __init__(self, name, d1, d2, d3):
        super(MatrixExample, self).__init__(name)
        self.dims = [d1, d2, d3]
        super(MatrixExample, self).get_int(d1)
        super(MatrixExample, self).get_int(d2)
        super(MatrixExample, self).get_int(d3)

        self.A = []
        self.B = []
        self.D = []
        
        for _ in range(d3):
            for _ in range(d1):
                self.A.append( random.uniform(-10, 10) )  
                
            for _ in range(d2):
                self.B.append( random.uniform(-10, 10) )

        for _ in range(d1):
            for _ in range(d2):
                self.D.append( random.uniform(-10, 10) )  
        
        self.register_array( "float" , "A", [d1, d3], self.A )
        self.register_array( "float" , "B", [d3, d2], self.B )
        self.register_array( "float" , "C", [d1, d2], [] )
        self.register_array( "double", "D", [d1, d2], self.D )
        self.register_array( "double", "E", [d1, d2], [] )
        
        self.samp_num = d2 // 2 if d2 >= d1 else d1 // 2
        super(MatrixExample, self).get_int(self.samp_num)
        self.J = random.sample( range(d1), self.samp_num )
        self.K = random.sample( range(d2), self.samp_num )

        self.register_array( "int", "J", [self.samp_num], self.J )
        self.register_array( "int", "K", [self.samp_num], self.K )
        
    
    def mockio_gen(self):
        for i in range(self.samp_num):
            x = self.J[i]
            y = self.K[i]

            s = 0.0
            for k in range(self.dims[2]):
                s += self.A[ (x*self.dims[2]) + k ] * self.B[ (k*self.dims[1]) + y ]
            
            super(MatrixExample, self).print_float(s)

            s = 0
            if 0 < x < (self.dims[0]-1) and 0 < y < (self.dims[1]-1):
                base = (x*self.dims[1]) + y
                s = self.D[base] + self.D[base-1] + self.D[base+1] + self.D[base-self.dims[1]] + self.D[base+self.dims[1]]
            
            super(MatrixExample, self).print_double(s)
            
    def srcbody_gen(self):
        return mat_src

if __name__ == '__main__':
    MatrixExample("matrix",32, 32, 64).make_example()
    TimSortExample("tim_sort", 5000).make_example()
    SearchExample("fib_subseq", 800).make_example()