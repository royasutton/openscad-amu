/***************************************************************************//**

  \file   openscad_dif_util.cpp

  \author Roy Allen Sutton
  \date   2016-2019

  \copyright

    This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
    (https://royasutton.github.io/openscad-amu)).

    openscad-amu is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    openscad-amu is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    [GNU General Public License] (https://www.gnu.org/licenses/gpl.html)
    for more details.

    You should have received a copy of the GNU General Public License
    along with openscad-amu.  If not, see <http://www.gnu.org/licenses/>.

  \brief
    OpenSCAD Doxygen input filter utility classes source.

  \todo support variable expansion with word selection (or introduce a
        framework for text functions). See expand(). Consider the forms:
        ${var[n]}, ${var[1,2,3,7-10]}, ${#var[0]}, ${#var[*]},
        ${var[@]:0}, ${var[@]:1}, ${var[@]:1:2}

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_util.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// ODIF::env_var
////////////////////////////////////////////////////////////////////////////////

ODIF::env_var::env_var( const string& p, const string& s,
                        const string& e, const string& ep,
                        const string& es,
                        const size_t& epl, const size_t& esl,
                        bool r, const string& rm)
{
  set_prefix( p );
  set_suffix( s );
  set_regexp( e );
  set_escape_prefix( ep );
  set_escape_suffix( es );
  set_escape_prefix_length( epl );
  set_escape_suffix_length( esl );
  set_report( r );
  set_report_message( rm );
}

ODIF::env_var::~env_var( void )
{
  clear();
}

string
ODIF::env_var::expand(const string& v)
{
  return ( expand(v, report, report_message) );
}

string
ODIF::env_var::expand(const string& v, bool r)
{
  return ( expand(v, r, report_message) );
}

string
ODIF::env_var::expand(const string& v, bool r, const string& rm)
{
  string mn;

  // remove prefix and suffix strings from named variable:
  // example: "${" + VAR + "}" --> VAR
  mn = v.substr( prefix.length()
               , v.length()-(prefix.length()+suffix.length()) );

  string rs;

  if ( exists( mn ) ) {
    rs = map[ mn ];
  } else {
    // avoid future expansion, use 'mn' rather than 'v'
    // if ( r ) rs = mn + "=" + rm;
    if ( r ) rs = rm;
  }

  return( rs );
}

string
ODIF::env_var::expand_text(const string& t)
{
  return ( expand_text(t, report, report_message) );
}

string
ODIF::env_var::expand_text(const string& t, bool r)
{
  return ( expand_text(t, r, report_message) );
}

string
ODIF::env_var::expand_text(const string& t, bool r, const string& rm)
{
  string text = t;
  string result;

  while ( expand_textP( text, r, rm, result) )
  {
    text = result;
    result.clear();
  }

  return( result );
}

size_t
ODIF::env_var::expand_textP(const string& t, bool r, const string& rm, string& et)
{
  using namespace boost;

  //              :---------------------------[0]------------------------------:
  //                   [1]             [2]            [3]             [4]
  string re_mp = "("+regexp+")|("+escape_prefix+")("+regexp+")("+escape_suffix+")";

  size_t match_count = 0;

  string::const_iterator start = t.begin();
  string::const_iterator end = t.end();
  match_results<string::const_iterator> match;
  regex expression( re_mp, regex::normal );
  match_flag_type flags = match_posix;

  while( regex_search(start, end, match, expression, flags) )
  {
    // unescaped: replace variable with its value
    if ( match[1].matched ) {
      const string vn( match[1].first, match[1].second );
      const string vv( expand(vn, r, rm) );

      // text to match
      et.append(start, match[1].first);

      // variable value
      et.append( vv );

      match_count++;
    }

    // escaped: remove matched prefix, copy variable, remove matched suffix
    if ( match[3].matched ) {
      if ( match[2].matched ) {
        // prefix matched: text to start of prefix (skipping prefix)
        et.append(start, match[2].first);
      } else {
        // prefix not matched: text to start of escaped variable
        et.append(start, match[3].first);
      }

      // copy variable (now unescaped)
      et.append(match[3].first, match[3].second);

      // skip suffix (matched or not)

      match_count++;
    }

    // update search start position
    start = match[0].second;
  }

  // last match position to end of text
  et.append(start, end);

  return( match_count );
}

void
ODIF::env_var::dump(void) {
  cout << endl
       << "(var map begin)" << endl;

  for ( std::map<string, string>::iterator  it=map.begin();
                                            it!=map.end();
                                          ++it )
  {
    cout << it->first << "=" << it->second << endl;
  }

  cout << "(var map end)" << endl;
}


////////////////////////////////////////////////////////////////////////////////
// ODIF::func_args
////////////////////////////////////////////////////////////////////////////////

ODIF::func_args::func_args(const string& p)
{
  clear();
  set_prefix( p );
}

ODIF::func_args::~func_args(void)
{
  clear();
}

/***************************************************************************//**

  \details

    If no argument name has been set prior, via \ref set_next_name,
    then a positional argument name is generated by appending the
    positional argument count to the default positional argument
    prefix.

*******************************************************************************/
void
ODIF::func_args::store(const std::string& v)
{
  bool p;

  // if no argument name has been specified, use (positional naming)
  // incrementing arguments prefixed with 'pos_prefix' + argument number
  if ( next_name.length() == 0 ) {
    next_name = pos_prefix + to_string( pos_count++ );
    p = true;
  } else {
    p = false;
  }

  arg_term ta;

  ta.name       = next_name;
  ta.value      = v;
  ta.positional = p;

  argv.push_back( ta );

  // next_name consumed, clear it.
  next_name.clear();
}

