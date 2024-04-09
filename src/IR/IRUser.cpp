#include "IRUser.h"
IRUser::IRUser(IRType *Ty, IRValue::ValueTy vty, const std::string &name)
    : IRValue(Ty, vty, name) {
}
