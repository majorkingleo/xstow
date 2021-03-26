#include "pairs.h"

extern int ext_count;
extern int ext_num;

Tools::Pairs::Pairs( std::string line, std::string::size_type startpos, const std::string & quote_sign)
: pairs(),
  quote_sign(quote_sign)
{
  pairs.reserve(5);
  extract( line, startpos );
}

void Tools::Pairs::clear()
{
  pairs.clear();
}

void Tools::Pairs::add( std::string::size_type first,
		std::string::size_type second )
{
  Pair pair;

  pair.first = first;
  pair.second = second;

  pairs.push_back( pair );
}

bool Tools::Pairs::is_in_pair( std::string::size_type pos,
		       std::string::size_type pair1,
		       std::string::size_type pair2 ) const 
{
  if( pos == std::string::npos ||
      pair1 == std::string::npos ||
      pair2 ==  std::string::npos )
    return false;
  
  return ((pair1 < pos) && (pos < pair2) );
}

bool Tools::Pairs::is_in_pair( std::string::size_type pos )
{
  for( Pair_list_it it =  pairs.begin(); it != pairs.end(); it++ )
    {
      if( is_in_pair( pos,
		      (std::string::size_type) it->first,
		      (std::string::size_type) it->second ) )
	{
	  return true;
	}
    }
  return false;
}

void Tools::Pairs::extract( std::string line, std::string::size_type startpos )
{
  for( std::string::size_type pair_pos = startpos;;)
    {
      bool ignore;      

      std::string::size_type pair1;

      do
	{
	  pair1 = line.find( quote_sign, pair_pos + 1 );

	  ignore = false;

	  if( pair1 != std::string::npos && pair1 > 0 )
	    if( line[pair1 - 1] == '\\' )
	      {	    
		ignore = true;
		if( pair1 > 1 && line[pair1 - 2] == '\\' )
		  ignore = false;	
		else
		  pair_pos++;
	      }
	} while( ignore );
      
      if( pair1 != std::string::npos )
	{
	  pair_pos = pair1;

	  std::string::size_type pair2;

	  int factor = 1;

	  do
	    {
	      pair2 = line.find( quote_sign, pair1 + factor );

	      ignore = false;
	      
	      if( pair2 != std::string::npos && pair2 > 0 )
		if( line[pair2 - 1] == '\\' )
		  {	    
		    ignore = true;
		    if( pair2 > 1 && line[pair2 - 2] == '\\' )
		      ignore = false;	
		    else
		      factor++;
		  }
	    } while( ignore );


	  if( pair2 != std::string::npos )
	    {
	      add( pair1,pair2 );
	      pair_pos = pair2;
	    }
	}
      else
	break;
    }
}