void
ODIF::func_args::store(const string& n, const string& v)
{
  set_next_name(n);
  store(v);
}

size_t
ODIF::func_args::size(bool n, bool p)
{
  size_t tc=0, nc=0, pc=0;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if ( it->positional ) pc++;
    else                  nc++;
  }

  if ( n ) tc+= nc;
  if ( p ) tc+= pc;

  return ( tc );
}

bool
ODIF::func_args::exists(const string& n)
{
  bool found = false;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end() && !found;
                                  ++it )
  {
    if ( it->name.compare( n )==0 )
      found = true;
  }

  return ( found );
}

string
ODIF::func_args::arg(const size_t n)
{
  string rs;

  if ( n < size() )
    rs = argv[ n ].value;

  return ( rs );
}

string
ODIF::func_args::arg(const string& n, bool& found)
{
  string rs;

  found = false;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end() && !found;
                                  ++it )
  {
    if ( it->name.compare( n )==0 )
    {
      rs = it->value;
      found = true;
    }
  }

  return ( rs );
}

string
ODIF::func_args::arg_firstof(const string& dv,
                             const string& n1, const string& n2,
                             const string& n3, const string& n4,
                             const string& n5, const string& n6)

{
  string rs;

  bool found;
                                 rs = arg( n1, found );
  if ( !found && !n2.empty() ) { rs = arg( n2, found );
  if ( !found && !n3.empty() ) { rs = arg( n3, found );
  if ( !found && !n4.empty() ) { rs = arg( n4, found );
  if ( !found && !n5.empty() ) { rs = arg( n5, found );
  if ( !found && !n6.empty() ) { rs = arg( n6, found );
  }}}}}

  // default
  if ( !found )
    rs = dv;

  return( rs );
}

vector<string>
ODIF::func_args::values_v(bool n, bool p)
{
  vector<string> rv;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if (   it->positional && p )  rv.push_back( it->value );
    if ( ! it->positional && n )  rv.push_back( it->value );
  }

  return ( rv );
}

string
ODIF::func_args::values_str(bool n, bool p)
{
  string rv;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->value;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->value;
    }
  }

  return ( rv );
}

vector<string>
ODIF::func_args::names_v(bool n, bool p)
{
  vector<string> rv;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if (   it->positional && p )  rv.push_back( it->name );
    if ( ! it->positional && n )  rv.push_back( it->name );
  }

  return ( rv );
}

string
ODIF::func_args::names_str(bool n, bool p)
{
  string rv;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->name;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->name;
    }
  }

  return ( rv );
}

