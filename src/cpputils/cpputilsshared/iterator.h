#ifndef TOOLS_iterator_h
#define TOOLS_iterator_h

/*
 * $Log$
 * Revision 1.1  2008/09/23 13:01:39  wamas
 * moved xml.h and c and iterator to cpputils
 *
 * Revision 1.3  2006/11/24 09:47:27  wamas
 * -Wshadow Warnings ausgebaut
 *
 * Revision 1.2  2006/11/22 22:48:30  wamas
 * Implementet postfix++ operator
 *
 * Revision 1.1.1.1  2006/03/17 19:49:15  wamas
 * own tools reponsitority
 *
 * Revision 1.1  2006/03/15 07:23:10  wamas
 * Brauchbares Tool hinzugefuegt, kommt aus xstow
 *
 *
 */


/**
   This template is so simple that it can be used without
   extending the effektive size of the binary. It simple has no effekt to
   the binary size cause it can completly removed by the optimizer.
*/

#include <iterator>

namespace Tools {

  /** template that returns the real type of a pointer */
  template<typename T> struct remove_pointer 
  { 
    typedef typename T::value_type result;
  };

  template<typename T> struct remove_pointer<T*>
  {
    typedef T result;
  };

  /** template that returns a pointer of a type */
  template<typename T> struct get_pointer 
  { 
    typedef T *result;
  };

  template<typename T> struct get_pointer<T*>
  { 
    typedef T *result;
  };

  template<typename T> struct pointer_traits
  {
    typedef typename T::pointer pointer;
    typedef typename T::value_type value_type;
  }; 

  template<typename T> struct pointer_traits<T*>
  {
    typedef T* pointer;
    typedef T  value_type;
  }; 

  
  /** 
      \class Iterator
      \brief iterator forward class template for pointers and smart pointers

      If a container holds pointers, or smart pointers the iterator access 
      on the real objects behind the iterators is tough.

      This class forwards the real objects, standing behind the pointers.

      By specialicing the pointer_traits template it's easy using this class
      with smart pointers that do not have the required typedefs.

      An example of the usage of this class can be found there: \link Iterator_Example.cc Iterator_Example.cc \endlink
  */
  template<typename iterator, typename _pointer = typename remove_pointer<iterator>::result > struct Iterator
  {
    typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<iterator>::difference_type   difference_type;

    typedef typename pointer_traits<_pointer>::value_type value_type;
    typedef typename pointer_traits<_pointer>::pointer    pointer;
    typedef typename pointer_traits<_pointer>::value_type &reference;

    iterator it; ///< the original iterator

    Iterator() {}

    Iterator( iterator it_ ) : it( it_ ) {}    

    bool operator!=( const iterator &i ) const { return it != i; }
    bool operator!=( const Iterator<iterator,_pointer> &i ) const { return it != i.it; }
    
    bool operator==( const iterator &i ) const { return it == i; }
    bool operator==( const Iterator<iterator,_pointer> &i ) const { return it == i.it; }

    Iterator<iterator,_pointer>& operator=( const iterator& i ) { return it = i; }
    Iterator<iterator,_pointer>& operator--() { --it; return *this; }
    Iterator<iterator,_pointer>& operator++() { ++it; return *this; }

    Iterator<iterator,_pointer> operator++(int c) { it.operator++(c); return *this; } 
    
    bool operator<( const Iterator<iterator,_pointer> &i ) const
    { 
      return it < i.it;
    }

    bool operator>( const Iterator<iterator,_pointer> &i ) const
    { 
      return it > i.it;
    }
    
    bool operator<=( const Iterator<iterator,_pointer> &i ) const
    { 
      return it <= i.it;
    }

    bool operator>=( const Iterator<iterator,_pointer> &i ) const
    { 
      return it >= i.it;
    }

    pointer operator->() { return *it; }
    reference operator*() { return *(*it); }
    reference operator[]( int i ) { return *(it[i]); }
    
    Iterator<iterator,_pointer>& operator+=( int i ) { it += i; return *this; }
    Iterator<iterator,_pointer>& operator-=( int i ) { it -= i; return *this; }

    /// returns the address of the current value
    pointer address() const { return *it; }
  };

  template<typename T, typename U> Iterator<T,U> operator+( const Iterator<T,U> &a, int i )
  {
    return Iterator<T,U>(a) + i;
  }

  template<typename T, typename U> Iterator<T,U> operator-( const Iterator<T,U> &a, int i )
  {
    return Iterator<T,U>(a) - i;
  }

}

#endif
