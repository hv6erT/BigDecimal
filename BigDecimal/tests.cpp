#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <concepts>

#include "lib/bigdec.hpp"
#include "lib/bigmath.hpp"

using uint = unsigned int;

constexpr uint max_size{ 25 };
constexpr uint max_log_size{ 40 };

template<typename cout_t>
concept iostream = requires(cout_t a) {
	{ std::cout << a };
};

template<typename wcout_t>
concept wiostream = requires(wcout_t a) {
	{ std::wcout << a };
} && !iostream<wcout_t>;

namespace testing {
	uint total_tests{ 0 };
	uint passed_tests{ 0 };
	uint failed_tests{ 0 };
	struct test_conf {
		std::string test_name{ "unknown_test" };
		bool write_passed{ false };
	};
	template <iostream cout_t>
	bool test(const cout_t& value, const cout_t& req_value, const test_conf& conf) {
		total_tests++;

		std::ios_base::sync_with_stdio(false);

		if (value == req_value) {
			if (conf.write_passed)
				std::cout << "Test: " << conf.test_name << " passed" << std::endl;

			passed_tests++;
			return true;
		}
		else {
			std::cout << "Test: " << conf.test_name << " failed" << std::endl;
			std::cout << "\t Required value: " << req_value << " , given value: " << value << std::endl;

			failed_tests++;
			return false;
		}
	}
	template <wiostream wcout_t>
	bool test(const wcout_t& value, const wcout_t& req_value, const test_conf& conf) {
		total_tests++;

		std::ios_base::sync_with_stdio(false);

		std::wstring test_name = std::wstring(conf.test_name.begin(), conf.test_name.end());
		if (value == req_value) {
			if (conf.write_passed)
				std::wcout << L"Test: " << test_name << L" passed" << std::endl;

			passed_tests++;
			return true;
		}
		else {
			std::wcout << L"Test: " << test_name << L" failed" << std::endl;
			std::wcout << L"\t Required value: " << req_value << L" , given value: " << value << std::endl;

			failed_tests++;
			return false;
		}
	}
}

namespace conf {
	bool all = false;
	bool type_casting = false;
	bool comparision = false;
	bool basic_operations = false;
	bool rounding_functions = false;
	bool logarithm_functions = false;
	bool power_function = false;
	bool trygonometric_functions = true;
}

