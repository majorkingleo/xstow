/*
 * $Log: tree.cpp,v $
 * Revision 1.5  2010/07/22 21:44:21  martin
 * Release of Verion 1.0.0
 *
 * Revision 1.4  2010/07/21 19:38:25  martin
 * gcc-4 Port
 *
 * Revision 1.3  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#include <stdlib.h>
#include <cstring>
#include "tree.h"
#include "debug.h"
#include "iterator.h"
#include "setup.h"
#include "nignore.h"

extern "C" {
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
}

#ifdef HAVE_FNMATCH_H
extern "C" {
#  include <fnmatch.h>
}
#endif

#ifdef HAVE_REGEX_H
extern "C" {
#  include <regex.h>
}
#endif


#include <algorithm>

#define OUT(level) DEBUG_OUT(level, MODULE::TREE)

namespace {

  std::string getName( const CppDir::File &file )
  {
    return CppDir::simplify_path( CppDir::concat_dir( file.get_path(), file.get_name() ) );
  }

}

Node::Node( CppDir::File file, Ref<Node> parent )
  : file( file ), parent( parent ), 
    //    fullpath( *this, std::string(), true ),
    path_is_hashed( false )
{
    BT;

  if( !file )
    type = TYPE::UNKNOWN;

  switch( file.get_type() )
    {
    case CppDir::EFILE::DIR:     type = TYPE::DIR; break;
    case CppDir::EFILE::FIFO:
    case CppDir::EFILE::CHAR:
    case CppDir::EFILE::BLOCK:
    case CppDir::EFILE::REGULAR: type = TYPE::FILE; break;

    default:
      type = TYPE::UNKNOWN;
    }

  if( type == TYPE::UNKNOWN )
    {
      if( file.is_link() )
	{
	  OUT(0)( "dead link %s. This does not harm, but can cause problems.\n"
		  "Handling dead link as normal file.\n", ::getName( file ) );
	  type = TYPE::FILE;
	}
      else
	OUT(0)( "unknown file type %s\n", ::getName( file ) );
    }
}

bool Node::scan( bool recursion )
{
    BT;

  if( childs.valid() )
    return true; // already scanned

  if( type == TYPE::FILE )
    return true;

  if( type == TYPE::UNKNOWN )
    {
      ERROR( "unknown file type %s\n", ::getName( file ) );
      return false;
    }

  if( !file.read_access() )
    {
      OUT(0)( "no read access at directory %s\n", ::getName( file ) );
      return true;
    }

  if( !file.exec_access() )
    {
      OUT(0)( "no exec access at directory %s\n", ::getName( file ) );
      return true;
    }

  CppDir::Directory dir( file );

  if( !dir )
    {
      ERROR( "expected that file %s is a directory, but reading failed\n", ::getName( file ) );
      eout( "reported error: %s\n", dir.error );
      return false;
    }

  CppDir::Directory::file_list fl = dir.get_files();

  for( CppDir::Directory::file_list_it it = fl.begin();
       it != fl.end(); ++it )
    {
      if( !childs.valid() )
	childs = new vec_node;

      Ref<Node> nn = new Node( *it, Ref<Node>( this, false ) );

      if( nn->file.get_name() == "." )
	continue;

      if( nn->file.get_name() == ".." )
	continue;
      
#ifdef CAN_USE_NIGNORE
      if( setup.nignores.size() )
      {
	  int found = -1;

	  for( unsigned i = 0; i < setup.nignores.size(); ++i )
	  {
	      if( setup.nignores[i]->match( Ref<Node>( this, false ) ) )
	      {
		  OUT(2) << setup.nignores[i] << '\n';
		  found = i;
		  break;
	      }
	  }
	  
	  if( found > -1 )
	      OUT(1)("directory %s passed nignore rule: %s\n", 
		     getFullPathName(), setup.nignores[found]->getName() );
	  else
	  {
	      OUT(1)("directory %s failed nignore check\n", getFullPathName() );
	      continue;
	  }
      }
#endif

      if( match_auto_protect_dirs( nn->file.get_name() ) )
      {
	  OUT(1)( "adding %s to protect-dir list\n", nn->file.get_name() );
	  setup.protect_dirs.dirs += ::getName( nn->file.get_name() );
      }

      DEBUG( OUT(2)( "xxxxx file %s type: %s match: %s\n",nn->file.get_name(), nn->getType(), match_ignore( nn->file.get_name() ) ) );
      
      if( nn->getType() == TYPE::FILE )
      {	      
	  if( match_ignore( nn->file.get_name() ) || 
	      match_ignore_file_in_dir( setup.current_package(), CppDir::concat_dir( getFullPathName(), nn->file.get_name() ) ) )
	  {
	      DEBUG( OUT(2)( "file %s ignored\n", CppDir::concat_dir( getFullPathName(), nn->file.get_name() ) ) );
	      continue;
	  }
	  
      }	  

      childs->push_back( nn );

      if( recursion )
	nn->scan();
    }

  return true;
}

std::string Node::calcFullPathName( const Node &node )
{
    BT;

  if( node.type == TYPE::UNKNOWN )
    return std::string();

  if( node.hasParent() )
    return CppDir::concat_dir( node.getParent()->getFullPathName(), node.file.get_name() );

  return CppDir::concat_dir( node.file.get_path(), node.file.get_name() );
}

std::string Node::getFullPathName() const
{
    BT;

  if( !path_is_hashed )
    full_path = calcFullPathName( *this );

  return full_path;
}

Ref<Node> Node::getRoot()
{
    BT;

  if( !hasParent() )
    return Ref<Node>(*this, false );

  return parent->getRoot();
}

Tree::Tree( const std::string &base_,
	    const std::string &target_,
	    const std::string &working_
	    )
{
    BT;

  base = new Node( CppDir::File( base_ ) );
  target = new Node( CppDir::File( target_ ) );
  working = new Node( CppDir::File( working_ ) );
  links = new vec_link;
}

bool Tree::scan()
{
    BT;

  DEBUG( OUT(3) << "target path name: " << target->getFullPathName() << std::endl );

  bool ret = base->scan();

  DEBUG( OUT(3) << "target path name: " << target->getFullPathName() << std::endl );

  return ret;
}

std::ostream& operator<<( std::ostream& out, Ref<Node> node )
{
    BT;

  if( !node )
    return out;

  out << node->getFullPathName();

  if( node->hasChilds() )
    {
      Ref<Node::vec_node> vn = node->getChilds();
      
      for( Node::vec_node::iterator it = vn->begin(); it != vn->end(); ++it )
	{
	  out << '\n';

	  out << *it;
	}
    }

  return out;
}

bool Tree::check( Ref<Node> target, Ref<Node> base, Ref<vec_link> links )
{
    BT;

  CppDir::File file( target->getFile() );

  DEBUG( OUT(3) << "target path name: " << target->getFullPathName() << std::endl );
  DEBUG( OUT(3) << "file.get_path(): " << file.get_path() << "\tfile.get_name(): " << file.get_name() << std::endl );

  if( !file || file.get_type() != CppDir::EFILE::DIR )
    {
      ERROR( "target dir %s is not a valid directory\n", getName( file ) );
      return false;
    }

  if( !file.read_access() )
    {
      OUT(0)( "no read access at directory %s\n", getName( file ) );

      if( setup.force() )
	OUT(0) << "ignoring cause XStow is in force mode\n";
      else
	return false;
    }

  if( !file.exec_access() )
    {

      OUT(0)( "no exec access at directory %s\n", getName( file ) );

      if( setup.force() )
	OUT(0) << "ignoring cause XStow is in force mode\n";
      else
	return false;
    }

  if( !file.write_access() )
    {
      OUT(0)( "no write access at directory %s\n", getName( file ) );

      if( setup.simulate() || setup.force() )
	{
	  if( setup.force() )
	    OUT(0) << "ignoring cause XStow is in force mode\n";
	  else
	    OUT(0) << "ignoring cause XStow is in simluation mode\n";
	}
      else
	return false;	
    }

  CppDir::Directory dir( file );

  if( !dir )
    {
      ERROR( "expected that file %s is a directory, but reading failed\n", getName( file ) );
      eout( "reported error: %s\n", dir.error );
      return false;
    }

  CppDir::Directory::file_list fl = dir.get_files();

  Ref<Node::vec_node> childs = base->getChilds();

  for( Iterator<Node::vec_node::iterator> it = childs->begin(); 
       it != childs->end(); ++it )
    {
      bool found = false;

      for( CppDir::Directory::file_list_it dit = fl.begin();
	   dit != fl.end(); ++dit )
	{
	  if( dit->get_name() == "." || dit->get_name() == ".." )
	    continue;

	  DEBUG( OUT(3)( "it->getName() == dit->get_name() => %s == %s\n", it->getName(), dit->get_name() ) );

	  if( it->getName() == dit->get_name() )
	    {
	      found = true;

	      if( dit->is_link() )
		{
		  OUT(1)( "checking if link %s is part of the own package\n", ::getName( *dit ) );
		  
		  DEBUG( OUT(3) << "dit->get_link_buf() => " << dit->get_link_buf() << '\n' );
		  DEBUG( OUT(3) << "target->getFullPathName() => " <<  target->getFullPathName() << '\n' );

		  DEBUG( OUT(2) << CppDir::get_full_path( dit->get_link_buf(), target->getFullPathName() ) 
			 << " == " 
			 << it->getFullPathName()
			 << '\n' 
		        );

		  if( CppDir::get_full_path( dit->get_link_buf(), target->getFullPathName() ) == it->getFullPathName() )
		    {
		      DEBUG( OUT(2) << "\t=> YES\n" );
		      OUT(1) <<  "link is part of the own package. Ignored it\n";

		      if( setup.unstow() )
			{
			  // remove link
			  
			  Link link;

			  link.action = Link::ACTION::REMOVE;
			  link.origin = CppDir::concat_dir( dit->get_path(), dit->get_name() );

			  links->push_back( link );
			}

		      break;
		    } 

		  DEBUG( OUT(2) << "\t=> NO\n" );

		}

#ifdef CAN_USE_INI
	      std::string s_exec;
	      if( it->getType() == Node::TYPE::FILE && match_exec( it->getName(), s_exec ) )
	      {
		  Link link( it->getFullPathName(), 
			     CppDir::concat_dir( target->getFullPathName(), it->getName() ),
			     s_exec );
		  link.action = Link::ACTION::EXEC;
		  links->push_back( link );

		  continue;
	      }
#endif	      

	      if( it->getType() == Node::TYPE::FILE && 
		  ( match_copy( it->getName() ) || match_copy_file_in_dir( setup.current_package(), 
									   it->getFullPathName() ) ) &&  
		    CppDir::EFILE::REGULAR )
	      {
		  if( !setup.unstow() )
		  {
		      // copy instead of linking
		      Link link( it->getFullPathName(), CppDir::concat_dir( target->getFullPathName(), it->getName() ) );		  
		      link.action = Link::ACTION::COPY;
		      links->push_back( link );
		  }		  

		  // a copied file won't be unstowed
		  continue;
	      } 

	      if( it->getType() == Node::TYPE::FILE || it->getType() == Node::TYPE::UNKNOWN )
		{
		    if( match_ignore( it->getName() ) || match_ignore_file_in_dir( base->getFullPathName(), it->getFullPathName() ) )
		    {
			DEBUG( OUT(2)( "file %s ignored\n", it->getFullPathName() ) );
			continue;
		    } else {
			
			ERROR( "file %s already exist\n", it->getFullPathName() );		  
			
			if( setup.force() )
			{
			    OUT(0) << "ignoring cause XStow is in force mode\n"
				"The file will be skipped";
			    continue;
			} else {			
			    return false;
			}
		    }
		  }

	      if( it->getType() == Node::TYPE::DIR )
		{		 
		  if( match_protect_dirs( ::getName( *dit ) ) )
		    {
			ERROR( "directory %s is protected\n", ::getName( *dit ) );

			if( setup.force() )
			{
			    OUT(0) <<  "skipped cause xstow is in force mode\n";
			    continue;

			} else {

			    return false;
			}
		    }

		  bool traverse_link = false;

		  if( dit->is_link() )
		    {
		      OUT(1)( "checking if link %s is a traversable link\n", ::getName( *dit ) );

		      for( unsigned int i = 0; i < setup.traversable_links.links.size(); ++i )
			{
			  DEBUG( OUT(3)( "traverse: %s == %s\n", ::getName( *dit ), setup.traversable_links.links[i] ) );
			  if( ::getName( *dit ) == setup.traversable_links.links[i] )
			    {
			      traverse_link = true;			    
			      break;
			    }
			}

		      if( !traverse_link )
			{
		      
			  OUT(1)( "checking if link %s is part of an existing package\n", ::getName( *dit ) );
			  
			  DEBUG( OUT(3) << "dit->get_link_buf() => " << dit->get_link_buf() << '\n' );
			  DEBUG( OUT(3) << "target->getFullPathName() => " <<  target->getFullPathName() << '\n' );
			  
			  std::string path = CppDir::get_full_path( dit->get_link_buf(), target->getFullPathName() );
			  
			  DEBUG( OUT(3) << "path: " << path << '\n' );
			  DEBUG( OUT(3) << "working: " << working->getFullPathName() << '\n' );
			  DEBUG( OUT(3) << "target->getRoot()->getFullPathName() => "
				 <<  target->getRoot()->getFullPathName() << '\n' );
			  
			  CppDir::File f( path );
			  
			  DEBUG( OUT(3) << "file: " << path << " is valid? " << (!f ? "NO\n" : "YES\n") );

/*
			  if( ( !f || path.find( working->getFullPathName() ) != 0 ) &&
			      ( path.find( target->getRoot()->getFullPathName() ) == 0 ) )

*/
			  if( ( !f || !CppDir::is_in_dir( path, working->getFullPathName() ) ) ||
			      ( CppDir::is_in_dir( path, target->getRoot()->getFullPathName() ) == 0 ) )
			    {			      
			     
			      // let's see if it is part of an other stow directory
			      bool other_stow = false;		  

			      OUT(1) << "checking if path is a stow dir\n";

			      if( match_stow_dirs( path ) )
				{
				  OUT(1) << "YES, package is part of other stow directory \n";
				  other_stow = true;
				} else {

				  OUT(1) << "cecking if path applies auto stow pattern\n";

				  // try auto stow dir adding
				  if( match_auto_stow_dirs( dit->get_link_buf() ) )
				    {
				      OUT(1) << "YES, auto stow dir succeded\n";
				      other_stow = true;
				    }
				}
			      
			      if( !other_stow )
				{
				  // check auto-if-target 
				  if( match_auto_traverse( dit->get_link_buf() ) )
				    {
				      DEBUG( OUT(1)( "added link %s to traversable links\n", ::getName( *dit ) ) );

				      setup.traversable_links.links += ( ::getName( *dit ) );
				      setup.handle_keep_dirs();
				      setup.handle_protect_dirs();

				      traverse_link = true;
				    }
				  else
				    {
				      OUT(0)( "path %s is not part of an xstow managed package\n", path );

				      if( setup.force() )
					{
					  OUT(0) << "ignoring cause XStow is in force mode\n"
					    "The file will be skipped";
					  continue;
					}
				      else
					return false;
				    }
				}
			    }
			  
			  if( !traverse_link )
			    {

			      OUT(1) <<  "trying to resolve dependencies\n";
			      
			      std::string wpath = CppDir::concat_dir( target->getFullPathName(), dit->get_name() );
			      
			      Link link;
			      
			      link.action = Link::ACTION::REMOVE;
			      link.origin = wpath;
			      
			      links->push_back( link );
			      
			      if( !checkDirs( *it.it, new Node( f ), wpath, links ) )
				{			  
				  OUT(0) << "resolving dependencies failed" << std::endl;
				  
				  if( setup.force() )
				    {
				      OUT(0) << "ignoring cause XStow is in force mode\n"
					"The file will be skipped";
				      continue;
				    }
				  else
				    return false;
				}
			    } // if( !traverse_link )

			} // if( !traverse_link )

		    }

		  if( dit->get_type() == CppDir::EFILE::DIR )
		    {
			if( match_copy( dit->get_name() ) )
			{
			    if( !setup.unstow() )
			    {
				// copy instead of linking
				Link link( it->getFullPathName(), CppDir::concat_dir( target->getFullPathName(), it->getName() ) );		  
				link.action = Link::ACTION::COPY_DIR;
				links->push_back( link );				
			    }

			    continue;
			}

		      // dive into subdirectory 
		      OUT(1)( "diving into subdirectory %s\n", it->getFullPathName() );
		      
		      if( traverse_link )
			{
			  if( !check( new Node( CppDir::File( 
                                                CppDir::get_full_path( dit->get_link_buf(), 
								       target->getFullPathName() ) ) 
						), 
						*it.it, links ) ) 
			    return false;
			}
		      else
			{
			  if( !check( new Node( *dit, target ), *it.it, links ) )
			    return false;
			}

		      break;
		    }
		}

	      break;
	    } // if 


	} // for 

      
      if( !found )
	{
	  if( !setup.unstow() )
	    {
	      if( match_ignore( it->getName() ) )
		{
		  OUT(1)( "%s %s ignored\n", 
			  it->getType() == Node::TYPE::FILE ? "File" : "Directory" , 
			  it->getFullPathName() );
		  continue;
		} 

		if( !match_protect_targets( CppDir::concat_dir( target->getFullPathName(), it->getName() )  ) )
		{
		    ERROR( "not allowed installing file %s there\n", CppDir::concat_dir( target->getFullPathName(), it->getName() ) );

		    if( setup.force() )
		    {
			OUT(0) << "Skipping cause xstow is in force mode\n";
		    }
		    else
		    {
			return false;
		    }
		}

		if( it->getType() == Node::TYPE::FILE && 
		    ( match_copy( it->getName() ) || match_copy_file_in_dir( setup.current_package(), 
									     it->getFullPathName() ) ) )
		{
		    // copy instead of linking
		    Link link( it->getFullPathName(), CppDir::concat_dir( target->getFullPathName(), it->getName() ) );
		    link.action = Link::ACTION::COPY;
		    links->push_back( link );

		} else if( it->getType() == Node::TYPE::DIR && match_copy( it->getName() ) ) {

		    Link link( it->getFullPathName(), CppDir::concat_dir( target->getFullPathName(), it->getName() ) );
		    link.action = Link::ACTION::COPY_DIR;
		    links->push_back( link );

		} else {

		    // add file to link list
		    links->push_back( Link( it->getFullPathName(), 
					    CppDir::concat_dir( target->getFullPathName(), it->getName() ) ) );
		}
	    }
	}
      
    } // for

  if( !links->empty() )
    OUT(1) << links << std::endl;
 
  return true;
}