string
ODIF::func_args::pairs_str(bool n, bool p, string a)
{
  string rv;

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                  ++it )
  {
    if (   it->positional && p ) {
      if ( rv.length() ) rv += " ";
      rv += it->name + a + it->value;
    }
    if ( ! it->positional && n ) {
      if ( rv.length() ) rv += " ";
      rv += it->name + a + it->value;
    }
  }

  return ( rv );
}

void
ODIF::func_args::dump(void)
{
  cout << "(";

  for ( vector<arg_term>::iterator  it=argv.begin();
                                    it!=argv.end();
                                        )
  {
    cout << it->name << "=" << it->value;

    ++it;
    if ( it!=argv.end() ) cout << " ";
  }

  cout << ")";
}


////////////////////////////////////////////////////////////////////////////////
// UTIL
////////////////////////////////////////////////////////////////////////////////

void
UTIL::sys_command(
  const string& command,
        string& result,
        bool& success,
  const bool& standard_error,
  const bool& replace_newlines)
{
  string cmd_str( command );

  if ( standard_error )
    cmd_str.append(" 2>&1");

#ifdef HAVE_POPEN
  FILE* pipe;
  char buffer[128];

  pipe = popen( cmd_str.c_str(), "r" );

  if (!pipe)
  {
    result = "popen() failed for " + cmd_str;

    success=false;
  }
  else
  {
    while ( !feof(pipe) )
    {
      if ( fgets(buffer, 128, pipe) != NULL )
          result.append( buffer );
    }

    pclose(pipe);

    success=true;
  }
#else /* HAVE_POPEN */
  result = "popen() not available, unable to execute " + cmd_str;

  success=false;
#endif /* HAVE_POPEN */

  if ( replace_newlines )
    result = UTIL::replace_chars(result, "\n\r", ' ');
}

size_t
UTIL::get_indent(const std::string& t)
{
  istringstream iss(t);
  string line;

  // move to first non-empty line
  while ( line.empty() && !iss.eof() )
    getline(iss, line);

  return ( line.find_first_not_of( " \t" ) );
}

string
UTIL::indent_line(const string& l, const int n)
{
  string new_line;

  for (int i=0; i<n; i++)
    new_line.append( " " );

  new_line.append( l );

  return new_line;
}

string
UTIL::indent_text(const string& t, const int n)
{
  istringstream iss(t);
  string new_text;

  while ( !iss.eof() )
  {
    string line;

    getline(iss, line);

    for (int i=0; i<n; i++)
      new_text.append( " " );

    new_text.append( line );
    new_text.append( "\n" );
  }

  return new_text;
}

string
UTIL::get_word(const string& w, const int n)
{
  istringstream iss(w);
  string iw, rw;

  for (int l=1; iss >> iw; l++)
    if ( l==n ) {
      rw=iw;
      break;
    }

  return rw;
}

size_t
UTIL::word_count(const string& w)
{
  istringstream iss(w);
  string iw;

  size_t l = 0;
  while( iss >> iw ) { l++; }

  return l;
}

string
UTIL::remove_chars(const string &s, const string &c)
{
  return ( replace_chars(s, c, '\0') );
}

string
UTIL::replace_chars(const string &s, const string &c, const char r)
{
  string result;

  for ( string::const_iterator its=s.begin(); its!=s.end(); ++its ) {
    bool append = true;

    for ( string::const_iterator itc=c.begin(); itc!=c.end(); ++itc )
      if ( *its == *itc ) { append = false; break; }

    if ( append )       result += *its;
    else if (r != '\0') result += r;
  }

  return( result );
}

