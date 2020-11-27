/*
 * calculator implementation
 * Xi He, Yuntao Li
 * xhe35@jh.edu yli346@jh.edu
 */
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <map>
#include "calc.h"

struct Calc {
};

class CalcImpl : public Calc {
    
public:
    int evalExpr(const char *expr, int *result);

private:
    std::map<std::string, std::string> mVars;

};

// split input to words seperated by space
std::vector<std::string> tokenize(const std::string &expr) {
    std::vector<std::string> vec;
    std::stringstream s(expr);
    std::string tok;
    while (s >> tok) {
        vec.push_back(tok);
    }
    return vec;
}

//return true if token is a valid interger, else return 0
bool isInteger(std::string token) {
  char *p;
  strtol(token.c_str(), &p, 10);
  return (*p) == 0;
}

//return true if token is a valid identifier, else return 0
bool isIdentifier(std::string token) {

    for (char c: token) {
        if(c >= 'a' && c <= 'z') {
            continue;
        } 
        else if(c>='A' && c<='z') {
            continue;
        }
        else {
            return false;
        }
    }
    return true;
}

//return true if token is a valid operator, else return 0
bool isOp(std::string token) {
    return token=="+" || token=="-" || token=="*" || token=="/";
}


//transform string input to meaningful series of operations
int transform(std::vector<std::string> in, std::vector<std::string> &out) {
    std::istringstream is;
    std::stack<std::string> ops;
    for (auto token: in) {
        if(token == "*" || token == "/") {
            if(!ops.empty()) {
                auto last_op = ops.top();
                if(last_op == "*" || last_op == "/") {
                    // do some calculation
                    ops.pop();
                    out.push_back(last_op);
                }
            }
            ops.push(token);
        }
        else if(token == "+" || token == "-") {
            if(!ops.empty()) {
                auto last_op = ops.top();
                if(last_op == "*" || last_op == "/") {
		   // do some calculation
                    ops.pop();
                    out.push_back(last_op);
                }
                else if(last_op == "+" || last_op == "-") {
		   // do some calculation
                    ops.pop();
                    out.push_back(last_op);
                }
            }
            ops.push(token);
        } 
        else if(token == "=") {

            if(!ops.empty()) {
                auto last_op = ops.top();

                if(last_op == "*" || last_op == "/") {
		   // do some calculation
                    ops.pop();
                    out.push_back(last_op);
                }
                else if(last_op == "+" || last_op == "-") {
		   // do some calculation
                    ops.pop();
                    out.push_back(last_op);
                }
            }
            ops.push(token);     
        }
        else {
            if(!isInteger(token) && !isIdentifier(token)) {
	       // invalid operand
                return 0;
            }
            out.push_back(token);
        }
    } 
    std::string op;
    while(!ops.empty()) {
        op = ops.top();
        out.push_back(op);
        ops.pop();
    }
    return 1;
}


int calc(std::string sa, std::string sb, std::string op) {
    int a = std::stoi(sa);
    int b = std::stoi(sb);
    if(op == "+") {
        return a+b;        
    }
    else if(op == "-") {
        return a-b;
    }
    else if(op == "*") {
        return a*b;
    }
    else {
        return a/b;
    }
}


int CalcImpl::evalExpr(const char *expr, int *result) {
    std::string s(expr);
    std::vector<std::string> tokens = tokenize(s);
    std::vector<std::string> out;
    if(!transform(tokens, out)) {
        return 0;
    }
    std::stack<std::string> stack;
    for (auto token: out) {

        if(isInteger(token)) {
            stack.push(token);
        }
        else if(isIdentifier(token)) {

            stack.push(token);

        }
        else {

            if(stack.size() < 2) {
                return 0;
            }

            auto b = stack.top();
            stack.pop();
            auto a = stack.top();
            stack.pop();

            if(isIdentifier(b)) {
                if(mVars.find(b) == mVars.end()) {
                    return 0;
                }

                b = mVars[b];
            }

            if(isOp(token)) {
                if(isIdentifier(a)) {
                    if(mVars.find(a) == mVars.end()) {
                        return 0;
                    }

                    a = mVars[a];
                }

		//invalid integer

                if(!isInteger(b) || !isInteger(a)) {
                    return 0;
                }
		
		//divided by 0
                if(token=="/" && b=="0") {
                    return 0;
                }

                int r = calc(a, b, token);
                stack.push(std::to_string(r));
            }
            else if(token == "=") {
	      //invalid operand
	      if(!isInteger(b) || !isIdentifier(a)) {
                    return 0;
                }
                mVars[a] = b;
                stack.push(b);
            }
            else {
                return 0;
            }
        }
    }

    if(stack.size() != 1) {
        return 0;
    }

    auto sret = stack.top();
    if(isInteger(sret)) {
        *result = std::stoi(sret);
    }
    else if(isIdentifier(sret)) {
        if(mVars.find(sret) == mVars.end()) {
            return 0;
        }
        else {
            *result = std::stoi(mVars[sret]);
        }
    }

    return 1;
}

//The calc_create function creates an instance of the struct Calc data type and returns a pointer to it.
extern "C" struct Calc *calc_create(void) {
    return new CalcImpl();
}

//The calc_destroy function destroys an instance of the struct Calc data type, by deallocating its memory.
extern "C" void calc_destroy(struct Calc *calc) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    delete obj;
}

//The calc_eval function evaluates an expression stored as a C character string in expr, 
//saving the result of evaluating the expression in the variable pointed to by result. 
//If the evaluation succeeds, calc_eval should return 1. If the evaluation fails because expr is invalid, 
//calc_eval should return 0.
extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    return obj->evalExpr(expr, result);
}