std::ostream& operator<<( std::ostream& out, const Tree::Link &link )
{
    BT;

  switch( link.action )
    {
    case Tree::Link::ACTION::SET_LINK:
      out << "SET_LINK: " << link.origin << " => " << link.target;
      break;
      
    case Tree::Link::ACTION::REMOVE:
      out << "REMOVE: " << link.origin;
      break;
      
    case Tree::Link::ACTION::MKDIR:
      out << "MKDIR: " << link.target;
      break;

    case Tree::Link::ACTION::COPY:
      out << "COPY: " << link.origin;
      break;

    case Tree::Link::ACTION::COPY_DIR:
      out << "COPY: " << link.origin;
      break;

#ifdef CAN_USE_INI
    case Tree::Link::ACTION::EXEC:
      out << "EXEC: " << command( link.exec, link.origin, link.target );
      // out << "EXEC: " << link.origin << " => " << link.target << " => " << link.exec;
      break;
#endif

	case Tree::Link::ACTION::FIRST__:
	case Tree::Link::ACTION::LAST__:
		break;

    }
  return out;
}

std::ostream& operator<<( std::ostream& out, Ref<Tree::vec_link> links )
{
  bool first = true;

  for( Tree::vec_link::iterator it = links->begin(); it != links->end(); ++it )
    {
      if( !first )
	out << '\n';
      
      first = false;

      out << *it;
    }

  return out;
}