string
UTIL::unquote(const string &s)
{
  string r = s;

  // remove outermost matching quotations
  // (1) quotes must be outermost text, excluding whitespace
  // (2) quotes must be matching set
  // (3) leading and trailing whitespace before and after
  //     the outermost quotation are discarded.

  // locate outer bounds of non-white space text
  size_t fc = s.find_first_not_of(" \f\n\r\t\v");
  size_t lc = s.find_last_not_of(" \f\n\r\t\v");

  // locate outermost quotations
  size_t fq = s.find_first_of("\"\'");
  size_t lq = s.find_last_of("\"\'");

  // unquote iff:
  // (1) not one in the same (character or string::npos)
  // (2) first non-white space character is first quote
  // (3) last non-white space character is last quote
  // (4) the quote characters are a matched pair
  if ( (fq != lq) && (fc == fq) && (lc == lq) )
  { // quotate character must match: ie '' or ""
    if ( s.at(fq) == s.at(lq) )
    {
      if ( (lq-fq) < 2 )
        // check for quoted null string ""
        r.clear();
      else
        // copy string between quotes
        r = s.substr( fq+1, lq-1-fq );
    }
  }

  return( r );
}

string
UTIL::unquote_trim(const string &s)
{
  return ( boost::trim_copy( unquote( s ) ) );
}

string
UTIL::to_string(const long v)
{
  ostringstream os;

  os << dec << v;

  return ( os.str() );
}

bool
UTIL::is_number(const std::string& s)
{
  string::const_iterator it = s.begin();

  while ( it != s.end() && isdigit(*it) )
    ++it;

  return ( !s.empty() && it == s.end() );
}

boost::filesystem::path
UTIL::get_relative_path(
    const boost::filesystem::path &to_path,
    const boost::filesystem::path &from_path,
    const bool parent)
{
  boost::filesystem::path::const_iterator tit = to_path.begin();
  boost::filesystem::path::const_iterator fit = from_path.begin();

  // remove leading '.' from both paths if any
  while (tit !=   to_path.end() && (*tit) == ".") ++tit;
  while (fit != from_path.end() && (*fit) == ".") ++fit;

  // loop while they are the same to find common parent path
  while (tit != to_path.end() && fit != from_path.end() && (*fit) == (*tit))
  {
    ++fit;
    ++tit;
  }

  boost::filesystem::path relative_path;

  if ( !parent )
  {
    // append ".." for each remaining level of from_path
    // in order to return to the common parent path
    while (fit != from_path.end())
    {
      // ignore current directory reference
      if ((*fit) != ".")
        relative_path /= "..";

      ++fit;
    }
  }

  // append remainder of to_path
  while (tit != to_path.end())
  {
    // ignore current directory reference
    if ((*tit) != ".")
      relative_path /= *tit;

    ++tit;
  }

  return relative_path;
}

bool
UTIL::make_dir(const std::string &d,
                     std::string &m,
               const bool &p,
               const std::string &a)
{
  boost::filesystem::path f ( d );      // new directory path
  boost::filesystem::path b ( a );      // existing ancestors base path
  boost::filesystem::path n ( b / f );  // new complete target path

  bool ok = true;

  m = "makedir-info";

  if ( n.empty() )
  { // target empty, do nothing.
    m += " [<pathname empty>]";
  }
  else if ( boost::filesystem::exists( n ) && boost::filesystem::is_directory( n ) )
  { // target exists, do nothing.
    m += " [" + n.string() + "] exists";
  }
  else if ( !p )
  {
    boost::filesystem::path np = n.parent_path();

    // make sure parent path exists
    if ( boost::filesystem::exists( np ) && boost::filesystem::is_directory( np ) )
    { // make target path.
      ok = boost::filesystem::create_directory( n );
      m += " [" + n.string() + "] 1 created";
    }
    else
    {
      ok = false;
      m += " ERROR: parent path [" + np.string() + "] does not exists, not created.";
    }
  }
  else
  { // otherwise assume ancestors exists, and make missing parents of target path.

    m += " [";

    boost::filesystem::path::const_iterator nit = n.begin();;
    boost::filesystem::path t;

    // ancestors: assume exists
    uint level = 0;

    if ( !b.empty() )
    {
      boost::filesystem::path::const_iterator bit = b.begin();;

      while (bit != b.end() && nit != n.end())
      {
        level++;

        ++bit;
        ++nit;
      }

      // mark end
      m += b.string() + ":";
      t  = b;
    }

    // make missing parents of target directory path
    uint count = 0;

    while (nit != n.end() && ok)
    {
      t /= *nit;

      if ( boost::filesystem::exists( t ) && boost::filesystem::is_directory( t ) )
      {
        m += (level?"/":"") + nit->string();
      }
      else
      {
        ok = boost::filesystem::create_directory( t );
        m += "/+<" + nit->string() + ">";
        count++;
      }
      level++;

      ++nit;
    }

    m += "] " + to_string( count ) + " created";
  }

  return ok;
}

