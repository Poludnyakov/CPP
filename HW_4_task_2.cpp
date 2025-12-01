#include <iostream>
#include <string>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <vector>

class BCD {
public:
    BCD(std::string str = "0") {
        if (str.empty()) str = "0";
        
        // Обработка знака
        if (str[0] == '-') {
            is_negative = true;
            str = str.substr(1);
        }
        
        // Замена запятой на точку
        size_t comma_pos = str.find(',');
        if (comma_pos != std::string::npos) {
            str[comma_pos] = '.';
        }
        
        // Разделение на целую и дробную части
        size_t dot_pos = str.find('.');
        if (dot_pos == std::string::npos) {
            // Нет дробной части
            integer_part = std::stoul(str);
            fractional_part = "";
        } else {
            integer_part = std::stoul(str.substr(0, dot_pos));
            fractional_part = str.substr(dot_pos + 1);
        }
        
        assert(std::to_string(integer_part).length() <= 10);
    }

    BCD(int l, std::string r = "") : integer_part(std::abs(l)), fractional_part(r), is_negative(l < 0) {
        assert(std::to_string(integer_part).length() <= 10);
    }
    
    BCD() : integer_part(0), fractional_part(""), is_negative(false) {}
    
    // Конструктор копирования
    BCD(const BCD& other) = default;
    
    // Move конструктор
    BCD(BCD&& other) noexcept = default;
    
    // Оператор присваивания
    BCD& operator=(const BCD& other) = default;
    
    // Move присваивание
    BCD& operator=(BCD&& other) noexcept = default;
    
    int ceil() const {
        if (fractional_part.empty() || is_zero_fractional()) {
            return is_negative ? -static_cast<int>(integer_part) : static_cast<int>(integer_part);
        }
        return is_negative ? -static_cast<int>(integer_part) : static_cast<int>(integer_part) + 1;
    }
    
    int floor() const {
        if (fractional_part.empty() || is_zero_fractional()) {
            return is_negative ? -static_cast<int>(integer_part) : static_cast<int>(integer_part);
        }
        return is_negative ? -static_cast<int>(integer_part) - 1 : static_cast<int>(integer_part);
    }
    
    int round() const {
        if (fractional_part.empty() || is_zero_fractional()) {
            return is_negative ? -static_cast<int>(integer_part) : static_cast<int>(integer_part);
        }
        
        int first_digit = fractional_part[0] - '0';
        if (first_digit >= 5) {
            return is_negative ? -static_cast<int>(integer_part) - 1 : static_cast<int>(integer_part) + 1;
        }
        return is_negative ? -static_cast<int>(integer_part) : static_cast<int>(integer_part);
    }
    
    BCD operator+(const BCD& other) const {
        if (is_negative == other.is_negative) {
            return add_same_sign(*this, other);
        } else {
            return add_different_sign(*this, other);
        }
    }
    
    BCD& operator+=(const BCD& other) {
        *this = *this + other;
        return *this;
    }
    
    BCD operator-(const BCD& other) const {
        return *this + (-other);
    }
    
    BCD& operator-=(const BCD& other) {
        *this = *this - other;
        return *this;
    }
    
    BCD operator*(const BCD& other) const {
        if (is_zero() || other.is_zero()) {
            return BCD(0, "");
        }
        
        // Определяем знак результата
        bool result_negative = (is_negative != other.is_negative);
        
        // Преобразуем в строки без учета знака
        std::string a_str = std::to_string(integer_part) + fractional_part;
        std::string b_str = std::to_string(other.integer_part) + other.fractional_part;
        
        // Удаляем ведущие нули, но сохраняем хотя бы одну цифру
        a_str = a_str.erase(0, std::min(a_str.find_first_not_of('0'), a_str.size() - 1));
        b_str = b_str.erase(0, std::min(b_str.find_first_not_of('0'), b_str.size() - 1));
        
        int total_decimal_places = get_precision() + other.get_precision();
        
        // Выполняем умножение
        int n = a_str.length();
        int m = b_str.length();
        std::vector<int> result(n + m, 0);
        
        for (int i = n - 1; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                int mul = (a_str[i] - '0') * (b_str[j] - '0');
                int sum = mul + result[i + j + 1];
                
                result[i + j + 1] = sum % 10;
                result[i + j] += sum / 10;
            }
        }
        
        // Преобразуем в строку
        std::string result_str;
        for (int num : result) {
            if (!(result_str.empty() && num == 0)) {
                result_str.push_back(num + '0');
            }
        }
        