bool Tree::checkDirs( Ref<Node> a, Ref<Node> b, std::string wpath, Ref<vec_link> links)
{
    BT;

  DEBUG( OUT(3)( "wpath: %s\n", wpath ) );
  DEBUG( OUT(3)( "a->getFullPathName(): %s\n", a->getFullPathName() ) );
  DEBUG( OUT(3)( "b->getFullPathName(): %s\n", b->getFullPathName() ) );

  Link link;

  link.action = Link::ACTION::MKDIR;
  link.target = wpath;
  
  links->push_back( link );

  a->scan( false );
  b->scan( false );

  Ref<Node::vec_node> ac = a->getChilds();
  Ref<Node::vec_node> bc = b->getChilds();

  for( Iterator<Node::vec_node::iterator> ait = ac->begin(); ait != ac->end(); ++ait )
    {
      for( Iterator<Node::vec_node::iterator> bit = bc->begin(); bit != bc->end(); ++bit ) 
	{
	  DEBUG( OUT(3)( "ait->getName() == bit->getName() => %s == %s\n",  ait->getName(), bit->getName() ) );

	  bool failed = true;

	  if( ait->getName() == bit->getName() )
	    {

	      if( ait->getType() == Node::TYPE::DIR && bit->getType() == Node::TYPE::DIR )
	      {
		if( !checkDirs( *ait.it, *bit.it, 
				CppDir::concat_dir( wpath, ait->getName() ), links ) )
		  return false;
		
		Link link;
		
		link.action = Link::ACTION::MKDIR;
		link.target = CppDir::concat_dir( wpath, ait->getName() );

		links->push_back( link );

		failed = false;

	      } else if( ait->getType() == Node::TYPE::FILE && bit->getType() == Node::TYPE::FILE ) { 
			 		  
		  if( match_copy( ait->getName() ) || match_copy_file_in_dir( setup.current_package(), 
									      ait->getFullPathName() ) ) 
		  {
		      DEBUG( OUT(2)( "file %s will be copied\n", ait->getFullPathName() ) );
		      failed = false;

		  } else if( match_ignore( ait->getName() ) || 
			     match_ignore_file_in_dir( base->getFullPathName(), ait->getFullPathName() ) ) {

		      DEBUG( OUT(2)( "file %s will be ignored\n", ait->getFullPathName() ) );
		      failed = false;
		  }

	      } 

	      if( failed ) {

		OUT(0)( "File %s and file %s are not of the same type\n", 
			ait->getFullPathName(), bit->getFullPathName() );

		return false;
	      }
	    }
	  }      
    }

  // add links from the other package

  for( Iterator<Node::vec_node::iterator> bit = bc->begin(); bit != bc->end(); ++bit )
    {
      bool found = false;

      for( Iterator<Node::vec_node::iterator> ait = ac->begin(); ait != ac->end(); ++ait )
	{
	  if( ait->getName() == bit->getName() )
	    {
	      found = true;
	      break;
	    }
	}

      if( !found )
	{
          DEBUG( OUT(3) << "add link: " << bit->getFullPathName() << " => " << CppDir::concat_dir( wpath, bit->getName() )  << '\n' );
	  links->push_back( Link( bit->getFullPathName(), CppDir::concat_dir( wpath, bit->getName() ) ) );
	} 	    
    }

  return true;
}

