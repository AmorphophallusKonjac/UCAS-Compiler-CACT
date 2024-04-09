#include "IRUse.h"
IRUse::IRUse(IRValue *v, IRUser *user) {
    val = v;
    this->user = user;
}
IRUse::IRUse() {
    val = nullptr;
    user = nullptr;
}
IRUse::IRUse(const IRUse &other) {
    this->val = other.val;
    this->user = other.user;
}
IRUse &IRUse::operator=(const IRUse &other) {
    if (this->user != other.user && this->val != other.val) {
        this->user = other.user;
        this->val = other.val;
    }
    return *this;
}