        if (result_str.empty()) {
            return BCD(0, "");
        }
        
        // Разделяем на целую и дробную части
        std::string integer_str, fractional_str;
        if (total_decimal_places >= static_cast<int>(result_str.length())) {
            integer_str = "0";
            fractional_str = std::string(total_decimal_places - result_str.length(), '0') + result_str;
        } else {
            integer_str = result_str.substr(0, result_str.length() - total_decimal_places);
            fractional_str = result_str.substr(result_str.length() - total_decimal_places);
        }
        
        int result_int = std::stoi(integer_str);
        
        // Вычисляем новую точность согласно формуле из задания
        int M = std::max(integer_part, other.integer_part);
        int log_term = (M == 0) ? 0 : static_cast<int>(std::log10(M)) + 1;
        int new_precision = std::min(get_precision(), other.get_precision()) - (1 + log_term);
        if (new_precision < 0) new_precision = 0;
        
        // Обрезаем дробную часть до новой точности
        if (new_precision < static_cast<int>(fractional_str.length())) {
            fractional_str = fractional_str.substr(0, new_precision);
        }
        
        BCD result_bcd(result_negative ? -result_int : result_int, fractional_str);
        return result_bcd;
    }
    
    BCD operator*=(const BCD& other) {
        *this = *this * other;
        return *this;
    }
    
    BCD operator+() const {
        return *this;
    }
    
    BCD operator-() const {
        BCD result = *this;
        if (!result.is_zero()) {
            result.is_negative = !result.is_negative;
        }
        return result;
    }
    
    // Операторы сравнения
    bool operator==(const BCD& other) const {
        if (is_negative != other.is_negative) return false;
        if (integer_part != other.integer_part) return false;
        
        std::string a_frac = fractional_part;
        std::string b_frac = other.fractional_part;
        align_fractions(a_frac, b_frac);
        return a_frac == b_frac;
    }
    
    bool operator!=(const BCD& other) const {
        return !(*this == other);
    }
    
    bool operator<(const BCD& other) const {
        if (is_negative != other.is_negative) {
            return is_negative;
        }
        
        if (integer_part != other.integer_part) {
            return is_negative ? integer_part > other.integer_part : integer_part < other.integer_part;
        }
        
        std::string a_frac = fractional_part;
        std::string b_frac = other.fractional_part;
        align_fractions(a_frac, b_frac);
        
        return is_negative ? a_frac > b_frac : a_frac < b_frac;
    }
    
    bool operator>(const BCD& other) const {
        return other < *this;
    }
    
    bool operator<=(const BCD& other) const {
        return !(other < *this);
    }
    
    bool operator>=(const BCD& other) const {
        return !(*this < other);
    }
    
    int get_precision() const {
        return fractional_part.length();
    }
    
    bool is_zero() const {
        return integer_part == 0 && is_zero_fractional();
    }
    
    // Метод для установки точности (добавлен)
    void set_precision(int precision) {
        if (precision < 0) precision = 0;
        if (fractional_part.length() < precision) {
            fractional_part += std::string(precision - fractional_part.length(), '0');
        } else if (fractional_part.length() > precision) {
            fractional_part = fractional_part.substr(0, precision);
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const BCD& bcd) {
        if (bcd.is_negative && !bcd.is_zero()) {
            os << '-';
        }
        os << bcd.integer_part;
        if (!bcd.fractional_part.empty()) {
            os << '.' << bcd.fractional_part;
        }
        return os;
    }
    unsigned integer_part = 0;
    std::string fractional_part = "";
    bool is_negative = false;
private:
    
    bool is_zero_fractional() const {
        for (char c : fractional_part) {
            if (c != '0') return false;
        }
        return true;
    }
    
    static void align_fractions(std::string& a, std::string& b) {
        size_t max_len = std::max(a.length(), b.length());
        a.append(max_len - a.length(), '0');
        b.append(max_len - b.length(), '0');
    }
    
    static BCD add_same_sign(const BCD& a, const BCD& b) {
        std::string a_frac = a.fractional_part;
        std::string b_frac = b.fractional_part;
        align_fractions(a_frac, b_frac);
        
        std::string result_frac;
        int carry = 0;
        
        // Складываем дробные части
        for (int i = a_frac.length() - 1; i >= 0; i--) {
            int sum = (a_frac[i] - '0') + (b_frac[i] - '0') + carry;
            result_frac.push_back((sum % 10) + '0');
            carry = sum / 10;
        }
        std::reverse(result_frac.begin(), result_frac.end());
        
        // Складываем целые части
        unsigned result_int = a.integer_part + b.integer_part + carry;
        
        // Вычисляем новую точность (min - 1)
        int new_precision = std::min(a.get_precision(), b.get_precision()) - 1;
        if (new_precision < 0) new_precision = 0;
        
        // Обрезаем дробную часть до новой точности
        if (new_precision < static_cast<int>(result_frac.length())) {
            result_frac = result_frac.substr(0, new_precision);
        }
        
        BCD result(0, result_frac);
        result.integer_part = result_int;
        result.is_negative = a.is_negative;
        return result;
    }
    
    static BCD add_different_sign(const BCD& a, const BCD& b) {
        // Определяем число с большим абсолютным значением
        BCD abs_a = a;
        abs_a.is_negative = false;
        BCD abs_b = b;
        abs_b.is_negative = false;
        
        if (abs_a == abs_b) {
            return BCD(0, "");
        }
        
        bool result_negative;
        const BCD* larger;
        const BCD* smaller;
        
        if (abs_a > abs_b) {
            larger = &a;
            smaller = &b;
            result_negative = a.is_negative;
        } else {
            larger = &b;
            smaller = &a;
            result_negative = b.is_negative;
        }
        
        std::string larger_frac = larger->fractional_part;
        std::string smaller_frac = smaller->fractional_part;
        align_fractions(larger_frac, smaller_frac);
        
        std::string result_frac;
        int borrow = 0;
        
        // Вычитаем дробные части
        for (int i = larger_frac.length() - 1; i >= 0; i--) {
            int diff = (larger_frac[i] - '0') - (smaller_frac[i] - '0') - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result_frac.push_back(diff + '0');
        }
        std::reverse(result_frac.begin(), result_frac.end());
        
        // Вычитаем целые части
        unsigned result_int = larger->integer_part - smaller->integer_part - borrow;
        
        // Вычисляем новую точность (min - 1)
        int new_precision = std::min(a.get_precision(), b.get_precision()) - 1;
        if (new_precision < 0) new_precision = 0;
        
        // Обрезаем дробную часть до новой точности
        if (new_precision < static_cast<int>(result_frac.length())) {
            result_frac = result_frac.substr(0, new_precision);
        }
        
        BCD result(0, result_frac);
        result.integer_part = result_int;
        result.is_negative = result_negative;
        return result;
    }
};

