/***************************************************************************//**

  \file   openscad_seam_scanner.cpp

  \author Roy Allen Sutton
  \date   2016

  \copyright

    This file is part of OpenSCAD AutoMake Utilities ([openscad-amu]
    (https://github.com/royasutton/openscad-amu)).

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
    Script extractor scanner source.

  \ingroup openscad_seam_src
*******************************************************************************/

using namespace std;

#include "openscad_seam_scanner.hpp"

#include "boost/filesystem.hpp"

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

SEAM::SEAM_Scanner::SEAM_Scanner(const string& f, const bool& m, const string& s)
{
  // initialize output prefix string
  set_ops( s );

  if ( m ) {
    // count mode scanner
    scanner_count_mode = true;
    scanner_output_on = false;
  } else {
    // extract mode scanner
    scanner_count_mode = false;
    scanner_output_on = true;
  }

  input_name = f;
  scanner_script_count = 0;
  prefix_scripts = true;

  init();
}

SEAM::SEAM_Scanner::~SEAM_Scanner(void)
{
  if ( input_file.is_open() ) {
    if ( !scanner_count_mode )
      cout << ops << "closing input file." << endl;

    input_file.close();
  }

  if ( output_file.is_open() ) {
    if ( !scanner_count_mode )
      cout << ops << "closing output file." << endl;

    output_file.close();
  }
}

void
SEAM::SEAM_Scanner::init(void)
{
  // clear output file name and switch output to standard out
  output_name.clear();
  switch_output( &cout );

  // if rootscope has been preveously set, reset to initial rootscope
  if ( !rootscope.empty() )
    set_rootscope( rootscope );

  if ( input_file.is_open() ) {
    if ( !scanner_count_mode )
      cout << ops << "resetting input file," << endl;
    input_file.close();
  }

  input_file.open( input_name.c_str() );

  if ( !input_file.good() ) {
    cerr << ops << "unable to open input file [" << input_name << "]";
    LexerError(", aborting...");
  }

  if ( !scanner_count_mode )
    cout << ops << "reading from file " << input_name << ";" << endl;

  // set lexer input to opened input file
  switch_streams( &input_file );
}

void
SEAM::SEAM_Scanner::abort(const std::string& m)
{
  cerr << ops << m << ", at line " << lineno() << ", near [" << YYText() << "]";
  LexerError(", aborting...");
}

std::string
SEAM::SEAM_Scanner::get_word(const std::string& w, const int n)
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

std::string
SEAM::SEAM_Scanner::get_filename(const std::string& e)
{
  using namespace boost::filesystem;
  path file_name;

  try
  {
    if ( prefix_scripts )
      file_name /= output_prefix;

    file_name /= scope.top().name();
    file_name += e;
  }
  catch (const filesystem_error& ex)
  {
    cout << ex.what() << endl;
  }

  return( file_name.string() );
}

void
SEAM::SEAM_Scanner::scanner_output( const char* buf, int size )
{
  if (scanner_output_on) {
    LexerOutput( buf, size );
  }
}

void
SEAM::SEAM_Scanner::switch_output(std::ostream* s)
{
  if ( output_file.is_open() ) {
    if (verbose) cout << ops << "closing output file." << endl;

    output_file.close();

    output_name.clear();
  }

  // update output stream variable
  switch_streams(0, s);
}

void
SEAM::SEAM_Scanner::switch_output(const std::string& ext)
{
  if ( output_file.is_open() ) {
    if (verbose) cout << ops << "switching output file." << endl;

    output_file.close();
  }

  output_name = get_filename( ext );

  // Test to make sure not over writing input file. This can happen
  // when the output prefix is the same as the input file path and
  // an openscad script is extracted in the root scope.

  if ( output_name.compare( input_name ) == 0 ) {
    cerr << endl
         << ops << "specified output file " << output_name << ","<< endl
         << ops << "would overwrite input file." << endl
         << endl
         << ops << "use sub-scoping, change root scope, or change" << endl
         << ops << "output prefix to eliminate file name conflict." << endl
         << endl
         << ops;
    LexerError("aborting...");
  }

  cout << ops << "writing to file " << output_name << "," << endl;

  output_file.open( output_name.c_str() );

  if ( !output_file.good() ) {
    cerr << ops << "unable to open output file [" << output_name << "]";
    LexerError(", aborting...");
  }

  // update output stream variable
  switch_streams(0, &output_file);
}

void
SEAM::SEAM_Scanner::output_info_header(const std::string& cs)
{
  string sl = cs + " ";

  output_file
    << sl << endl
    << sl << "THIS FILE HAS BEEN GENERATED, CHANGES WILL LIKELY BE OVERRIDDEN" << endl
    << sl << endl
    << sl << "    package: " << PACKAGE_NAME << endl
    << sl << "    version: " << PACKAGE_VERSION << endl
    << sl << " bug report: " << PACKAGE_BUGREPORT << endl
    << sl << "   site url: " << PACKAGE_URL << endl
    << sl << endl
    << sl << " input file: " << input_name << endl
    << sl << "output file: " << output_name << " (this file)" << endl
    << sl << endl;
}

void
SEAM::SEAM_Scanner::output_info_footer(const std::string& cs)
{
  string sl = cs + " ";

  output_file
    << endl
    << sl << endl
    << sl << "EOF" << endl
    << sl << endl;
}