void Tree::unify_links( Ref<vec_link> &l )
{
    BT;

    Ref<vec_link> ll = new vec_link;

    for( vec_link::iterator it = l->begin(); it != l->end(); ++it )
    {
	bool found = false;

	for( vec_link::iterator vit = ll->begin(); vit != ll->end(); ++vit )
	{
	    if( *it == *vit )
	    {
		found = true;
		break;
	    }
	}
	
	if( !found )
	    ll->push_back( *it );
    }

    l = ll;
} 

bool operator==( const Tree::Link &a, const Tree::Link &b )
{
  if( a.action != b.action )
    return false;

  if( a.origin != b.origin )
    return false;

  if( a.target != b.target )
    return false;

  return true;
}

bool operator<( const Tree::Link &a, const Tree::Link &b )
{
  if( a.action < b.action )
    return true;
/*
  if( !a.target.empty() && !b.target.empty() )
    if( a.target < b.target )
      return true;
  
  if( !a.origin.empty() && !b.origin.empty() )
    if( a.origin < b.origin )
      return true;
*/
  return false;
}

bool Tree::exec_list()
{
    BT;

  unify_links( links );

  DEBUG( OUT(3) << '\n' << links << '\n' );
  
  if( !setup.links.absolute_paths() )
    make_relative_links( links );

  DEBUG( OUT(3) << '\n' << links << '\n' );
  
  vout[1] << links << '\n';

  if( setup.simulate() || setup.no_changes() )
    return true;

  std::list<Link> l;

  for( vec_link::iterator it = links->begin(); it != links->end(); ++it )
    l.push_back( *it );


  for( std::list<Link>::iterator it = l.begin(); it != l.end(); ++it )
    {
      DEBUG( OUT(1) << "Executing: " << *it << '\n' );
      vout[1] << "Executing: " << *it << '\n';

      switch( it->action )
	{
	case Link::ACTION::REMOVE:
	  {
	    CppDir::File file( it->origin );
	    
	    if( !file )
	      {
		ERROR( "cannot remove file %s\nFile does not exist\n", it->origin );
		return false;
	      }

	    if( !file.is_link() && file.get_type() == CppDir::EFILE::DIR )
	      {
		// remove directories
		
		// check if directory is in keep list               
		bool keep = keep_dir( it->origin );

		if( !keep )
		  {
		    errno = 0;
		    
		    if( rmdir( it->origin.c_str() ) !=  0 )
		      {
			ERROR( "unlink: errno: %d\n", errno );
			eout << std::strerror( errno ) << '\n';
			eout( "file: %s\n", it->origin );
			return false;
		      }
		  }
	      }
	    else
	      {
		// link or file
		
		errno = 0;
		
		if( unlink( it->origin.c_str() ) != 0 )
		  {
		    ERROR( "unlink: errno: %d\n", errno );
		    eout << std::strerror( errno ) << '\n';
		    return false;
		  }
	      }

	    if( setup.unstow() )
	      {
		// remove empty directories

		// check if directory is in keep list                
		bool keep = keep_dir( it->origin );
                
		if( !keep )
		  {

		    CppDir::Directory dir( CppDir::File( file.get_path() ) );
		    
		    if( dir.is_valid() )
		      {
			CppDir::Directory::file_list files = dir.get_files();
			
			if( std::distance( files.begin(), files.end() ) == 2 )
			  {
			    Link link;
			    
			    link.action = Link::ACTION::REMOVE;
			    link.origin = file.get_path();
			    
			    l.push_back( link );
			  }
		      }
		  } // if( !keep )
	      } // if( setup.unstow() )

	  } // Link::Remove
	  break;

	case Link::ACTION::SET_LINK:
	  {
	    errno = 0;

	    if( symlink( it->origin.c_str(), it->target.c_str() ) != 0 )
	      {
		ERROR( "symlink: errno: %d\n", errno );
		OUT(1) << "origin: " << it->origin << '\n';
		OUT(1) << "target: " << it->target << '\n';
		eout << std::strerror( errno ) << '\n';
		
		return false;
	      }
	  }
	break;

	case Link::ACTION::MKDIR:
	  {
	    errno = 0;

	    if( mkdir( it->target.c_str(), 0755 ) != 0 )
	      {
		ERROR( "mkdir: %s\nerrno: %d\n", it->target.c_str(), errno );
		eout << std::strerror( errno ) << '\n';

		return false;
	      }
	  }
	break;

	case Link::ACTION::COPY:
	  {
	      errno = 0;

	      if( !file_copy( it->origin, it->target ) )
	      {
		  ERROR( "file_copy: %s\nerrno: %d\n", it->target.c_str(), errno );
		  eout << std::strerror( errno ) << '\n';
	      }
	  }
	break;

	case Link::ACTION::COPY_DIR:
	  {
	      errno = 0;

	      if( !dir_copy( it->origin, it->target ) )
	      {
		  ERROR( "dir_copy: %s\nerrno: %d\n", it->target.c_str(), errno );
		  eout << std::strerror( errno ) << '\n';
	      }

	  }
	break;

#ifdef CAN_USE_INI
       case Link::ACTION::EXEC:
         {
	     std::string com = command( it->exec, it->origin, it->target );

	     int ret = system( com.c_str() );

	     if( ret == -1 )
		 ERROR( "system() failed\n" );		 
	     if( ret != 0 )
		 ERROR( "exit status of command %s was %d\n", com, ret );
         }
	 break;
#endif

	case Link::ACTION::FIRST__:
	case Link::ACTION::LAST__:
		break;

	} // switch
    } // for

  return true;
}

