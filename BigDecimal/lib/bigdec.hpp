#pragma once

#include <string>
#include <deque>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <concepts>
#include <limits>

#include "bigspecials.hpp"

template<typename integral_t>
concept integral = std::integral<integral_t>;

template<typename floating_t>
concept floating_point = std::floating_point<floating_t>;

template<typename arithmetic_t>
concept arithmetic = std::integral<arithmetic_t> or std::floating_point<arithmetic_t>;

template<class string_t>
concept string = std::is_convertible_v<string_t, std::string>;

template<class wstring_t>
concept wstring = std::is_convertible_v<wstring_t, std::wstring>;

template<typename char_t>
concept character = std::is_same_v<char_t, char> || std::is_same_v<char_t, wchar_t>;

template <uint32_t max_size>
class BigDecimal {
	using digit_t = int_fast16_t;
	using length_t = int_fast32_t;
private:
	void unoverflow() {
		digit_t hdigit = this->highest_digit();
		// change sign if required
		if (hdigit < 0) {
			this->positive = !this->positive;
			for (length_t i = 0; i < this->size; i++)
				this->val[i] *= (-1);
		}
		// change -0 and 0.00 values to 0
		else if (hdigit == 0) {
			this->positive = true;

			this->exp = 0;
			this->size = 0;
			return;
		}

		for (length_t i = 0; i < this->size; i++) {
			// prevent over 9 overflow
			if (this->val[i] > 9) {
				if (i == (this->size - 1)) {
					if(i == (max_size - 1))
						i--;

					this->alloc(this->size + 1);
					this->exp++;
				}

				this->val[i + 1] += static_cast<digit_t>(this->val[i] / 10);
				this->val[i] = this->val[i] % 10;
			}
			// prevent under 0 overflow (with correct sign set)
			else if (this->val[i] < 0) {
				length_t s{ i };
				for (s; s < this->size; s++) {
					if (this->val[s] > 0) break;
				}

				// s must be lower than (this->size - 1)

				this->val[s]--;
				for (length_t y = (s - 1); y > i; y--)
					this->val[y] += 9;

				this->val[i] += 10;
				i = s;
			}
		}

		// removes zeroes from start of decimal
		while (this->val[this->size - 1] == 0) {
			this->size--;
			this->exp--;
		}

		// removes zeroes from end of decimal
		while (this->val[0] == 0) {
			this->val.pop_front();
			this->size--;
		}
	}
	void alloc(length_t new_size) {
		if (new_size <= this->size) return;

		this->val.resize(new_size);

		if (new_size > max_size) {
			if (this->val[new_size - max_size - 1] >= 5) {
				this->val[new_size - max_size]++;

				for (length_t i = (new_size - max_size); i < this->size; i++) {
					// prevent numeric overflow
					if (this->val[i] > 9) {
						this->val[i + 1] += static_cast<digit_t>(this->val[i] / 10);
						this->val[i] = this->val[i] % 10;
					}
					else break;
				}
			}

			const auto begin = this->val.begin();
			this->val.erase(begin, begin + new_size - max_size);
		}

		this->size = this->val.size();
	}
	digit_t highest_digit() {
		// returns first sigificant digit otherwise returns 0
		for (length_t i = (this->size - 1); i >= 0; i--) {
			if (this->val[i] != 0)
				return this->val[i];
		}

		return 0;
	}
	digit_t lowest_digit() {
		// returns last sigificant digit otherwise returns 0
		for (length_t i = 0; i < this->size; i++) {
			if (this->val[i] != 0)
				return this->val[i];
		}

		return 0;
	}
protected:
	std::deque<digit_t> val = {}; // digits container
	// digits are stored in reverse order (most significant digit is in the end)
	length_t size{ 0 };
	length_t exp{ 0 }; // notation number (specify number of non-fractional digits)
	bool positive{ true };
public:
	BigDecimal() = default;
	template <typename integral_t> requires integral<integral_t>
	BigDecimal(integral_t integer) {
		if (integer == 0) return;
		else if (integer < 0) {
			this->positive = false;
			integer *= -1;
		}

		this->size = 0;
		this->exp = 0;

		do {
			this->alloc(this->size + 1);
			this->exp++;

			this->val[this->size - 1] = integer % 10;
			integer /= 10;
		} while (integer != 0);
	}
	template<typename floating_t> requires floating_point<floating_t>
	BigDecimal(floating_t float_num) {
		if (float_num == 0) return;
		else if (float_num < 0) {
			this->positive = false;
			float_num *= -1;
		}

		this->size = 0;
		this->exp = 0;

		uintmax_t num = static_cast<uintmax_t>(float_num);
		while (num != float_num) {
			num *= 10;
			float_num *= 10;
			num += static_cast<uintmax_t>(float_num) % 10;
			this->exp--;
		}

		while (num != 0) {
			this->alloc(this->size + 1);
			this->exp++;

			this->val[this->size - 1] = num % 10;
			num /= 10;
		}
	}
	template<typename string_t> requires string<string_t>
	BigDecimal(string_t str_v) {
		std::string str{ str_v };
		
		if(str.length() == 0) return;

		length_t s{ 0 };

		if (str[0] == '-') {
			this->positive = false;
			s++;
		}
		// support for '+' notation for positive nums
		else if (str[0] == '+') {
			this->positive = true;
			s++;
		}

		while (str[s] == '0') s++;

		this->size = 0;
		this->exp = 0;

		for (length_t i = (str.size() - 1); i >= s; i--) {
			if (BigDecimal::is_comma(str[i]))
				this->exp -= str.size() - i - 1;
			else {
				this->alloc(this->size + 1);
				this->exp++;

				this->val[this->size - 1] = std::stoi(str.substr(i, 1));
			}
		}

		this->unoverflow();
	}
	template<typename wstring_t> requires wstring<wstring_t>
	BigDecimal(wstring_t wstr_v) {
		// exact copy of BigDecimal constructor for string_t, including wide-string support
		std::wstring wstr{ wstr_v };

		if (wstr.length() == 0) return;

		length_t s{ 0 };

		if (wstr[0] == L'-') {
			this->positive = false;
			s++;
		}
		// support for '+' notation for positive nums
		else if (wstr[0] == L'+') {
			this->positive = true;
			s++;
		}

		while (wstr[s] == L'0') s++;

		this->size = 0;
		this->exp = 0;

		for (length_t i = (wstr.size() - 1); i >= s; i--) {
			if (BigDecimal::is_comma(wstr[i]))
				this->exp -= wstr.size() - i - 1;
			else {
				this->alloc(this->size + 1);
				this->exp++;

				this->val[this->size - 1] = std::stoi(wstr.substr(i, 1));
			}
		}

		this->unoverflow();
	}
	template <length_t other_max_size>
	BigDecimal(BigDecimal<other_max_size> big_decimal) {
		std::deque<digit_t> rev_val = big_decimal.value();

		this->exp = big_decimal.exponent();
		this->positive = (big_decimal >= 0);

		this->alloc(rev_val.size());
		for (length_t i = 0; i < this->size; i++)
			this->val[i] = rev_val[this->size - i - 1];
	}
	template <typename integral_t> requires integral<integral_t>
	operator integral_t () const {
		integral_t num{ 0 };
		for (length_t i = 1; i <= this->exp; i++) {
			num *= 10;
			if(i <= this->size)
				num += this->val[this->size - i];
		}

		if (this->positive == false)
			num *= -1;

		return num;
	}
	template<typename floating_t> requires floating_point<floating_t>
	operator floating_t () const {
		floating_t float_part{ 0 };
		for (length_t i = 0; i < (this->size - this->exp); i++) {
			if (i < this->size)
				float_part += this->val[i];
			float_part /= 10;
		}

		floating_t int_part{ 0 };
		for (length_t i = 1; i <= this->exp; i++) {
			int_part *= 10;
			if (i <= this->size)
				int_part += this->val[this->size - i];
		}

		floating_t num = int_part + float_part;
		if (this->positive == false)
			num *= -1;

		return num;
	}
	template <uint32_t other_max_size>
	operator BigDecimal<other_max_size>() {
		return BigDecimal<other_max_size>((*this));
	}
	std::string to_string() {
		std::string str{ "" };

		if (this->positive == false)
			str += "-";

		for (length_t i = 1; i <= exp; i++) {
			if (i <= this->size)
				str += std::to_string(this->val[this->size - i]);
			else
				str += "0";
		}

		if (this->exp <= 0)
			str += "0";

		if (this->exp < this->size) {
			str += ".";
			for (length_t i = (this->exp + 1); i <= this->size; i++) {
				if (i > 0 && i <= this->size)
					str += std::to_string(this->val[this->size - i]);
				else
					str += "0";
			}
		}

		return str;
	}
	std::wstring to_wstring() {
		std::wstring wstr{ L"" };

		if (this->positive == false)
			wstr += L"-";

		for (length_t i = 1; i <= exp; i++) {
			if (i <= this->size)
				wstr += std::to_wstring(this->val[this->size - i]);
			else
				wstr += L"0";
		}

		if (this->exp <= 0)
			wstr += L"0";

		if (this->exp < this->size) {
			wstr += L".";
			for (length_t i = (this->exp + 1); i <= this->size; i++) {
				if (i > 0 && i <= this->size)
					wstr += std::to_wstring(this->val[this->size - i]);
				else
					wstr += L"0";
			}
		}

		return wstr;
	}
	std::ostream& operator<< (std::ostream& stream) {
		stream << this->to_string();
		return stream;
	}
	bool operator== (const BigDecimal& right) const {
		// check if both numbers have same integer part length
		if (this->exp != right.exp)
			return false;

		// check signs of both nums
		if (this->positive != right.positive)
			return false;

		const BigDecimal* min_size_v;
		const BigDecimal* max_size_v;

		if (this->size > right.size) {
			min_size_v = &right;
			max_size_v = this;
		}
		else {
			min_size_v = this;
			max_size_v = &right;
		}

		for (length_t i = 0; i < (max_size_v->size - min_size_v->size); i++) {
			if (max_size_v->val[i] != 0)
				return false;
		}

		for (length_t i = 1; i <= min_size_v->size; i++) {
			if (min_size_v->val[min_size_v->size - i] != max_size_v->val[max_size_v->size - i])
				return false;
		}

		return true;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator == (arithmetic_t right) const {
		return (*this) == BigDecimal(right);
	}
	bool operator != (const BigDecimal& right) const {
		return !((*this) == right);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator != (arithmetic_t right) const {
		return (*this) != BigDecimal(right);
	}
	bool operator > (const BigDecimal& right) const {
		// check if both numbers have different integer part length
		if (this->exp != right.exp && this->size > 0 && right.size > 0)
			return (this->exp > right.exp);

		// check signs of both nums
		if (this->positive != right.positive)
			return (this->positive);

		const BigDecimal* min_size_v;
		const BigDecimal* max_size_v;

		if (this->size > right.size) {
			min_size_v = &right;
			max_size_v = this;
		}
		else {
			min_size_v = this;
			max_size_v = &right;
		}

		for (length_t i = 1; i <= min_size_v->size; i++) {
			if (min_size_v->val[min_size_v->size - i] != max_size_v->val[max_size_v->size - i])
				return this->val[this->size - i] > right.val[right.size - i];
		}

		for (length_t i = 0; i < (max_size_v->size - min_size_v->size); i++) {
			if (max_size_v->val[i] != 0)
				return (max_size_v == this);
		}

		return false;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator > (arithmetic_t right) const {
		return (*this) > BigDecimal(right);
	}
	bool operator < (const BigDecimal& right) const {
		// exact copy of operator > with just changed operator ">" to "<"

		// check if both numbers have different integer part length
		if (this->exp != right.exp && this->size > 0 && right.size > 0)
			return this->exp < right.exp;

		// check signs of both nums
		if (this->positive != right.positive)
			return !(this->positive);

		const BigDecimal* min_size_v;
		const BigDecimal* max_size_v;

		if (this->size > right.size) {
			min_size_v = &right;
			max_size_v = this;
		}
		else {
			min_size_v = this;
			max_size_v = &right;
		}

		for (length_t i = 1; i <= min_size_v->size; i++) {
			if (min_size_v->val[min_size_v->size - i] != max_size_v->val[max_size_v->size - i])
				return this->val[this->size - i] < right.val[right.size - i];
		}

		for (length_t i = 0; i < (max_size_v->size - min_size_v->size); i++) {
			if (max_size_v->val[i] != 0)
				return (max_size_v != this);
		}

		return false;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator < (arithmetic_t right) const {
		return (*this) < BigDecimal(right);
	}
	bool operator >= (const BigDecimal& right) const {
		return ((*this) == right || (*this) > right);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator >= (arithmetic_t right) const {
		return (*this) >= BigDecimal(right);
	}
	bool operator <= (const BigDecimal& right) const {
		return ((*this) == right || (*this) < right);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	bool operator <= (arithmetic_t right) const {
		return (*this) <= BigDecimal(right);
	}
	BigDecimal operator + (const BigDecimal& right) const {
		BigDecimal res;
		res.exp = std::max(this->exp, right.exp);
		res.alloc(res.exp + std::max(this->size - this->exp, right.size - right.exp));
		res.positive = (this->positive || right.positive);

		length_t this_i = (this->size - this->exp) - (res.size - res.exp);
		length_t right_i = (right.size - right.exp) - (res.size - res.exp);
		for (length_t i = 0; i < res.size; i++) {
			if (this_i >= 0 && this_i < this->size)
				res.val[i] += (this->positive == res.positive) ? this->val[this_i] : (-1) * this->val[this_i];

			if (right_i >= 0 && right_i < right.size)
				res.val[i] += (right.positive == res.positive) ? right.val[right_i] : (-1) * right.val[right_i];

			this_i++;
			right_i++;
		}

		res.unoverflow();
		return res;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator + (arithmetic_t right) {
		return (*this) + BigDecimal(right);
	}
	BigDecimal operator - (const BigDecimal& right) const {
		BigDecimal res;
		res.exp = std::max(this->exp, right.exp);
		res.alloc(res.exp + std::max(this->size - this->exp, right.size - right.exp));
		res.positive = (this->positive || right.positive);

		length_t this_i = (this->size - this->exp) - (res.size - res.exp);
		length_t right_i = (right.size - right.exp) - (res.size - res.exp);
		for (length_t i = 0; i < res.size; i++) {
			if (this_i >= 0 && this_i < this->size)
				res.val[i] += (this->positive == res.positive) ? this->val[this_i] : (-1) * this->val[this_i];

			if (right_i >= 0 && right_i < right.size)
				res.val[i] -= (right.positive == res.positive) ? right.val[right_i] : (-1) * right.val[right_i];

			this_i++;
			right_i++;
		}

		res.unoverflow();
		return res;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator - (arithmetic_t right) {
		return (*this) - BigDecimal(right);
	}
	BigDecimal operator * (const BigDecimal& right) const {
		BigDecimal res;
		res.exp = this->exp + right.exp;
		res.positive = (this->positive == right.positive);

		std::deque<digit_t> val{};
		val.resize((this->size + right.size), 0);

		for (length_t r = 0; r < right.size; r++) {
			for (length_t t = 0; t < this->size; t++) {
				length_t index = r + t;
				val[index] += right.val[r] * this->val[t];

				// prevent overflow
				if (val[index] > 9) {
					val[index + 1] += val[index] / 10;
					val[index] %= 10;
				}
			}
		}

		while (val.size() > 0 && val[val.size() - 1] == 0) {
			val.pop_back();
			res.exp--;
		}

		for (length_t i = 0; i < val.size(); i++) {
			res.alloc(res.size + 1);
			res.val[res.size - 1] = val[i];
		}

		res.unoverflow();
		return res;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator * (arithmetic_t right) {
		return (*this) * BigDecimal(right);
	}
	BigDecimal operator / (const BigDecimal& right) const {
		if (right == 0) {
			if ((*this) == 0)
				throw bigspecials::undefined;
			else
				throw bigspecials::complex_infinity;
		}

		BigDecimal res;
		res.exp = this->exp;
		res.positive = (this->positive == right.positive);

		std::deque<digit_t> rev_val{};

		BigDecimal divisive;
		BigDecimal divider = right;
		divider.positive = true;

		length_t first_div{ 0 };

		do {
			divisive *= 10;
			if(rev_val.size() < this->size)
				divisive += this->val[this->size - 1 - rev_val.size()];

			rev_val.push_back(0);
			while (divisive >= divider) {
				divisive -= divider;

				rev_val[rev_val.size() - 1]++;

				// counting division precision
				if (first_div == 0)
					first_div = rev_val.size();
			}
		} while ((divisive != 0 || rev_val.size() < (this->size)) && ((rev_val.size() - first_div) < max_size));

		while (rev_val.size() > 0 && rev_val[0] == 0) {
			rev_val.pop_front();
			res.exp--;
		}
		
		for (length_t i = (rev_val.size() - 1); i >= 0; i--) {
			res.alloc(res.size + 1);
			res.val[res.size - 1] = rev_val[i];
		}

		res.unoverflow();
		return res;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator / (arithmetic_t right) {
		return (*this) / BigDecimal(right);
	}
	BigDecimal operator % (const BigDecimal& right) const {
		if (right == 0) {
			if ((*this) == 0)
				throw bigspecials::undefined;
			else
				throw bigspecials::complex_infinity;
		}

		BigDecimal divisive = (*this);
		divisive.positive = true;

		BigDecimal divider = right;
		divider.positive = true;
		divider.exp = (divisive.exp > divider.exp) ? divisive.exp : divider.exp;

		while (divisive >= right && divider.exp >= right.exp) {
			while (divisive >= divider)
				divisive -= divider;

			divider.exp--;
		}

		divider.exp = right.exp;
		
		BigDecimal res = (this->positive == right.positive || divisive == 0) ? divisive : (divider - divisive);
		res.positive = right.positive;

		return res;
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator % (arithmetic_t right) {
		return (*this) % BigDecimal(right);
	}
	BigDecimal& operator += (const BigDecimal& right) {
		(*this) = (*this) + right;
		return (*this);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator += (arithmetic_t right) {
		return (*this) += BigDecimal(right);
	}
	BigDecimal& operator -= (const BigDecimal& right) {
		(*this) = (*this) - right;
		return (*this);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator -= (arithmetic_t right) {
		return (*this) -= BigDecimal(right);
	}
	BigDecimal& operator *= (const BigDecimal& right) {
		(*this) = (*this) * right;
		return (*this);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator *= (arithmetic_t right) {
		return (*this) *= BigDecimal(right);
	}
	BigDecimal& operator /= (const BigDecimal& right) {
		(*this) = (*this) / right;
		return (*this);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator /= (arithmetic_t right) {
		return (*this) /= BigDecimal(right);
	}
	BigDecimal& operator %= (const BigDecimal& right) {
		(*this) = (*this) % right;
		return (*this);
	}
	template<typename arithmetic_t> requires arithmetic<arithmetic_t>
	BigDecimal operator %= (arithmetic_t right) {
		return (*this) %= BigDecimal(right);
	}
	BigDecimal& operator++() {
		if (this->exp > 0 && this->exp <= this->size) {
			this->val[this->size - this->exp]++;
			this->unoverflow();
		}
		else
			(*this) += 1;

		return (*this);
	}
	BigDecimal& operator--() {
		if (this->exp >= 0 && this->exp <= this->size) {
			this->val[this->size - this->exp]--;
			this->unoverflow();
		}
		else
			(*this) -= 1;
		return (*this);
	}
	BigDecimal operator++(int) {
		BigDecimal prev = (*this);
		++(*this);
		return prev;
	}
	BigDecimal operator--(int) {
		BigDecimal prev = (*this);
		--(*this);
		return prev;
	}
	BigDecimal operator-() {
		BigDecimal res = (*this);
		res.positive = !res.positive;

		return res;
	}
	std::deque<digit_t> value() {
		std::deque<digit_t> value = this->val;

		while (value[value.size() - 1] == 0)
			value.pop_back();

		std::reverse(value.begin(), value.end());

		return value;
	}
	length_t exponent() {
		return this->exp;
	}
	length_t length() {
		return this->size;
	}
	~BigDecimal() {}
	// static members
	static constexpr auto digit_limit = std::numeric_limits<digit_t>::max();
	static constexpr auto size_limit = std::numeric_limits<length_t>::max();
	template<typename char_t> requires character<char_t>
	static bool is_digit(char_t ch) {
		return (ch >= 48 && ch <= 57);
	}
	template<typename char_t> requires character<char_t>
	static bool is_comma(char_t ch) {
		return (ch == static_cast<int>('.') || ch == static_cast<int>(','));
	}
};