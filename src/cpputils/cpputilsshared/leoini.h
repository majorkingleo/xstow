#ifndef TOOLS_Leo_Ini_h
#define TOOLS_Leo_Ini_h

#include <string>
#include <fstream>
#include <list>

#include "range.h"

namespace Tools {

namespace Leo
{

  /// a class for easy accessing ini files
  class Ini 
  {
  public:
    struct Element ///< This structure represents an ini-file entry 
    {
      std::string section; ///< the name of the current section
      std::string key;     ///< the name of the key
      std::string value;   ///< the value of the key
      
      typedef std::list<Element> element_list;  
      typedef element_list::iterator element_list_it;
      
      /// subelements
      /** if you read a whole section the subelements are stored in this list */
      mutable element_list elements; 
      
	struct EnumType {
	    enum ETYPE
	    {
		FIRST__,
		SECTION_LIST, ///< the element is a section list
		SECTION,      ///< the element is a whole section
		KEY,          ///< the element is a key
		UNDEF,        ///< not defined
		LAST__
	    };

	    virtual ~EnumType() {}
	};
      
      typedef Tools::EnumRange<EnumType> TYPE;

      TYPE type; ///< the type of the element
      
      Element( TYPE type_ = TYPE::UNDEF, std::string section_ = "", std::string key_ = "", std::string value_ = "" )
		  : section( section_ ),
		    key( key_ ),
		    value( value_ ),
		    elements(),
		    type( type_ )
	  {}
      
      virtual ~Element() {}

      /// adds a section to the section list, or adds a key to the section
      /** returns false on error */
      bool add( Element& element ); 
    };
    
  private:
    struct Line ///< structure representing a whole line
    {
      struct String  ///< structure representing a substring of a whole line
      {
	std::string::size_type pos;   ///< the cursor position
	std::string str;              ///< the string
	String() : pos(0), str() {}
	void clear() { pos = 0; str =""; } ///< clear the String
      };
      
      int number;      ///< the line number
      String comment;  ///< the possible comment 
      String tag;      ///< the stripped key, section,.. 
      std::string str; ///< the whole line
      
      Line() : number( 0 ), comment(), tag(), str() {}
      void clear()  ///< clears the Line
      { number = 0; comment.clear(); tag.clear(); str = ""; }
      std::string get_line(); ///< returns regenerated Line
    };
    
    typedef std::list<Line> line_list;
    typedef line_list::iterator line_list_it;
    
  public:
    struct MemElement : Element ///< class Representing an Element within the memory
    {
      Line line;   ///< the Line
      
      typedef std::list<MemElement> mem_element_list;
      typedef mem_element_list::iterator mem_element_list_it;
      
      mutable mem_element_list mem_elements; ///< list of subelements
      
      MemElement() : Element(), line(), mem_elements(){}
      
      MemElement( const Element& e )  ///< copy contructor for Elements
	: Element() ,
	  line(),
	  mem_elements()
      { *this = e; }
      
      MemElement & operator=( const Element& e ); ///< copy en Element
      void clear(); ///< clear it
      
      Element get_element(); ///< returns en Element
    };
  private:
    
    MemElement::mem_element_list elements; ///< list of MemElements
    line_list comments; ///< comment list
    
    int openmode;          ///< mode how we opened the file
    std::string file_name; ///< remember the filename
    std::fstream file;          ///< fstream for accessing the file
    bool is_open;          ///< true if a file is opened
    bool valid;            ///< true if system is valid
	bool file_readed;      ///< true if we already read the file
    
    bool eof_reached;      ///< true if we reached eof
    int line_number;       ///< the current line number
    
    bool changed;          ///< true if something has changed and we have to write it to the file

	std::string auto_global_section_name;

  public:
    Ini()
    : elements(),
      comments(),
      openmode(0),
      file_name(),
      file(),
      is_open( false ),
      valid( false ),
      file_readed(false),
      eof_reached( false ),
      line_number(0),
      changed( false ),
	  auto_global_section_name()
    {}

    Ini( std::string filename, int mode = std::ios::in | std::ios::out);   

