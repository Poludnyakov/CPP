#include<iostream>
#include <deque>
#include <cmath>
#include <string>
#include <algorithm>

class Calculator {
    std::deque<double> d;
public:
    void push(double a) {
        d.push_back(a);
    }
    
    double pop() {
        if (d.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        double a = d.back();
        d.pop_back();
        return a;
    }
    
    size_t size() const {
        return d.size();
    }
    
    void plus() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough operands for plus");
        }
        double a = pop();
        double b = pop();
        push(a + b);
    }
    
    void minus() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough operands for minus");
        }
        double a = pop();
        double b = pop();
        push(b - a);
    }
    
    void mult() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough operands for multiplication");
        }
        double a = pop();
        double b = pop();
        push(a * b);
    }
    
    void div() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough operands for division");
        }
        double a = pop();
        double b = pop();
        if (a == 0) {
            throw std::runtime_error("Error: zero division");
        }
        push(b / a);
    }
    
    void sin() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for sin");
        }
        double a = pop();
        push(std::sin(a));
    }
    
    void cos() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for cos");
        }
        double a = pop();
        push(std::cos(a));
    }
    
    void tg() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for tg");
        }
        double a = pop();
        push(std::tan(a));
    }
    
    void ctg() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for ctg");
        }
        double a = pop();
        double tan_val = std::tan(a);
        if (tan_val == 0) {
            throw std::runtime_error("Error: ctg argument out of domain");
        }
        push(1 / tan_val);
    }
    
    void exp() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for exp");
        }
        double a = pop();
        push(std::exp(a));
    }
    void log() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for log");
        }
        double a = pop();
        if (a <= 0) {
            throw std::runtime_error("Argument <= 0 for log");
        }
        push(std::log(a));
    }
    
    void print() {
        if (d.size() == 0) {
            throw std::runtime_error("Incorrect input, stack is empty");
        } 
        else if (d.size() > 1) {
            throw std::runtime_error("Incorrect input, some data left");
        }
        else {
            std::cout << pop();
        }
    }
    
    void sqrt() {
        if (d.size() < 1) {
            throw std::runtime_error("Incorrect input, not enough operands for sqrt");
        }
        double a = pop();
        if (a < 0) {
            throw std::runtime_error("Argument < 0 for sqrt");
        }
        push(std::sqrt(a));
    }
    
    void atan2() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough arguments for atan2");
        }
        double a = pop();
        double b = pop();
        push(std::atan2(a, b));
    }
    
    void pow() {
        if (d.size() < 2) {
            throw std::runtime_error("Incorrect input, not enough arguments for pow");
        }
        double a = pop();
        double b = pop();
        push(std::pow(b, a));
    }
    
    void median() {
        if (d.size() < 3) {
            throw std::runtime_error("Incorrect input, not enough arguments for median");
        }
        double a = pop();
        double b = pop();
        double c = pop();
        if ((a > b) ^ (a > c)) 
            push(a);
        else if ((b > a) ^ (b > c)) 
            push(b);
        else 
            push(c);
    }
};

int main() {
    std::string input_line;
    std::getline(std::cin, input_line);
    
    Calculator calc;
    
    try {
        size_t i = 0;
        while (i < input_line.size()) {
            while (i < input_line.size() && input_line[i] == ' ') {
                ++i;
            }
            if (i >= input_line.size()) break;
            
            std::string token;
            while (i < input_line.size() && input_line[i] != ' ') {
                token += input_line[i];
                ++i;
            }
            
            if (token.empty()) continue;
            try {
                double number = std::stod(token);
                calc.push(number);
            } catch (const std::invalid_argument&) {
                if (token == "+") {
                    calc.plus();
                }
                else if (token == "-") {
                    calc.minus();
                }
                else if (token == "*") {
                    calc.mult();
                }
                else if (token == "/") {
                    calc.div();
                }
                else if (token == "sin") {
                    calc.sin();
                }
                else if (token == "cos") {
                    calc.cos();
                }
                else if (token == "tg") {
                    calc.tg();
                }
                else if (token == "ctg") {
                    calc.ctg();
                }
                else if (token == "exp") {
                    calc.exp();
                }
                else if (token == "log") {
                    calc.log();
                }
                else if (token == "sqrt") {
                    calc.sqrt();
                }
                else if (token == "atan2") {
                    calc.atan2();
                }
                else if (token == "pow") {
                    calc.pow();
                }
                else if (token == "median") {
                    calc.median();
                }
                else {
                    throw std::runtime_error("Incorrect input: unknown token '" + token + "'");
                }
            }
        }
        
        calc.print();
        
    } catch (const std::exception& e) {
        std::cout << e.what();
        return 1;
    }
    
    return 0;
}