int main() {
	std::cout << "<< Testing started..." << std::endl << std::endl;

	testing::test_conf def_test_conf;
	def_test_conf.write_passed = true;

	// type casting
	if(conf::all || conf::type_casting)
	{
		std::cout << "<< Testing type casting: " << std::endl;

		BigDecimal<max_size> a0(0);
		BigDecimal<max_size> a1(7);
		BigDecimal<max_size> a2(0.5);
		BigDecimal<max_size> a3(100);
		BigDecimal<max_size> a4(-56);
		BigDecimal<max_size> a5(34.79);
		BigDecimal<max_size> a6(-24.49);
		BigDecimal<max_size> a7("457");
		BigDecimal<max_size> a8("-679");
		BigDecimal<max_size> a9("459.011");
		BigDecimal<max_size> a10("-6969.9696");
		BigDecimal<max_size> a11("4598,67400000000");
		BigDecimal<max_size> a12(L"321");
		BigDecimal<max_size> a13(L"-764");
		BigDecimal<max_size> a14(L"5493.735");
		BigDecimal<max_size> a15(L"-0.86869");
		BigDecimal<max_size> a16(L"+76.000100000");

		std::vector<BigDecimal<max_size>> a{ a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15,a16 };

		std::vector<long> at_1{ 0, 7, 0, 100, -56, 34, -24, 457, -679, 459, -6969, 4598, 321, -764, 5493, 0, 76 };
		for (uint t = 0; t < at_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "at_1_" + std::to_string(t);

			testing::test(static_cast<long>(a[t]), at_1[t], conf);
		}

		std::vector<double> at_2{ 0, 7, 0.5, 100, -56, 34.79, -24.49, 457, -679, 459.011, -6969.9696, 4598.674, 321, -764, 5493.735, -0.86869, 76.0001 };
		for (uint t = 0; t < at_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "at_2_" + std::to_string(t);

			testing::test(static_cast<double>(a[t]), at_2[t], conf);
		}

		std::vector<std::string> at_3{ "0", "7", "0.5", "100", "-56", "34.79", "-24.49", "457", "-679", "459.011", "-6969.9696", "4598.674", "321", "-764", "5493.735", "-0.86869","76.0001" };
		for (uint t = 0; t < at_3.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "at_3_" + std::to_string(t);

			testing::test(a[t].to_string(), at_3[t], conf);
		}

		std::vector<std::wstring> at_4{ L"0", L"7", L"0.5", L"100", L"-56", L"34.79", L"-24.49", L"457", L"-679", L"459.011", L"-6969.9696", L"4598.674", L"321", L"-764", L"5493.735", L"-0.86869", L"76.0001" };
		for (uint t = 0; t < at_4.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "at_4_" + std::to_string(t);

			testing::test(a[t].to_wstring(), at_4[t], conf);
		}

		std::cout << std::endl;
	}

	// comparison operators between two BigDecimals
	if(conf::all || conf::comparision)
	{
		std::cout << "<< Testing comaparision operators between two BigDecimals: " << std::endl;

		BigDecimal<max_size> b0_1(0);
		BigDecimal<max_size> b0_2("-0");
		BigDecimal<max_size> b1_1(458);
		BigDecimal<max_size> b1_2(123);
		BigDecimal<max_size> b2_1(86.89);
		BigDecimal<max_size> b2_2(86.40);
		BigDecimal<max_size> b3_1(-65);
		BigDecimal<max_size> b3_2(L"-65");
		BigDecimal<max_size> b4_1("-56.75");
		BigDecimal<max_size> b4_2(-56.75);
		BigDecimal<max_size> b5_1(987.654);
		BigDecimal<max_size> b5_2(987.652);
		BigDecimal<max_size> b6_1(L"09.0");
		BigDecimal<max_size> b6_2(9.0);
		BigDecimal<max_size> b7_1(L"-89");
		BigDecimal<max_size> b7_2("89");
		BigDecimal<max_size> b8_1(L"54");
		BigDecimal<max_size> b8_2("54.000001");
		BigDecimal<max_size> b9_1("0.000000000000000000001");
		BigDecimal<max_size> b9_2(L"0.00001");

		std::vector<BigDecimal<max_size>> b_1{ b0_1, b1_1, b2_1, b3_1, b4_1, b5_1, b6_1, b7_1, b8_1, b9_1 };
		std::vector<BigDecimal<max_size>> b_2{ b0_2, b1_2, b2_2, b3_2, b4_2, b5_2, b6_2, b7_2, b8_2, b9_2 };

		std::vector<bool> bt_1{ true, false, false, true, true, false, true, false, false, false };
		for (uint t = 0; t < bt_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "bt_1_" + std::to_string(t);

			testing::test(static_cast<int>(b_1[t] == b_2[t]), static_cast<int>(bt_1[t]), conf);
		}

		std::vector<bool> bt_2{ false, true, true, false, false, true, false, false, false, false };
		for (uint t = 0; t < bt_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "bt_2_" + std::to_string(t);

			testing::test(static_cast<int>(b_1[t] > b_2[t]), static_cast<int>(bt_2[t]), conf);
		}

		std::vector<bool> bt_3{ false, false, false, false, false, false, false, true, true, true };
		for (uint t = 0; t < bt_3.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "bt_3_" + std::to_string(t);

			testing::test(static_cast<int>(b_1[t] < b_2[t]), static_cast<int>(bt_3[t]), conf);
		}

		for (uint t = 0; t < std::min(bt_1.size(), bt_2.size()); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "bt_4_" + std::to_string(t);

			testing::test(static_cast<int>(b_1[t] >= b_2[t]), static_cast<int>((bt_1[t] || bt_2[t])), conf);
		}

		for (uint t = 0; t < std::min(bt_1.size(), bt_3.size()); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "bt_5_" + std::to_string(t);

			testing::test(static_cast<int>(b_1[t] <= b_2[t]), static_cast<int>((bt_1[t] || bt_3[t])), conf);
		}

		std::cout << std::endl;
	}

	// addition and substraction between two BigDecimals
	if(conf::all || conf::basic_operations)
	{
		std::cout << "<< Testing addition and substraction between two BigDecimals: " << std::endl;

		BigDecimal<max_size> c0_1(76);
		BigDecimal<max_size> c0_2(23);
		BigDecimal<max_size> c1_1(75);
		BigDecimal<max_size> c1_2(65);
		BigDecimal<max_size> c2_1(35.5);
		BigDecimal<max_size> c2_2(4);
		BigDecimal<max_size> c3_1("78.63");
		BigDecimal<max_size> c3_2("2.24");
		BigDecimal<max_size> c4_1("0.86");
		BigDecimal<max_size> c4_2("0.49");
		BigDecimal<max_size> c5_1("1.765");
		BigDecimal<max_size> c5_2("0.3454");
		BigDecimal<max_size> c6_1(-43);
		BigDecimal<max_size> c6_2(-12);
		BigDecimal<max_size> c7_1(-45.5);
		BigDecimal<max_size> c7_2(-87.4);
		BigDecimal<max_size> c8_1(0.04);
		BigDecimal<max_size> c8_2(0.6);
		BigDecimal<max_size> c9_1("-23.7");
		BigDecimal<max_size> c9_2("23.7");
		BigDecimal<max_size> c10_1("-179.63");
		BigDecimal<max_size> c10_2("17.5");
		BigDecimal<max_size> c11_1("0");
		BigDecimal<max_size> c11_2("-0.012");

		std::vector<BigDecimal<max_size>> c_1{ c0_1, c1_1, c2_1, c3_1, c4_1, c5_1, c6_1, c7_1, c8_1, c9_1, c10_1, c11_1 };
		std::vector<BigDecimal<max_size>> c_2{ c0_2, c1_2, c2_2, c3_2, c4_2, c5_2, c6_2, c7_2, c8_2, c9_2, c10_2, c11_2 };

		std::vector<std::string> ct_1{ "99", "140", "39.5", "80.87", "1.35", "2.1104", "-55", "-132.9", "0.64", "0", "-162.13", "-0.012" };
		for (uint t = 0; t < ct_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ct_1_" + std::to_string(t);

			testing::test((c_1[t] + c_2[t]).to_string(), ct_1[t], conf);
		}

		std::vector<std::string> ct_2{ "53", "10", "31.5", "76.39", "0.37", "1.4196", "-31", "41.9", "-0.56", "-47.4", "-197.13", "0.012" };
		for (uint t = 0; t < ct_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ct_2_" + std::to_string(t);

			testing::test((c_1[t] - c_2[t]).to_string(), ct_2[t], conf);
		}

		for (uint t = 0; t < ct_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ct_3_" + std::to_string(t);

			BigDecimal<max_size> c_1_copy = c_1[t];

			testing::test((c_1_copy += c_2[t]).to_string(), ct_1[t], conf);
		}

		for (uint t = 0; t < ct_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ct_4_" + std::to_string(t);

			BigDecimal<max_size> c_1_copy = c_1[t];

			testing::test((c_1_copy -= c_2[t]).to_string(), ct_2[t], conf);
		}

		std::cout << std::endl;
	}

	// multiplication and division between two BigDecimals
	if(conf::all || conf::basic_operations)
	{
		std::cout << "<< Testing multiplication and division between two BigDecimals: " << std::endl;

		BigDecimal<max_size> d0_1(0);
		BigDecimal<max_size> d0_2(1);
		BigDecimal<max_size> d1_1(6);
		BigDecimal<max_size> d1_2(4);
		BigDecimal<max_size> d2_1(25);
		BigDecimal<max_size> d2_2(5);
		BigDecimal<max_size> d3_1(12.5);
		BigDecimal<max_size> d3_2(-5);
		BigDecimal<max_size> d4_1(0.02);
		BigDecimal<max_size> d4_2("0.06");
		BigDecimal<max_size> d5_1(-0.05);
		BigDecimal<max_size> d5_2(-1);
		BigDecimal<max_size> d6_1(15);
		BigDecimal<max_size> d6_2(8);
		BigDecimal<max_size> d7_1("12345.6789");
		BigDecimal<max_size> d7_2("-9876.54321");

		std::vector<BigDecimal<max_size>> d_1{ d0_1, d1_1, d2_1, d3_1, d4_1, d5_1, d6_1, d7_1 };
		std::vector<BigDecimal<max_size>> d_2{ d0_2, d1_2, d2_2, d3_2, d4_2, d5_2, d6_2, d7_2 };

		std::vector<std::string> dt_1{ "0", "24", "125", "-62.5", "0.0012", "0.05", "120", "-121932631.112635269" };
		for (uint t = 0; t < dt_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "dt_1_" + std::to_string(t);

			testing::test((d_1[t] * d_2[t]).to_string(), dt_1[t], conf);
		}

		std::vector<std::string> dt_2{ "0", "1.5", "5", "-2.5", "0.3333333333333333333333333", "0.05", "1.875", "-1.249999988609375000142383" };
		for (uint t = 0; t < dt_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "dt_2_" + std::to_string(t);

			testing::test((d_1[t] / d_2[t]).to_string(), dt_2[t], conf);
		}

		for (uint t = 0; t < dt_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "dt_3_" + std::to_string(t);

			BigDecimal<max_size> d_1_copy = d_1[t];

			testing::test((d_1_copy *= d_2[t]).to_string(), dt_1[t], conf);
		}

		for (uint t = 0; t < dt_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "dt_4_" + std::to_string(t);

			BigDecimal<max_size> d_2_copy = d_1[t];

			testing::test((d_2_copy /= d_2[t]).to_string(), dt_2[t], conf);
		}

		std::cout << std::endl;
	}

	// modulo between two BigDecimal
	if(conf::all || conf::basic_operations)
	{
		std::cout << "<< Testing modulo between two BigDecimals: " << std::endl;

		BigDecimal<max_size> e0_1(0);
		BigDecimal<max_size> e0_2(1);
		BigDecimal<max_size> e1_1(6);
		BigDecimal<max_size> e1_2(4);
		BigDecimal<max_size> e2_1(25);
		BigDecimal<max_size> e2_2(5);
		BigDecimal<max_size> e3_1(12.5);
		BigDecimal<max_size> e3_2(-5);
		BigDecimal<max_size> e4_1(0.02);
		BigDecimal<max_size> e4_2("0.06");
		BigDecimal<max_size> e5_1(-0.05);
		BigDecimal<max_size> e5_2(-1);

		std::vector<BigDecimal<max_size>> e_1{ e0_1, e1_1, e2_1, e3_1, e4_1, e5_1 };
		std::vector<BigDecimal<max_size>> e_2{ e0_2, e1_2, e2_2, e3_2, e4_2, e5_2 };

		std::vector<std::string> et_1{ "0", "2", "0", "-2.5", "0.02", "-0.05" };
		for (uint t = 0; t < et_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "et_1_" + std::to_string(t);

			testing::test((e_1[t] % e_2[t]).to_string(), et_1[t], conf);
		}

		for (uint t = 0; t < et_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "et_2_" + std::to_string(t);

			BigDecimal<max_size> e_1_copy = e_1[t];

			testing::test((e_1_copy %= e_2[t]).to_string(), et_1[t], conf);
		}

		std::cout << std::endl;
	}

	// testing abs and rounding functions
	if(conf::all || conf::rounding_functions)
	{
		std::cout << "Testing abs and rounding functions of BigDecimal: " << std::endl;

		BigDecimal<max_size> f0(-4.2);
		BigDecimal<max_size> f1(0.1);
		BigDecimal<max_size> f2(0);
		BigDecimal<max_size> f3(27.5);
		BigDecimal<max_size> f4(2.4);
		BigDecimal<max_size> f5(-8.5);
		BigDecimal<max_size> f6(-0.1);
		BigDecimal<max_size> f7(-0.6);
		BigDecimal<max_size> f8(563.1);

		std::vector<BigDecimal<max_size>> f{ f0, f1, f2, f3, f4, f5, f6, f7, f8 };

		std::vector<std::string> ft_1{ "4.2", "0.1", "0", "27.5", "2.4", "8.5", "0.1", "0.6", "563.1"};
		for (uint t = 0; t < ft_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_1_" + std::to_string(t);

			testing::test((bigmath::abs<max_size>(f[t])).to_string(), ft_1[t], conf);
		}

		std::vector<std::string> ft_2{ "-4", "0", "0", "28", "2", "-8", "0", "-1", "563" };
		for (uint t = 0; t < ft_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_2_" + std::to_string(t);

			testing::test((bigmath::round<max_size>(f[t], BigDecimal<max_size>(1))).to_string(), ft_2[t], conf);
		}

		std::vector<std::string> ft_3{ "-4", "0", "0", "30", "2", "-8", "0", "-1", "600" };
		for (uint t = 0; t < ft_3.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_3_" + std::to_string(t);

			testing::test((bigmath::round<max_size>(f[t], 1).to_string()), ft_3[t], conf);
		}

		std::vector<std::string> ft_4{ "-4", "1", "0", "28", "3", "-8", "0", "0", "564" };
		for (uint t = 0; t < ft_4.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_4_" + std::to_string(t);

			testing::test((bigmath::ceil<max_size>(f[t], BigDecimal<max_size>(1)).to_string()), ft_4[t], conf);
		}

		std::vector<std::string> ft_5{ "-4", "1", "0", "30", "3", "-8", "0", "0", "600" };
		for (uint t = 0; t < ft_5.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_5_" + std::to_string(t);

			testing::test((bigmath::ceil<max_size>(f[t], 1).to_string()), ft_5[t], conf);
		}

		std::vector<std::string> ft_6{ "-5", "0", "0", "27", "2", "-9", "-1", "-1", "563" };
		for (uint t = 0; t < ft_6.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_6_" + std::to_string(t);

			testing::test((bigmath::floor<max_size>(f[t], BigDecimal<max_size>(1)).to_string()), ft_6[t], conf);
		}

		std::vector<std::string> ft_7{ "-5", "0", "0", "20", "2", "-9", "-1", "-1", "500" };
		for (uint t = 0; t < ft_7.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ft_7_" + std::to_string(t);

			testing::test((bigmath::floor<max_size>(f[t], 1).to_string()), ft_7[t], conf);
		}

		std::cout << std::endl;
	}

	// testing exponential and logarithms functions
	if(conf::all || conf::logarithm_functions)
	{
		std::cout << "Testing exponential and logarithm functions of BigDecimal: " << std::endl;

		BigDecimal<max_log_size> g0(1);
		BigDecimal<max_log_size> g1(2);
		BigDecimal<max_log_size> g2(4.5);
		BigDecimal<max_log_size> g3(8.01);
		BigDecimal<max_log_size> g4(13.74);
		BigDecimal<max_log_size> g5(27.12);

		std::vector<BigDecimal<max_log_size>> g{ g0, g1, g2, g3, g4, g5 };

		std::vector<std::string> gt_1{ "2.718281828", "7.389056099", "90.0171313", "3010.917113", "927269.9404", "59988271620" };
		for (uint t = 0; t < gt_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "gt_1_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::exp<max_log_size>(g[t])), 10).to_string(), gt_1[t], conf);
		}

		std::vector<std::string> gt_2{ "0", "0.693147181", "1.504077397", "2.080690761", "2.620311287", "3.300271463" };
		for (uint t = 0; t < gt_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "gt_2_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::ln<max_log_size>(g[t])), 10).to_string(), gt_2[t], conf);
		}

		std::vector<std::string> gt_3{ "0", "1", "2.169925001", "3.001802243", "3.780310099", "4.761285273" };
		for (uint t = 0; t < gt_3.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "gt_3_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::log2<max_log_size>(g[t])), 10).to_string(), gt_3[t], conf);
		}

		std::vector<std::string> gt_4{ "0", "0.301029996", "0.653212514", "0.903632516", "1.137986733", "1.433289685" };
		for (uint t = 0; t < gt_4.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "gt_4_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::log10<max_log_size>(g[t])), 10).to_string(), gt_4[t], conf);
		}

		std::cout << std::endl;
	}

	// testing power function of BigDecimal
	if(conf::all || conf::power_function)
	{
		std::cout << "Testing power function of BigDecimal: " << std::endl;

		BigDecimal<max_size> h0_1(0);
		BigDecimal<max_size> h0_2(10);
		BigDecimal<max_size> h1_1(1);
		BigDecimal<max_size> h1_2(90);
		BigDecimal<max_size> h2_1(2);
		BigDecimal<max_size> h2_2(6);
		BigDecimal<max_size> h3_1(3);
		BigDecimal<max_size> h3_2(3);
		BigDecimal<max_size> h4_1(8);
		BigDecimal<max_size> h4_2(-1);
		BigDecimal<max_size> h5_1(2);
		BigDecimal<max_size> h5_2("0.5");

		std::vector<BigDecimal<max_size>> h_1{ h0_1, h1_1, h2_1, h3_1, h4_1, h5_1 };
		std::vector<BigDecimal<max_size>> h_2{ h0_2, h1_2, h2_2, h3_2, h4_2, h5_2 };

		std::vector<std::string> ht_0{ "0", "1", "64", "27", "0.125", "1.414213562" };
		for (uint t = 0; t < ht_0.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "ht_0_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::pow<max_size>(h_1[t], h_2[t])), 10).to_string(), ht_0[t], conf);
		}

		std::cout << std::endl;
	}

	// testing trygonometric functions of BigDecimal
	if (conf::all || conf::trygonometric_functions) {
		std::cout << "Testing trygonometric functions of BigDecimal: " << std::endl;

		BigDecimal<max_log_size> i0(0);
		BigDecimal<max_log_size> i1(1);
		BigDecimal<max_log_size> i2(-2);
		BigDecimal<max_log_size> i3(bigmath::constants::pi);
		BigDecimal<max_log_size> i4(17.24);
		BigDecimal<max_log_size> i5(-38.91);

		std::vector<BigDecimal<max_log_size>> i{ i0, i1, i2, i3, i4, i5 };

		std::vector<std::string> it_0{ "0", "0.841470985", "-0.909297427", "0", "-0.999248941", "-0.935929169" };
		for (uint t = 0; t < it_0.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "it_0_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::sin<max_log_size>(i[t])), 10).to_string(), it_0[t], conf);
		}

		std::vector<std::string> it_1{ "1", "0.540302306", "-0.416146837", "-1", "-0.038749891", "0.352188288" };
		for (uint t = 0; t < it_1.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "it_1_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::cos<max_log_size>(i[t])), 10).to_string(), it_1[t], conf);
		}

		std::vector<std::string> it_2{ "0", "1.557407725", "2.185039863", "0", "25.78714220", "-2.657468179" };
		for (uint t = 0; t < it_2.size(); t++) {
			testing::test_conf conf = def_test_conf;
			conf.test_name = "it_2_" + std::to_string(t);

			testing::test(bigmath::round((bigmath::tan<max_log_size>(i[t])), 10).to_string(), it_2[t], conf);
		}

		std::cout << std::endl;
	}

	std::cout << "<< Testing has ended succesfully" << std::endl << std::endl;
	std::cout << "Passed: " << testing::passed_tests << " of " << testing::total_tests << ", failed " << testing::failed_tests << std::endl;
	std::cout << std::endl;

	return 0;
}