    /// destructor
    /** if a file is still open the buffer will be flushed and the file closed */
    ~Ini();
    
	/// name of the autoglobal section
	/*** automatically create a section for key value pairs that are 
		 not assigned to a section and stores in a section named here
		 call this function before reading any data from the ini file
	*/
	void setAutoGlobalSectionName( const std::string & name )
	{
	  auto_global_section_name = name;
	}

    /// open a ini file
    /** returns false if we already opened a file or file opening failed */
    bool open( std::string filename, int mode = std::ios::in | std::ios::out );
    
    
    /// closes the file
    /** if data had changed the buffer will be flushed */
    void close(); 
    void flush();  ///< flushes the buffer
    
    /// read the ini file
    /** returns false if no file is opened and if the file is not already read */
    bool read(); 
    
    /// read an Element
    /** If the file wasn't read, it will be read now */
    bool read( Element& element );
    
    /// same as flush
    /** but return false if no file is opened, or the file was opened in readonly mode */
    bool write();
    
    /// writes an Element into the ini file
    /** returns false if no file is opened, 
	if file was opend in readonly mode,
	if the element is not a correct element.
	
	If you write a whole section, the old elements of the section 
	won't be destroyed. If you wan't to destroy a whole section,
	you have to erase it.
	
	If the you wan't to write a key into a section that does not exists,
	its section will be created automatically.
    */
    bool write( Element element );
    
    /// clears the file
    /** returns false if no file is opened, or file is open in readonly mode  */
    bool erase(); 
    
    /// erases one element 
    /** returns false if no file is open, the element is not correct or the element
	does not exist 
	
	If you wan't to erase a whole section or section list, only the elements
	you are listed within the list will be destroyed. If you wan't to
	erase a whole section you will have to read it first, that you have all
	elements of it, and then erase it by calling this function.
	
	If the last key of a section was detroyed the section will be destroyed.
    */
    bool erase( Element element ); 
    
    bool is_valid() const { return valid; } ///< returns true if the system is valid
    bool operator!() const { return !valid; } 
    
    void print_mem();

    friend bool operator == ( const Ini::MemElement& a, const Ini::MemElement& b );
  
    std::string get_file_name() const { return file_name; }

  private:
    Line read_line(); ///< reads one line
    std::string strip( std::string str, std::string what = " \t\0\n" ); ///< strips a string
    std::string::size_type find_comment( const std::string& str ); ///< finds a comment within a string
    
    /// finds the tag
    bool find_tag( std::string::size_type& start, std::string::size_type& end, const std::string& str ); 
    bool is_section( const std::string& str ); ///< detects a section
    bool is_key( const std::string& str ); ///< checks if the string is a key
    std::string extract_section_name( const std::string& str ); 
    std::string extract_key_name( const std::string& str );
    std::string extract_key_data( const std::string& str );
    
    bool is_good_element( Element& element ); ///< checks if the element is sane
    bool is_good_element_rec( Element& element ); ///< checks all elements
    
    void clear(); //clears the memory
    bool write_line( const Line& line, int last_line ); ///< write a line to file
    
    bool is_section_empty( MemElement& element );
  }; // class Ini
  
  bool operator == ( const Ini::MemElement& a, const Ini::MemElement& b );

} // namspace Leo


/// print an element
std::ostream& operator<<( std::ostream& out, const Leo::Ini::Element& e );

} // namespace Tools

#ifndef NO_STRSTREAM

#include "string_utils.h"

namespace Tools {

namespace Leo
{

  /// generates an indexed string
  template <class A, class B> std::string gen_index( const A a, const B b ) 
  {
    std::string s = x2s( a ) + '[' + x2s( b ) + ']';
    return s;
  }

  /// returns the index of a string
  template <class A> A get_index( const std::string& s )
  {
    std::string::size_type start = s.find( '[' );
    std::string::size_type end = s.find( ']', start );

    if( start == std::string::npos ||
	end == std::string::npos )
      s = "";
    else
      s = s.substr( start+1, start-end -1 );

    return s2x<A>(s);
  }
} // namespace Leo

#endif // NO_STRSTREAM

} // namespace Tools

#endif


