/**
 * Classes for typesave versions of sprintf() that are returning a std::string, or std::wstring
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 *
 * Examples:
 *    std::cout << format( "Hello %s, I have $05d$ in my pocket", "world", 5 ) << std::endl;
 *    std::cout << format( "Do not try this with printf: %s", 10101 ) << std::endl;
 *
 *	  wchar_t style:
 *
 *    std::wcout << wformat( L"Hello %s, I have $05d$ in my pocket", L"world", 5 ) << std::endl;
 *    std::wcout << wformat( L"Do not try this with printf: %s", 10101 ) << std::endl;
 *
 */

#ifndef _TOOLS_FORMAT_H
#define _TOOLS_FORMAT_H

#if __cplusplus < 201103
#include <format1.h>
#else
#include <format2.h>
#include <wformat2.h>
#endif

#endif  /* _TOOLS_FORMAT_H */
