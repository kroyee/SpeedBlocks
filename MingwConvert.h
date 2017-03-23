#ifndef _MINGWCONVERT_H
#define _MINGWCONVERT_H

#ifdef __MINGW32__
#if __GNUC__ < 6 || (__GNUC__ == 6 && __GNUC_MINOR__ < 1)
std::string to_string(int);
short stoi(std::string);
#endif
#endif

#endif