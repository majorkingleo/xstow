/**
 * @cpp_util_min_max.h
 * Es mag komisch erscheinen, dass hier ein eigenes min und max
 * implementiert wird. Der Grund liegt darin, dass std::min() und std::max()
 * nicht in jeder Lebenslage verwendet werden koennen.
 * So werden min() und max() in windows.h min anscheinen mit einem define definiert.
 * So duerfte man vor dem inkludieren von cpp_util.h weder die 
 * owil.h, noch windows.h inkludieren.
 * Ansonsten gibts unter Windows einen Compilerfehler.
 * VS9 bis VS-2019
 * @author Copyright (c) 2020 SSI Schaefer IT Solutions
 */

#ifndef _TOOLS_CPP_UTIL_MIN_MAX_H
#define _TOOLS_CPP_UTIL_MIN_MAX_H

namespace Tools {

template<typename T> T CppUtilMin( const T value_a, const T value_b ) {
	if( value_a < value_b ) {
		return value_a;
	}

	return value_b;
}

template<typename T> T CppUtilMax( const T value_a, const T value_b ) {
	if( value_a > value_b ) {
		return value_a;
	}

	return value_b;
}

} // /namespace Tools

#endif  /* _wamas_CPP_MIN_H */