/**
 * compares 2 directories.
 * /usr/local == /usr/local => true true
 * /usr/local == local => true
 * /usr/local == usr => false
 * @param a
 * @param b
 * @return 
 */
bool Tree::compare_keep_dir( const std::string & a, const std::string &b )
{
    if( b.empty() )
        return false;
    
    if( b[0] == '/' ) {
        return a == b;
    }
    
    if( b.size() > a.size() )
        return false;
    
    // /usr/local/bin with local/bin
    
    // a_part1 is now /usr/
    std::string a_part1 = a.substr(0,a.size()-b.size());
    
    // a_part2 is now local/bin
    std::string a_part2 = a.substr(a.size()-b.size());
    
    if( a_part1[a_part1.size()-1] != '/' ) {
        return false;
    }
    
    return a_part2 == b;
}

bool Tree::keep_dir(const std::string & a) 
{
    for (unsigned int i = 0; i < setup.keep_dirs.dirs.size(); ++i) {

        bool matches = compare_keep_dir(a,  setup.keep_dirs.dirs[i]);       
        
        std::string str_inode_a;
        std::string str_inode_b;

        /*
         * if strcmp doesn't find a way, try comparing the inode numbers
         * this works if the path points to the same directory by using
         * different ways.         
         */
        if (!matches) 
        {
            CppDir::File dir_a(a);
            CppDir::File dir_b(setup.keep_dirs.dirs[i]);

            if (dir_a.is_valid() && dir_b.is_valid()) {
                long inode_a = dir_a.get_inode();
                long inode_b = dir_b.get_inode();

                str_inode_a = format("(%d)", inode_a);
                str_inode_b = format("(%d)", inode_b);

                if (inode_a != 0) {
                    if (inode_a == inode_b) {
                        matches = true;
                    }
                }
            }
        }
            
        std::string comp = " != ";
        if (matches) {
            comp = " == ";
        }            
        
        OUT(2) << "keepdir: " << a << str_inode_a << comp << setup.keep_dirs.dirs[i] << str_inode_b << '\n';
        
        if (matches) {
            return true;
        }        
    }
    
    return false;
}

