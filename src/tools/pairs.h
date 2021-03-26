#ifndef TOOLS_H_pairs_h
#define TOOLS_H_pairs_h

#include <string>
#include <list>
#include <vector>

namespace Tools {

/// class handling text that should be ignored within a string
class Pairs
{
 private:

  /// structure presenting a single Pair
  struct Pair
  {
    std::string::size_type first;  ///< the first position
    std::string::size_type second; ///< the second position
  };
#if 0
  typedef std::list<Pair> Pair_list;
  typedef std::list<Pair>::iterator Pair_list_it;
#else
  typedef std::vector<Pair> Pair_list;
  typedef std::vector<Pair>::iterator Pair_list_it;
#endif

  Pair_list pairs;

  const std::string quote_sign;

 public:
  /// extract all pairs found in line
  Pairs( std::string line , std::string::size_type startpos = 0, const std::string & quote_sign = "\"");
  
  /// add a pair to the list
  void add( std::string::size_type first,
	    std::string::size_type second );

  void clear();  ///< clears the list

  /// checks if the position is between two pairs
  /** returns false if pos == std::string::npos */
  bool is_in_pair( std::string::size_type pos );
 
  /// extracts all pairs by itsself
  void extract( std::string line, std::string::size_type startpos = 0 );

 private:
  /// checks if the position is between the two pairs
  /** returns false if pos or one of the pairs == std::string::npos */
  bool is_in_pair( std::string::size_type pos,  ///< the current position
		   std::string::size_type pair1,
		   std::string::size_type pair2 ) const ;
};

}

#endif
