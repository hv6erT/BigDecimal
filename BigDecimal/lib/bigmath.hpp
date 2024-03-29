#pragma once

#include <string>
#include <vector>
#include <numeric>
#include <stdexcept>

#include "bigdec.hpp"
#include "bigspecials.hpp"

namespace bigmath {
	using length_t = int_fast32_t;

	const BigDecimal<1> default_epsilon{ 1e-10 };
	namespace constants {
		const BigDecimal<1024> pi{ "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513200056812714526356082778577134275778960917363717872146844090122495343014654958537105079227968925892354201995611212902196086403441815981362977477130996051870721134999999837297804995105973173281609631859502445945534690830264252230825334468503526193118817101000313783875288658753320838142061717766914730359825349042875546873115956286388235378759375195778185778053217122680661300192787661119590921642019893809525720106548586327" };
		const BigDecimal<1024> e{ "2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741359662904357290033429526059563073813232862794349076323382988075319525101901157383418793070215408914993488416750924476146066808226480016847741185374234544243710753907774499206955170276183860626133138458300075204493382656029760673711320070932870912744374704723069697720931014169283681902551510865746377211125238978442505695369677078544996996794686445490598793163688923009879312773617821542499922957635148220826989519366803318252886939849646510582093923982948879332036250944311730123819706841614039701983767932068328237646480429531180232878250981945581530175671736133206981125099618188159304169035159888851934580727386673858942287922849989208680582574927961048419844436346324496848756023362482704197862320900216099023530436994184914631409343173814364054625315209618369088870701676839642437814059271456354906130310720851038375051011574770417189861068739696552126715468895703503540212340784981933432106" };
		const BigDecimal<1024> phi{ "1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475408807538689175212663386222353693179318006076672635443338908659593958290563832266131992829026788067520876689250171169620703222104321626954862629631361443814975870122034080588795445474924618569536486444924104432077134494704956584678850987433944221254487706647809158846074998871240076521705751797883416625624940758906970400028121042762177111777805315317141011704666599146697987317613560067087480710131795236894275219484353056783002287856997829778347845878228911097625003026961561700250464338243776486102838312683303724292675263116533924731671112115881863851331620384005222165791286675294654906811317159934323597349498509040947621322298101726107059611645629909816290555208524790352406020172799747175342777592778625619432082750513121815628551222480939471234145170223735805772786160086883829523045926478780178899219902707769038953219681986151437803149974110692608867429622675756052317277752035361393621076738937645560606059" };
		const BigDecimal<1024> ln2{ "0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754200148102057068573368552023575813055703267075163507596193072757082837143519030703862389167347112335011536449795523912047517268157493206515552473413952588295045300709532636664265410423915781495204374043038550080194417064167151864471283996817178454695702627163106454615025720740248163777338963855069526066834113727387372292895649354702576265209885969320196505855476470330679365443254763274495125040606943814710468994650622016772042452452961268794654619316517468139267250410380254625965686914419287160829380317271436778265487756648508567407764845146443994046142260319309673540257444607030809608504748663852313818167675143866747664789088143714198549423151997354880375165861275352916610007105355824987941472950929311389715599820565439287170007218085761025236889213244971389320378439353088774825970171559107088236836275898425891853530243634214367061189236789192372314672321720534016492568727477823445353476481149418642386776774" };
		const BigDecimal<256> ln10{ "2.30258509299404568401799145468436420760110148862877297603332790096757260967735248023599720508959829834196778404228624863340952546508280675666628736909878168948290720832555468084379989482623319852839350530896537773262884616336622228769821988674654366747440424327436515504893431493939147961940440022210510171417480036880840126470806855677432162283552201148046637156591213734507478569476834636167921018064450706480002775026849167465505868569356734206705811364292245544057589257242082413146956890167589402567763113569192920333765871416602301057030896345720754403708474699401682692828084811842893148485249486448719278096762712757753970276686059524967166741834857044225071979650047149510504922147765676369386629769795221107182645497347726624257094293225827985025855097852653832076067263171643095059950878075237103331011978575473315414218084275438635917781170543098274823850456480190956102992918243182375253577097505395651876975103749708886921802051893395072385392051446341972652872869651108625714921988499787488737713456862091670" };
	}
	template <length_t max_size>
	BigDecimal<max_size> abs(BigDecimal<max_size> x) {
		return (x < 0)? (x * (-1)) : x;
	}
	template <length_t max_size>
	BigDecimal<max_size> _count_epsilon(BigDecimal<max_size> x, length_t digits) {
		if (digits <= 0)
			throw std::runtime_error("Invalid digits param passed");

		length_t exp = x.exponent();

		BigDecimal<max_size> div{ 1 };
		for (length_t e = 1; e <= (exp - digits); e++)
			div *= 10;

		bool is_frac = (exp == 0);

		for (length_t e = 1; e <= (digits - std::abs(exp) - static_cast<length_t>(is_frac)); e++)
			div /= 10;

		return div;
	}
	template <length_t max_size>
	BigDecimal<max_size> round(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		if (epsilon <= 0)
			throw std::runtime_error("Invalid epsilon value passed");

		BigDecimal<max_size> rest = x % epsilon;

		if (rest != 0) {
			if (rest.exponent() == (epsilon.exponent() - 1) && rest.value()[0] >= 5)
				x += epsilon;

			x -= rest;
		}

		return x;
	}
	template <length_t max_size>
	BigDecimal<max_size> round(BigDecimal<max_size> x, length_t digits) {
		BigDecimal<max_size> epsilon = bigmath::_count_epsilon(x, digits);
		return bigmath::round(x, epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> ceil(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		if (epsilon <= 0)
			throw std::runtime_error("Invalid epsilon value passed");

		BigDecimal<max_size> rest = x % epsilon;

		if (rest != 0) {
			x += epsilon;
			x -= rest;
		}

		return x;
	}
	template <length_t max_size>
	BigDecimal<max_size> ceil(BigDecimal<max_size> x, length_t digits) {
		BigDecimal<max_size> epsilon = bigmath::_count_epsilon(x, digits);
		return bigmath::ceil(x, epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> floor(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		if (epsilon <= 0)
			throw std::runtime_error("Invalid epsilon value passed");

		BigDecimal<max_size> rest = x % epsilon;

		if (rest != 0)
			x -= rest;

		return x;
	}
	template <length_t max_size>
	BigDecimal<max_size> floor(BigDecimal<max_size> x, length_t digits) {
		BigDecimal<max_size> epsilon = bigmath::_count_epsilon(x, digits);
		return bigmath::floor(x, epsilon);
	}
	template <length_t max_size>
	std::vector<BigDecimal<max_size>> prime_factorization(BigDecimal<max_size> x) {
		if (x.length() != x.exponent() || x < 0)
			throw std::runtime_error("Invalid x argument passed");

		std::vector<BigDecimal<max_size>> factors{};

		while (x % 2 == 0) {
			factors.push_back(BigDecimal<max_size>(2));
			x /= 2;
		}

		for (BigDecimal<max_size> i = 3; x != 0; i += 2) {
			while (x % i == 0) {
				factors.push_back(i);
				x /= i;
			}
		}

		return factors;
	}
	template <length_t max_size>
	BigDecimal<max_size> sum(std::vector<BigDecimal<max_size>> vec) {
		BigDecimal<max_size> sum;

		for (const BigDecimal<max_size>& el : vec)
			sum += el;

		return sum;
	}
	template <length_t max_size>
	BigDecimal<max_size> exp(BigDecimal<max_size> x, BigDecimal<max_size> epsilon){
		// based on solution from: https://www.quora.com/How-can-I-make-an-exponential-function-in-C-like-e-m-without-using-math-h-library-functions
		
		BigDecimal<max_size> sum{ 0 };
		BigDecimal<max_size> n{ 0 };
		BigDecimal<max_size> factorial{ 1 };
		BigDecimal<max_size> power{ 1 };
		BigDecimal<max_size> term;
		
		do {
			term = power / factorial;
			sum += term;
			n++;
			power *= x;
			factorial *= n;
		} while (bigmath::abs<max_size>(term) >= epsilon);
		
		return sum;
	}
	template <length_t max_size>
	BigDecimal<max_size> exp(BigDecimal<max_size> x) {
		return bigmath::exp<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> ln(BigDecimal<max_size> x, BigDecimal<max_size> epsilon){
		// based on solution from: https://math.stackexchange.com/questions/1438095/building-a-ln-function-from-scratch

		if (x == 0)
			throw bigspecials::negative_infinity;
		else if (x < 0)
			throw std::runtime_error("Invalid argument x passed");
		else if (x == 1)
			return BigDecimal<max_size>(0);
		else {
			BigDecimal<max_size> yn = x - 1; // using the first term of the taylor series as initial-value
			BigDecimal<max_size> yn1 = yn;

			do {
				yn = yn1;

				BigDecimal<max_size> exp_val = bigmath::exp(yn, epsilon);
				yn1 = yn + ((x - exp_val) * 2) / (x + exp_val);
			} while (bigmath::abs(yn - yn1) >= epsilon);

			return yn1;
		}
	}
	template <length_t max_size>
	BigDecimal<max_size> ln(BigDecimal<max_size> x) {
		return bigmath::ln<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> log2(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		if (max_size > bigmath::constants::ln2.size())
			return (bigmath::ln<max_size>(x, epsilon) / bigmath::ln<max_size>(2, epsilon));
		else
			return (bigmath::ln<max_size>(x, epsilon) / bigmath::constants::ln2);
	}
	template <length_t max_size>
	BigDecimal<max_size> log2(BigDecimal<max_size> x) {
		return bigmath::log2<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> log10(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		if (max_size > bigmath::constants::ln10.size())
			return (bigmath::ln<max_size>(x, epsilon) / bigmath::ln<max_size>(10, epsilon));
		else
			return (bigmath::ln<max_size>(x, epsilon) / bigmath::constants::ln10);
	}
	template <length_t max_size>
	BigDecimal<max_size> log10(BigDecimal<max_size> x) {
		return bigmath::log10<max_size>(x, bigmath::default_epsilon);
	}
	template<length_t max_size>
	BigDecimal<max_size> log(BigDecimal<max_size> base, BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		return (bigmath::ln<max_size>(x, epsilon) / bigmath::ln<max_size>(base, epsilon));
	}
	template<length_t max_size>
	BigDecimal<max_size> log(BigDecimal<max_size> base, BigDecimal<max_size> x) {
		return bigmath::log<max_size>(base, x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> pow(BigDecimal<max_size> x, BigDecimal<max_size> y, BigDecimal<max_size> epsilon) {
		// based on solution from: https://stackoverflow.com/questions/2882706/how-can-i-write-a-power-function-myself
		
		if (x == 0) {
			if (y == 0)
				throw bigspecials::undefined;
			else
				return BigDecimal<max_size>(0);
		}
		else if(y == 0)
			return BigDecimal<max_size>(1);
		else if (y % 1 == 0) {
			BigDecimal<max_size> res = x;

			for (BigDecimal<max_size> i = 1; i < y;) {
				if (i * 2 <= y) {
					res *= res;
					i *= 2;
					continue;
				}

				res *= x;
				i++;
			}

			if (y < 0)
				return (BigDecimal<max_size>(1) / res);
			else return res;
		}
		else
			return bigmath::exp<max_size>(y * bigmath::ln<max_size>(x), epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> pow(BigDecimal<max_size> x, BigDecimal<max_size> y) {
		return bigmath::pow<max_size>(x, y, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> factorial(BigDecimal<max_size> x) {
		if (x <= 0)
			throw std::runtime_error("Invalid argument x passed");

		BigDecimal<max_size> res{ 1 };

		for (BigDecimal<max_size> i = 2; i < x; i++)
			x *= 2;

		return res;
	}
	template <length_t max_size>
	BigDecimal<max_size> sin(BigDecimal<max_size> x, BigDecimal<max_size> epsilon){
		// based on solution from: https://stackoverflow.com/questions/34703147/sine-function-without-any-library
		// and from: https://stackoverflow.com/questions/45169675/python-calculate-sine-cosine-with-a-precision-of-up-to-1-million-digits?noredirect=1&lq=1

		x = x % (BigDecimal<max_size>(bigmath::constants::pi) * 2);

		BigDecimal<max_size> t = x;
		BigDecimal<max_size> a = 1;
		BigDecimal<max_size> sine = t;
		do{
			BigDecimal<max_size> mult = -x * x / ((a * 2 + 1) * (a * 2));
			t *= mult;
			sine += t;

			a++;
		} while (bigmath::abs(t) >= epsilon);

		return sine;
	}
	template <length_t max_size>
	BigDecimal<max_size> sin(BigDecimal<max_size> x) {
		return bigmath::sin<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> cos(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		// based on solution from: https://stackoverflow.com/questions/34703147/sine-function-without-any-library
		// and from: https://stackoverflow.com/questions/45169675/python-calculate-sine-cosine-with-a-precision-of-up-to-1-million-digits?noredirect=1&lq=1

		x = x % (BigDecimal<max_size>(bigmath::constants::pi) * 2);

		BigDecimal<max_size> t = 1;
		BigDecimal<max_size> a = 1;
		BigDecimal<max_size> cosine = t;
		do {
			BigDecimal<max_size> mult = -x * x / ((a * 2) * (a * 2 - 1));
			t *= mult;
			cosine += t;

			a++;
		} while (bigmath::abs(t) >= epsilon);

		return cosine;
	}
	template <length_t max_size>
	BigDecimal<max_size> cos(BigDecimal<max_size> x) {
		return bigmath::cos<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> tan(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		x = x % (BigDecimal<max_size>(bigmath::constants::pi) * 2);

		BigDecimal<max_size> t_sin = x;
		BigDecimal<max_size> t_cos = 1;

		BigDecimal<max_size> a = 1;

		BigDecimal<max_size> sine = t_sin;
		BigDecimal<max_size> cosine = t_cos;
		do {
			BigDecimal<max_size> mult = -x * x / (a * 2);

			t_sin *= (mult / (a * 2 + 1));
			sine += t_sin;

			t_cos *= (mult / (a * 2 - 1));
			cosine += t_cos;

			a++;
		} while (bigmath::abs(t_sin) >= epsilon || bigmath::abs(t_cos) >= epsilon);

		if (cosine < epsilon)
			throw bigspecials::complex_infinity;
		else
			return sine / cosine;
	}
	template <length_t max_size>
	BigDecimal<max_size> tan(BigDecimal<max_size> x) {
		return bigmath::tan<max_size>(x, bigmath::default_epsilon);
	}
	template <length_t max_size>
	BigDecimal<max_size> ctan(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		x = x % (BigDecimal<max_size>(bigmath::constants::pi) * 2);

		BigDecimal<max_size> t_sin = x;
		BigDecimal<max_size> t_cos = 1;

		BigDecimal<max_size> a = 1;

		BigDecimal<max_size> sine = t_sin;
		BigDecimal<max_size> cosine = t_cos;
		do {
			BigDecimal<max_size> mult = -x * x / (a * 2);

			t_sin *= (mult / (a * 2 + 1));
			sine += t_sin;

			t_cos *= (mult / (a * 2 - 1));
			cosine += t_cos;

			a++;
		} while (bigmath::abs(t_sin) >= epsilon || bigmath::abs(t_cos) >= epsilon);

		if (sine < epsilon)
			throw bigspecials::complex_infinity;
		else
			return cosine / sine;
	}
	template <length_t max_size>
	BigDecimal<max_size> ctan(BigDecimal<max_size> x) {
		return bigmath::ctan<max_size>(x, bigmath::default_epsilon);
	}
	template<length_t max_size>
	BigDecimal<max_size> arcsin(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		BigDecimal<max_size> t = x;
		BigDecimal<max_size> a = 1;

		BigDecimal<max_size> arcsine = t;
		
		do {
			BigDecimal<max_size> mult = ((x * x) * (a * 2 - 1) * (a * 2 - 1)) / ((a * 2) * (a * 2 + 1));
			t *= mult;
			arcsine += t;
		} while (bigmath::abs(t) >= epsilon);

		return arcsine;
	}
	template <length_t max_size>
	BigDecimal<max_size> arcsin(BigDecimal<max_size> x) {
		return bigmath::arcsin<max_size>(x, bigmath::default_epsilon);
	}
	template<length_t max_size>
	BigDecimal<max_size> arccos(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {
		BigDecimal<max_size> t = 1;
		BigDecimal<max_size> a = 1;

		BigDecimal<max_size> arcsine = t;

		do {
			BigDecimal<max_size> mult = ((x * x) * (a * 2 - 1) * (a * 2 - 1)) / ((a * 2) * (a * 2 + 1));
			t *= mult;
			arcsine += t;
		} while (bigmath::abs(t) >= epsilon);

		return ((bigmath::constants::pi / 2) - arcsine);
	}
	template <length_t max_size>
	BigDecimal<max_size> arccos(BigDecimal<max_size> x) {
		return bigmath::arccos<max_size>(x, bigmath::default_epsilon);
	}
	/*template <length_t max_size>
	BigDecimal<max_size> arctg(BigDecimal<max_size> x, BigDecimal<max_size> epsilon) {

	}*/
}