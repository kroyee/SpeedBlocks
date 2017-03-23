#ifdef __MINGW32__
#if __GNUC__ < 6 || (__GNUC__ == 6 && __GNUC_MINOR__ < 1)

#include <boost/lexical_cast.hpp>
#include "MingwConvert.h"

std::string to_string(int val) {
	return boost::lexical_cast<std::string>(val);
}
short stoi(std::string convert_str) {
	return boost::lexical_cast<short>(convert_str);
}

#endif
#endif