void
SEAM::SEAM_Scanner::set_rootscope( const std::string& t )
{
  rootscope = t;

  // empty stack and initialize rootscope
  while ( !scope.empty())
    scope.pop();
  scope.push(SEAM_Scope( rootscope ));

  // clear and initialize the scope_id vector with the rootscope
  scope_id.clear();
  scope_id.push_back( rootscope );
}

void
SEAM::SEAM_Scanner::begin_scope( const char t, const std::string& lm )
{
  string id = get_word( lm, 2 );

  // remove ';' character at end of string
  id.erase(id.length()-1, 1);

  string lt;
  string cs;

  if      ( t == 'r' )
  {
    lt = "root";
    cs = id;
    scope.push(SEAM_Scope(cs, id, t));
  }
  else if ( t == 'p' )
  {
    lt = "prepend";
    cs = scope.top().name();
    scope.push(SEAM_Scope(cs, id + scopejoiner, t));
  }
  else   // t == 'a'
  {
    lt = "append";
    cs = scope.top().name();
    scope.push(SEAM_Scope(cs, scopejoiner + id, t));
  }

  if (verbose) cout << ops << "begin scope " << lt << ":[" << id << "] "
                    << cs << " --> " << scope.top().name() << "," << endl;

  // add new scope to the scope identifiers vector
  scope_id.push_back( scope.top().name() );
}

void
SEAM::SEAM_Scanner::end_scope(void)
{
  string ps = scope.top().name();

  scope.pop();

  if (verbose) cout << ops << "end scope " << scope.top().name()
                    << "<--" << ps << "," << endl;
}

void
SEAM::SEAM_Scanner::begin_mfscript(void)
{
  scanner_script_count++;

  if (scanner_count_mode) {
    if ( verbose ) cout << ops << "(" << scanner_script_count << ") makefile script ["
                        << get_filename( mfscript_ext ) << "]" << endl;
    return;
  }

  string scope_makefile = get_filename( makefile_ext );
  string scope_openscad = get_filename( openscad_ext );
  string mfscript_init = "lib/bootloader.bash";

  switch_output( mfscript_ext );

  output_file << "#!" << bash_path << endl;
  output_info_header("#");

  if ( verbose )
  output_file << "__VERBOSE__=true" << endl;

  output_file << "__LIBPATH__=\"" << lib_path << "\""
              << " && source \"${__LIBPATH__}/" << mfscript_init << "\"" << endl
              << "__PREFIX__=\"" << output_prefix << "\"" << endl
              << "__SOURCE_FILE__=\"" << input_name << "\"" << endl
              << "__SCOPE_FILE__=\"" << scope_openscad << "\"" << endl
              << "__MAKE_FILE__=\"" << scope_makefile << "\"" << endl;

  if ( !openscad_path.empty() )
  output_file << "sc_openscad=" << openscad_path << endl;

  // output defines
  for(size_t i=0; i<define.size(); ++i)
    output_file << define[i] << endl;
}

void
SEAM::SEAM_Scanner::end_mfscript(void)
{
  if (scanner_count_mode)
    return;

  output_info_footer("#");

  // store output file name
  string script_file = output_name;

  // close output and switch to cout
  switch_output( &std::cout );

  if ( show ) {
    ifstream tmp_file( script_file.c_str() );

    cout << "### begin show file [" << script_file << "] ###" << endl;
    cout << tmp_file.rdbuf();
    cout << "### end show file ###" << endl;

    tmp_file.close();
  }

  // set execute bits
  using namespace boost::filesystem;
  try
  {
    path script_path( script_file );

    permissions(script_path, add_perms|owner_exe|group_exe|others_exe);
  }
  catch (const filesystem_error& ex)
  {
    cout << ex.what() << endl;
  }

  // run script
  if ( run ) {
    string scmd = bash_path + " --norc --noprofile " + script_file;

    if ( verbose ) cout << ops << "executing script: " << scmd << endl;

    int res = system( scmd.c_str() ) ;

    if ( res != 0 ) {
      cerr << ops << "makefile script returned error";
      LexerError(", aborting...");
    }

    if ( verbose ) cout << "script returned: " << res << endl;
  }

  // run make with target
  if ( make && !target.empty() ) {
    string scope_makefile = get_filename( makefile_ext );

    string scmd = make_path + " --makefile=" + scope_makefile + " " + target;

    if ( verbose ) cout << ops << "executing make: " << scmd << endl;

    int res = system( scmd.c_str() ) ;

    if ( res != 0 ) {
      cerr << ops << "make returned error";
      LexerError(", aborting...");
    }

    if ( verbose ) cout << ops << "make returned: " << res << endl;
  }
}

void
SEAM::SEAM_Scanner::begin_openscad(void)
{
  scanner_script_count++;

  if (scanner_count_mode) {
    if ( verbose ) cout << ops << "(" << scanner_script_count << ") openscad script ["
                        << get_filename( openscad_ext ) << "]" << endl;
    return;
  }

  switch_output( openscad_ext );

  output_info_header("//");
}

void
SEAM::SEAM_Scanner::end_openscad(void)
{
  if (scanner_count_mode)
    return;

  output_info_footer("//");

  // store output file name
  string script_file = output_name;

  // close output and switch to cout
  switch_output( &std::cout );

  if ( show ) {
    ifstream tmp_file( script_file.c_str() );

    cout << "### begin show file [" << script_file << "] ###" << endl;
    cout << tmp_file.rdbuf();
    cout << "### end show file ###" << endl;

    tmp_file.close();
  }
}

/*******************************************************************************
// eof
*******************************************************************************/
