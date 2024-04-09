#ifndef COMPILER_IRGLOBALVALUE_H
#define COMPILER_IRGLOBALVALUE_H


#include "IRUser.h"
class IRPointerType;
class IRModule;

class IRGlobalValue : public IRUser {
public:
    enum LinkageTypes {
        ExternalLinkage, // Externally visible function
        LinkOnceLinkage, // Keep one copy of named function when linking (inline)
        WeakLinkage,     // Keep one copy of named function when linking (weak)
        AppendingLinkage,// Special purpose, only applies to global arrays
        InternalLinkage  // Rename collisions when linking (static functions)
    };

protected:
    IRGlobalValue(IRType *Ty, ValueTy vty, LinkageTypes linkage,
                  const std::string &name = "")
        : IRUser(Ty, vty, name), Linkage(linkage), Parent(nullptr) {}
    LinkageTypes Linkage;
    IRModule *Parent;

public:
    ~IRGlobalValue() = default;

    inline IRPointerType *getType() const {
        return (IRPointerType *) IRUser::getType();
    }

    bool hasExternalLinkage() const { return Linkage == ExternalLinkage; }
    bool hasLinkOnceLinkage() const { return Linkage == LinkOnceLinkage; }
    bool hasWeakLinkage() const { return Linkage == WeakLinkage; }
    bool hasAppendingLinkage() const { return Linkage == AppendingLinkage; }
    bool hasInternalLinkage() const { return Linkage == InternalLinkage; }
    void setLinkage(LinkageTypes LT) { Linkage = LT; }
    LinkageTypes getLinkage() const { return Linkage; }

    inline IRModule *getParent() { return Parent; }

    static inline bool classof(const IRGlobalValue *T) { return true; }
    static inline bool classof(const IRValue *V) {
        return V->getValueType() == IRValue::FunctionVal ||
               V->getValueType() == IRValue::GlobalVariableVal;
    }
};


#endif//COMPILER_IRGLOBALVALUE_H