void make_relative_links( Ref<Tree::vec_link> links )
{
    BT;

  for( Tree::vec_link::iterator it = links->begin(); it != links->end(); ++it )
    {
      if( it->action == Tree::Link::ACTION::SET_LINK )
	{
	  std::string path, name;

	  CppDir::split_name( it->target, path, name );

	  it->origin = CppDir::make_relative( path, it->origin );
	  //	  it->origin = it->origin + '/' + name;
	  //	  it->origin = CppDir::make_relative( it->target, it->origin );
	}
    }
}

bool match_ignore( const std::string &file )
{
    BT;

#ifdef HAVE_REGEX_H
  for( unsigned int i = 0; i < setup.matches.ignore_regex.size(); ++i )
    {
      if( regexec( &setup.matches.ignore_regex.regex(i),
		   file.c_str(), 0, 0, 0 ) == 0 )
	return true;
    }
#endif

#ifdef HAVE_FNMATCH_H
  for( unsigned int i = 0; i < setup.matches.ignore.size(); ++i )
    {
      if( fnmatch( setup.matches.ignore[i].c_str(), file.c_str(), FNM_PATHNAME ) == 0 )
	{
	  return true;
	}
      else
	{
	  DEBUG( OUT(3)( "fnmatch failed: '%s' != '%s'",  setup.matches.ignore[i], file ) );
	}
    }
#endif

  return false;
}

