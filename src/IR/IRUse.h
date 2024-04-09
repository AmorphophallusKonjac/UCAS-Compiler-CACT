#ifndef COMPILER_IRUSE_H
#define COMPILER_IRUSE_H

class IRValue;
class IRUser;

class IRUse {
public:
    IRValue *val;
    IRUser *user;
    IRUse();
    IRUse(const IRUse &other);
    IRUse &operator=(const IRUse &other);
    IRUse(IRValue *v, IRUser *user);
};

#endif//COMPILER_IRUSE_H
