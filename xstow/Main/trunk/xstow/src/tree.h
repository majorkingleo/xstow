/*
 * $Log: tree.h,v $
 * Revision 1.2  2005/07/04 21:59:42  martin
 * added logging to all files
 *
 */
#ifndef tree_h
#define tree_h

#include "cppdir.h"
#include "ref.h"
#include "local_config.h"
#include <vector>
#include <iostream>

class Node
{
 public:

    struct EnumType
    {
	enum ETYPE
	{
	    FIRST__,
	    FILE,
	    DIR,
	    UNKNOWN,
	    LAST__
	};
    };
  
    typedef EnumRange<EnumType> TYPE;

  typedef std::vector< Ref<Node> > vec_node;

 protected:

  TYPE         type;
  CppDir::File file;

  Ref<Node> parent;
  Ref<vec_node> childs;

  bool path_is_hashed;
  mutable std::string full_path;

 public:

  Node( CppDir::File file, Ref<Node> parent = Ref<Node>() );

  bool scan( bool recursion = true );

  Ref<Node> getParent() const { return parent; }
  Ref<vec_node> getChilds() const { return childs; }

  bool hasParent() const { return parent.valid(); }
  bool hasChilds() const { return childs.valid(); }

  TYPE getType() const { return type; }
  bool isLink() const { return file.is_link(); }
  bool isDeadLink() const { return file.is_link() && !file; }

  std::string getFullPathName() const;
  std::string getName() const { return file.get_name(); }

  static std::string calcFullPathName( const Node &node );

  CppDir::File getFile() const { return file; }

  Ref<Node> getRoot();
};

class Tree
{
 public:

  struct Link
  {
    std::string origin;
    std::string target;
    std::string exec;

      struct EnumAction {
	  enum ETYPE
	  {
	      // don't change order; required for sorting
	      FIRST__  = 0,
	      REMOVE   = 1,
	      MKDIR    = 2,
	      SET_LINK = 3,
	      COPY     = 4,
	      COPY_DIR = 5,
	      EXEC     = 6,
	      LAST__
	  };
      };

      typedef EnumRange<EnumAction> ACTION;

      ACTION action;

    Link( const std::string &origin, const std::string &target, const std::string &exec = "" )
      : origin( origin ), target( target ), exec( exec ), action( ACTION::SET_LINK )
    {}
    Link() : action( ACTION::SET_LINK ) {}
  };

  typedef std::vector<Link> vec_link;

 private:

  Ref<Node> base;
  Ref<Node> target;
  Ref<Node> working;
  Ref<vec_link> links;

 public:

  Tree( const std::string &base, 
	const std::string &target,
	const std::string &working );

  bool scan();

  bool check()
    {
      links->clear();
      return check( target, base, links );
    }

  Ref<Node> getTree() const { return base; }

  bool exec_list();

 private:
  bool check( Ref<Node> target, Ref<Node> base, Ref<vec_link> links );
  bool checkDirs( Ref<Node> a, Ref<Node> b, std::string wpath, Ref<vec_link> links );

  void unify_links( Ref<vec_link> &links );
  
  bool compare_keep_dir( const std::string & a, const std::string &b );
  bool keep_dir( const std::string & a );
};

std::ostream& operator<<( std::ostream &out, Ref<Node> node );
std::ostream& operator<<( std::ostream& out, Ref<Tree::vec_link> links );
std::ostream& operator<<( std::ostream& out, const Tree::Link &link );

bool operator==( const Tree::Link &a, const Tree::Link &b );
bool operator<( const Tree::Link &a, const Tree::Link &b );

void make_relative_links( Ref<Tree::vec_link> links );

bool match_ignore( const std::string &file );
bool match_copy( const std::string &file );
bool match_ignore_file_in_dir( const std::string working_dir, const std::string &file );
bool match_copy_file_in_dir( const std::string working_dir, const std::string &file );
bool match_auto_traverse( const std::string &file );
bool match_stow_dirs( const std::string &file );
bool match_auto_stow_dirs( const std::string &file );
bool match_protect_dirs( const std::string &file );
bool match_auto_protect_dirs( const std::string &file );
bool match_protect_targets( const std::string &file );

#ifdef CAN_USE_INI
bool match_exec( const std::string &file, std::string &exec_string );
std::string command( std::string exec, const std::string &source, const std::string &target );
#endif

bool file_copy( std::string origin, std::string target );
bool dir_copy( std::string origin, std::string target );

#endif
