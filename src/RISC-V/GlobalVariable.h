#ifndef COMPILER_GLOBALVARIABLE_H
#define COMPILER_GLOBALVARIABLE_H

#include "User.h"
#include "Environment.h"

class IRGlobalVariable;

namespace RISCV {

    class User;

    class Module;

    class GlobalVariable : public User {
    public:
        GlobalVariable(IRGlobalVariable *irGV, Module *module);

        IRGlobalVariable *getIrGv() const;

        std::string getSectionName();

        void print(std::ostream &O);

    private:
        std::string name;
        Section section;
        unsigned align = 3;
        unsigned size;
        IRGlobalVariable *irGV;

        void printInitVal(std::ostream &O);

        static void printType(std::ostream &O, IRType *type);

        static void printVal(std::ostream &O, IRConstant *val);
    };

} // RISCV

#endif //COMPILER_GLOBALVARIABLE_H