bool match_copy( const std::string &file )
{
    BT;

#ifdef HAVE_REGEX_H
  for( unsigned int i = 0; i < setup.matches.copy_regex.size(); ++i )
    {
      if( regexec( &setup.matches.copy_regex.regex(i),
		   file.c_str(), 0, 0, 0 ) == 0 )
	return true;
    }
#endif

#ifdef HAVE_FNMATCH_H
  for( unsigned int i = 0; i < setup.matches.copy.size(); ++i )
    {
      if( fnmatch( setup.matches.copy[i].c_str(), file.c_str(), FNM_PATHNAME ) == 0 )
	return true;
    }
#endif

  return false;
}

bool match_auto_traverse( const std::string &file )
{
    BT;

#ifdef HAVE_REGEX_H
  for( unsigned int i = 0; i < setup.traversable_links.add_if_targets_regex.size(); ++i )
    {
      if( regexec( &setup.traversable_links.add_if_targets_regex.regex(i),
		   file.c_str(), 0, 0, 0 ) == 0 )
	return true;
    }
#endif

#ifdef HAVE_FNMATCH_H
  for( unsigned int i = 0; i < setup.traversable_links.add_if_targets.size(); ++i )
    {
      if( fnmatch( setup.traversable_links.add_if_targets[i].c_str(), file.c_str(), 
		   FNM_PATHNAME | FNM_PERIOD ) == 0 )
	return true;
    }
#endif

  return false;
}