string
UTIL::to_roman_numeral(const int &n)
{
  int   decimal[] = {1000,900,500,400,100,90,50,40,10,9,5,4,1};
  string symbol[] = {"M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I"};

  string rn;
  int dn = n;

  for (int i = 0; i < 13; i++)
    while (dn >= decimal[i])
    {
      dn -= decimal[i];
      rn.append( symbol[i] );
    }

  return ( rn );
}

std::string
UTIL::openscad_rmecho_line(const std::string &line)
{
  //
  // OpenSCAD ECHO format: [[ECHO:][ ][" ... echo-content ... "]endl]
  //

  std::string new_line = line;

  // [ECHO:]
  if ( new_line.find( "ECHO:" ) == 0 )
  { // 'ECHO:' iff at pos==0
    new_line.erase( 0, 5 );

    // [ ] single space character at pos==0
    // if ( new_line.find_first_of( " " ) == 0 )
    //  new_line.erase( 0, 1 );

    // [ ] all white space from pos==0
    size_t p = new_line.find_first_not_of( " \t" );
    if ( (p != 0) && (p != string::npos) )
      new_line.erase( 0, p );

    // [" ... echo-content ... "]
    if ( new_line.find_first_of( "\"" ) == 0 )
    { // open quote at pos==0
      new_line.erase( 0, 1 );

      // close quote at pos=eol
      size_t l = new_line.length();
      if ( (l != 0) && (new_line.find_last_of( "\"" ) == (l-1)) )
        new_line.erase( l-1, 1 );
      else
        new_line.append( "<ERROR: close quote missing>" );
    }
  }

  return ( new_line );
}

std::string
UTIL::openscad_rmecho_text(const std::string &text)
{
  istringstream iss(text);
  string new_text;

  while ( !iss.eof() )
  {
    string line;

    getline(iss, line);
    line = UTIL::openscad_rmecho_line(line);

    new_text.append( line );
    new_text.append( "\n" );
  }

  return new_text;
}

/*
  boost::empty_token_policy
    { drop_empty_tokens | keep_empty_tokens }
*/
vector<string>
UTIL::string_tokenize_to_vector(
  const std::string &str,
  const std::string &toks,                      // dropped delimiters
  const std::string &toks_keep,                 // kept delimiters
  const boost::empty_token_policy empty_tokens
)
{
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

  boost::char_separator<char>
      field_separators( toks.c_str(), toks_keep.c_str(), empty_tokens );

  tokenizer str_tokens( str, field_separators );

  vector<string> str_vector;
  for ( tokenizer::iterator it=str_tokens.begin(); it!=str_tokens.end(); ++it )
    str_vector.push_back( boost::trim_copy( *it ) );

  return ( str_vector );
}

std::string
UTIL::get_field(const size_t &num,
                const std::string &str, const std::string &def_str,
                const std::string &toks, const std::string &defs)
{
  string str_field;

  vector<string> str_vector =
    string_tokenize_to_vector(str, toks, "", boost::keep_empty_tokens);

  if ( num < str_vector.size() )
  {
    // get field
    str_field = str_vector.at( num );

    // when 'str_field' equal to 'defs' get default from 'def_str'
    if ( str_field.compare( defs ) == 0 )
    {
      string def_field;

      vector<string> def_vector =
        string_tokenize_to_vector(def_str, toks, "", boost::keep_empty_tokens);

      if ( num < def_vector.size() )
        def_field = def_vector.at( num );

      // assign default
      str_field = def_field;
    }
  }

  return ( str_field );
}

/*******************************************************************************
// eof
*******************************************************************************/