BCD calculateReciprocal(long long N, int precision) {
    assert(N != 0);
    
    if (N == 1) return BCD(1, std::string(precision, '0'));
    if (N == -1) return BCD(-1, std::string(precision, '0'));
    
    bool result_negative = N < 0;
    N = std::abs(N);
    
    std::string fractional;
    long long remainder = 1;
    
    for (int i = 0; i < precision; i++) {
        if (remainder == 0) {
            fractional += '0';
            continue;
        }
        
        remainder *= 10;
        int digit = remainder / N;
        remainder %= N;
        fractional += std::to_string(digit);
    }
    
    BCD result(0, fractional);
    result.is_negative = result_negative;
    return result;
}

// Вспомогательная функция для вычисления факториала с использованием BCD
BCD factorial_bcd(int n) {
    if (n <= 1) return BCD(1);
    BCD result(1);
    for (int i = 2; i <= n; i++) {
        result = result * BCD(i);
    }
    return result;
}

int main() {
    BCD a(-10, "9988754");
    BCD b(1, "12300000001");
    std::cout <<a - b << std::endl;
    BCD test("1000,1234");
    std::cout<<test << std::endl;
    BCD a1(-10, "9988754");
    BCD b1(-1, "12300000001");
    std::cout <<a1*b1<< std::endl;
    BCD c(0, "123");
    BCD d = -c;
    std::cout << d << std::endl;
    BCD reciprocal = calculateReciprocal(-5, 100);
    std::cout << reciprocal<< std::endl;
    BCD reciprocal2 = calculateReciprocal(345671, 100);
    std::cout << reciprocal2 << std::endl;
    const int target_precision = 100;
    const int computation_precision = 400;
    BCD sum(1);
    BCD term(1);
    int n = 1;
    sum.set_precision(computation_precision);
    term.set_precision(computation_precision);
    while (n < 71) {
        BCD recip_n = calculateReciprocal(n, computation_precision);
        term = term * recip_n;
        BCD old_sum = sum;
        sum = old_sum +  term;
        n++;
    }
    sum.set_precision(target_precision);
    std::cout << "e = " << sum <<" " <<sum.fractional_part.length()<<"\n";
    return 0;
}