bool match_stow_dirs( const std::string &file )
{
    BT;

  for( unsigned int aa=0; aa < setup.stow_dirs.dirs.size(); ++aa )
    {
      if( CppDir::is_in_dir( file, setup.stow_dirs.dirs[aa] ) )
	return true;
    }

  return false;
}

bool match_auto_stow_dirs( const std::string &file )
{
    BT;

    DEBUG( OUT(3) << "file: " << file << '\n' );

#ifdef HAVE_REGEX_H
  for( unsigned int i = 0; i < setup.stow_dirs.auto_add_dirs_regex.size(); ++i )
    {
      if( regexec( &setup.stow_dirs.auto_add_dirs_regex.regex(i),
		   file.c_str(), 0, 0, 0 ) == 0 )
	return true;
    }
#endif

#ifdef HAVE_FNMATCH_H
  for( unsigned int i = 0; i < setup.stow_dirs.auto_add_dirs.size(); ++i )
    {
      if( fnmatch(  setup.stow_dirs.auto_add_dirs[i].c_str(), file.c_str(), 
		    FNM_PATHNAME | FNM_PERIOD ) == 0 ) 
        return true;
    }                 
#endif

  return false;
}

bool match_protect_dirs( const std::string &file )
{
    BT;

    for( unsigned int aa=0; aa < setup.protect_dirs.dirs.size(); ++aa )
    {
	if( CppDir::is_in_dir( file, setup.protect_dirs.dirs[aa] ) )
	    return true;
    }

  return false;
}

bool match_auto_protect_dirs( const std::string &file )
{
    BT;

    DEBUG( OUT(3) << "file: " << file << '\n' );

#ifdef HAVE_REGEX_H
  for( unsigned int i = 0; i < setup.protect_dirs.auto_add_dirs_regex.size(); ++i )
    {
      if( regexec( &setup.protect_dirs.auto_add_dirs_regex.regex(i),
		   file.c_str(), 0, 0, 0 ) == 0 )
	return true;
    }
#endif

#ifdef HAVE_FNMATCH_H
  for( unsigned int i = 0; i < setup.protect_dirs.auto_add_dirs.size(); ++i )
    {
      if( fnmatch(  setup.protect_dirs.auto_add_dirs[i].c_str(), file.c_str(), 
		    FNM_PATHNAME | FNM_PERIOD ) == 0 ) 
        return true;
    }                 
#endif

  return false;
}

bool match_protect_targets( const std::string &file )
{
    BT;

    if( setup.protect_dirs.targets.size() == 0 )
	return true; // value not set

  for( unsigned int aa=0; aa < setup.protect_dirs.targets.size(); ++aa )
    {
      if( CppDir::is_in_dir( file, setup.protect_dirs.targets[aa] ) )
	return true;
    }

  return false;
}

#ifdef CAN_USE_INI
bool match_exec( const std::string &file, std::string &exec )
{
    BT;

    for( unsigned i = 0; i < setup.exec.size(); ++i )
    {
	if( setup.exec[i].match() == file )
	{
	    if( setup.unstow() && !setup.exec[i].exec_unstow().empty() )
		exec = setup.exec[i].exec_unstow();
	    else
		exec = setup.exec[i].exec();

	    return true;
	}

#ifdef HAVE_FNMATCH_H
	if( fnmatch(  setup.exec[i].match().c_str(), file.c_str(), 
		      FNM_PATHNAME | FNM_PERIOD ) == 0 )
	{
	    exec = setup.exec[i].exec();
	    return true;
	}
#endif
    }

    return false;
}

std::string command( std::string exec, const std::string &source, const std::string &target )
{
    exec = substitude( exec, "%s", '\"' + source + '\"' );
    exec = substitude( exec, "%t", '\"' + target + '\"' );

    return exec;
}
#endif

bool match_ignore_file_in_dir( const std::string working_dir, const std::string &file )
{
    BT;

    DEBUG( OUT(4) << "working dir: " << working_dir << " file: " << file << '\n' );

    for( unsigned i = 0; i < setup.matches.ignore_file_in_dir.size(); i++ )
    {
	std::string f = CppDir::concat_dir( working_dir, setup.matches.ignore_file_in_dir[i] );

	if( f == file )
	    return true;
    }

    return false;
}

bool match_copy_file_in_dir( const std::string working_dir, const std::string &file )
{
    BT;

    DEBUG( OUT(4) << "working dir: " << working_dir << " file: " << file << '\n' );

    for( unsigned i = 0; i < setup.matches.copy_file_in_dir.size(); i++ )
    {
	std::string f = CppDir::concat_dir( working_dir, setup.matches.copy_file_in_dir[i] );

	if( f == file )
	    return true;
    }

    return false;
}

