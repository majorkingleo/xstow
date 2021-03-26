/**
 * @file
 * @todo describe file content
 * @author Copyright (c) 2009 Salomon Automation GmbH
 */

#ifndef _Tools_VEC_UTILS_H
#define _Tools_VEC_UTILS_H

#include <vector>
#include <string>

namespace Tools {

template <class A, class B, class C> void addAll( std::vector<A> & a, const std::vector<B> & b, const C & funktor )
{
  for( typename std::vector<B>::const_iterator it = b.begin(); it != b.end(); it++ )
	a.push_back( funktor(*it) );
}

template <class A> void addAll( std::vector<const A*> & a, const std::vector<A> & b )
{
  for( typename std::vector<A>::const_iterator it = b.begin(); it != b.end(); it++ )
	a.push_back( &(*it) );
}

void addAll( std::vector<const char *> & a, const std::vector<std::string> & b );

} // /namespace wamas

#endif  /* _wamas_VEC_UTILS_H */
