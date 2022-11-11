/**
 * utf conversion utilities
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */
#include "utf8_util.h"
#include "utf8.h"
#include "stderr_exception.h"

#if __cplusplus >= 201103

namespace Tools {

bool Utf8Util::isUtf8( const std::string & text_ )
{
	std::string text = text_;

	std::string::iterator end_it = utf8::find_invalid(text.begin(), text.end());

	if( end_it != text.end() ) {
		return false;
	}

	return true;
}

bool Utf8Util::isAsciiOnly( const std::string & text )
{
	for( unsigned i = 0; i < text.size(); i++ ) {
		if( !isascii( text[i]) ) {
			return false;
		}
	}

	return true;
}

std::wstring Utf8Util::toWcharString16( const std::string & text )
{
	std::u16string str = utf8::utf8to16(text);
	std::wstring wstr;

	wstr.resize(str.size());

	for( unsigned i = 0; i < str.size(); i++ ) {
		wstr[i] = str[i];
	}

	return wstr;
}

std::wstring Utf8Util::toWcharString32( const std::string & text )
{
	std::u32string str = utf8::utf8to32(text);

	std::wstring wstr;

	wstr.resize(str.size());

	for( unsigned i = 0; i < str.size(); i++ ) {
		wstr[i] = str[i];
	}

	return wstr;
}

std::wstring Utf8Util::utf8toWString( const std::string & text )
{
	if( sizeof( wchar_t ) == sizeof( int16_t ) ) {
		return toWcharString16( text );
	} else if( sizeof( wchar_t ) == sizeof( int32_t ) ) {
		return toWcharString32( text );
	} else {
		throw STDERR_EXCEPTION( "unknown wchar size" );
	}
}


std::string Utf8Util::utf16toString( const std::wstring & text )
{
	std::u16string wstr;

	wstr.resize(text.size());

	for( unsigned i = 0; i < text.size(); i++ ) {
		wstr[i] = text[i];
	}

	std::string str = utf8::utf16to8(wstr);

	return str;
}

std::string Utf8Util::utf32toString( const std::wstring & text )
{
	std::u32string wstr;

	wstr.resize(text.size());

	for( unsigned i = 0; i < text.size(); i++ ) {
		wstr[i] = text[i];
	}

	std::string str = utf8::utf32to8(wstr);

	return str;
}

std::string Utf8Util::wStringToUtf8( const std::wstring & text )
{
	if( sizeof( wchar_t ) == sizeof( int16_t ) ) {
		return utf16toString( text );
	} else if( sizeof( wchar_t ) == sizeof( int32_t ) ) {
		return utf32toString( text );
	} else {
		throw STDERR_EXCEPTION( "unknown wchar size" );
	}
}

} // namespace Tools;

#endif

