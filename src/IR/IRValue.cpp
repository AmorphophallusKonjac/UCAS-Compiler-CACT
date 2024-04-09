#include "IRValue.h"

#include <utility>


IRValue::IRValue(IRType *Ty, IRValue::ValueTy vty, std::string name) : name(std::move(name)) {
    ty = Ty;
    vTy = vty;
}
