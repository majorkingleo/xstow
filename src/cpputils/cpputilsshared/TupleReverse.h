/**
 * Helper function for tuple handling
 *
 * SequenceHelper: expands a tuple to a parameter pack
 * tuple_reverse: returns the reverse type of a given tuple
 * reverseTuple( tuple ): reverses tuple values
 *
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */

#ifndef _TOOLS_TUPLEREVERSE_H
#define _TOOLS_TUPLEREVERSE_H

#if __cplusplus >= 201103

#include <tuple>
#include <algorithm>
#include <vector>

namespace Tools {

namespace TupleHelper {

namespace SequenceHelper {

// Tuple to parameter pack
// Original Author: Faheem Mitha https://stackoverflow.com/users/350713/faheem-mitha
// https://stackoverflow.com/questions/36612596/tuple-to-parameter-pack
//
// License: Creative Commons Attribution-ShareAlike (CC-BY-SA)
// https://www.ictrecht.nl/en/blog/what-is-the-license-status-of-stackoverflow-code-snippets

template<int ...>
struct seq { };

template<int N, int ...S>
struct gens : gens<N-1, N-1, S...> {};

template<int ...S>
struct gens<0, S...> {
	typedef seq<S...> type;

#if __GNUC__
	virtual ~gens() {} // this is only to avoid -Weffc++ warning
#endif
};

// end of Faheem Mitha's code

} // namespace SequenceHelper


// reverses the tuple types
// How do I reverse the order of element types in a tuple type?
// Original Author: Jonathan Wakely https://stackoverflow.com/users/981959/jonathan-wakely
// https://stackoverflow.com/a/17178399
//
// License: Creative Commons Attribution-ShareAlike (CC-BY-SA)
// https://www.ictrecht.nl/en/blog/what-is-the-license-status-of-stackoverflow-code-snippets

template <typename... Ts>
struct tuple_reverse;

template <>
struct tuple_reverse<std::tuple<>>
{
    using type = std::tuple<>;
};

template <typename T, typename... Ts>
struct tuple_reverse<std::tuple<T, Ts...>>
{
  using head = std::tuple<T>;
  using tail = typename tuple_reverse<std::tuple<Ts...>>::type;

  using type = decltype(std::tuple_cat(std::declval<tail>(), std::declval<head>()));
};

// end of Jonathan Wakely's code

// reverses the content of the tuple too
namespace TupleReverseImpl {


// Helper functions to recursively copy the source tuple to the target tuple
template <class ...Args>
void doCopyTypes( std::vector<void*>::size_type index,
                  std::vector<const void*> & content_source,
                  std::vector<void*> & content_target ) {}

template <class T, class ...Args>
void doCopyTypes( std::vector<void*>::size_type index,
                  std::vector<const void*> & content_source,
                  std::vector<void*> & content_target,
                  const T * t,
                  Args... args )
{
  // target and source vector do have the types in the same order
  // so we can cast here to the correct type, then copy it
  T * ptrSource = reinterpret_cast<T*>(const_cast<void*>(content_source.at(index)));
  T * ptrTarget = reinterpret_cast<T*>(content_target.at(index));

  *ptrTarget = *ptrSource;

  doCopyTypes( index+1, content_source, content_target, args... );
}

template <class Tuple, int ...S>
void copyTypes( std::vector<const void*> & content_source,
                std::vector<void*> & content_target,
                Tuple & tuple,
                SequenceHelper::seq<S...> )
{
  doCopyTypes( 0, content_source, content_target, &std::get<S>(tuple)... );
}

// Helper functions to fill a vector of pointers, to prepare copying

template <class V>
void dofillContent( V & content ) {}

template <class V, class T, class ...Args>
void dofillContent( V & content, T * t, Args... args )
{
  content.push_back( t );

  dofillContent( content, args... );
}

template <class V,class Tuple, int ...S>
void fill( V & content, Tuple & tuple, SequenceHelper::seq<S...> )
{
  dofillContent( content, &std::get<S>(tuple)... );
}

} // namespace TupleReverseImpl

/*
 * this reverses a tuple and its content
 *
 * std::tuple<int,bool,std::string> reverse = Tools::TupleHelper::reverseTuple( std::make_tuple( std::string( "eins", true, 42 ) ) );
 */
template <class Tuple>
typename tuple_reverse<Tuple>::type reverseTuple( const Tuple & tuple )
{
	// declare return type
	typedef typename tuple_reverse<Tuple>::type REVERSE_TUPLE_TYPE;
	REVERSE_TUPLE_TYPE return_tuple;

	// create source and target pointer vectors for the copy action
	std::vector<const void*> contentSource;
	std::vector<void*> contentTarget;

	TupleReverseImpl::fill( contentSource, tuple, typename SequenceHelper::gens<std::tuple_size<Tuple>::value>::type() );
	TupleReverseImpl::fill( contentTarget, return_tuple, typename SequenceHelper::gens<std::tuple_size<REVERSE_TUPLE_TYPE>::value>::type() );

	// to be in the same order as contentTarget
	std::reverse(contentTarget.begin(), contentTarget.end() );


	// now copy everything
	TupleReverseImpl::copyTypes( contentSource, contentTarget, tuple, typename SequenceHelper::gens<std::tuple_size<Tuple>::value>::type() );

	return return_tuple;
}

} // namespace TupleHelper

} // namespace Tools

#endif

#endif  /* _wamas_TUPLEREVERSE_H */
