#ifndef COMPILER_REGISTER_H
#define COMPILER_REGISTER_H

class Register{
private:
    std::string regName;    //记录reg的编号

public:
    explicit Register(std::string name = "") : regName(name) {};

    /*获得reg编号*/
    std::string& getRegName(){ return regName; };//获得reg编号

    void setRegName(std::string& name) { regName.assign(name); };

    /*如果是reg，regName不会是"";如果不是reg，分配在栈上，则是""*/
    bool isreg() { return !regName.empty(); };
};

#endif //COMPILER_REGISTER_H