/**************************************************************************//**

  \file   openscad_dif_main.cpp

  \author Roy Allen Sutton
  \date   2016-2018

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
    OpenSCAD Doxygen input filter main source.

  \todo add option to list JavaScript referenced by all input source. this
        would help construct makefile dependencies (--list-js). The larger
        point is that this filter needs to be responsible for installing
        and removing JavaScript dependencies (or at least communicating
        them to a responsible party). See Doxygen configuration tab
        HTML_EXTRA_FILES.
  \todo add option to create a histogram/map of amu command that exists
        in the input file.

  \ingroup openscad_dif_src
*******************************************************************************/

#include "openscad_dif_scanner.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <iomanip>
#include <map>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

//! \ingroup openscad_dif_src
//! @{

using namespace std;
using namespace boost::filesystem;
namespace po = boost::program_options;


// program constants.
namespace
{
  // program return value constants.
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 1;
  const size_t ERROR_IN_COMMAND_LINE = 2;
  const size_t ERROR_UNABLE_TO_OPEN_FILE = 3;
}


//! output build information.
void
build_info(ostream& sout, const string& command_name)
{
  int w=20;

  sout << command_name << " " << PACKAGE_VERSION << endl << endl
       << setw(w) << "package: " << PACKAGE_NAME << endl
       << setw(w) << "version: " << PACKAGE_VERSION << endl
       << setw(w) << "bug report: " << PACKAGE_BUGREPORT << endl
       << setw(w) << "site url: " << PACKAGE_URL << endl << endl
       << setw(w) << "build date: " << __BUILD_DATE__ << endl
       << setw(w) << "architecture: " << __BUILD_ARCH__ << endl << endl
       << setw(w) << "default lib path: " << __LIB_PATH__ << endl << endl;
}


//! check if an option is set and throws exception if so.
void
option_set_conflict(
  const po::variables_map& vm,
  const char* opt,
  const char* msg)
{
  if ( vm.count(opt) && !vm[opt].defaulted() )
    throw logic_error( string("Conflicting option '--")
          + opt + "'" + msg );
}


//! check if both options are set and throws exception if so.
void
option_conflict(
  const po::variables_map& vm,
  const char* opt1,
  const char* opt2)
{
  if ( vm.count(opt1) && !vm[opt1].defaulted()
    && vm.count(opt2) && !vm[opt2].defaulted() )
    throw logic_error( string("Conflicting options '--")
          + opt1 + "' and '--" + opt2 + "'" );
}


//! check for opt2 if opt1 is set and throws exception if not.
void
option_depend(
  const po::variables_map& vm,
  const char* opt1,
  const char* opt2)
{
  if ( vm.count(opt1) && !vm[opt1].defaulted() )
    if ( vm.count(opt2) == 0 || vm[opt2].defaulted() )
      throw logic_error( string("Option '--" ) + opt1
            + "' requires option '--" + opt2 + "'" );
}


//! begin or end a comment block for filter debugging messages.
void
debug_hf(const bool output, const bool header, const string& command_name="")
{
  if ( output )
  {
    if ( header ) {
      cout << "//! \\cond __INCLUDE_FILTER_DEBUG__" << endl
           << "/**" << endl
           << "\\page debug_" << command_name
           << " Debug (" << command_name << ")" << endl
           << "\\verbatim" << endl;

      cerr << "filter debugging page begin:" << endl;

      build_info( cout, command_name );
      build_info( cerr, command_name );
    } else {
      cout << "\\endverbatim" << endl
           << "*/" << endl
           << "//! \\endcond" << endl;

      cerr << "filter debugging page end." << endl;
    }
  }
}

//! output filter debugging message to debug page and standard error.
void
debug_m(const bool& output, const string& message, const bool& newline=true)
{
  if ( output )
  {
    cout << message;
    if ( newline )
      cout << endl;
    cerr << message;
    if ( newline )
      cerr << endl;
  }
}


//! search for configuration file for given input file.
bool
find_config(
  const string input,
  const string auto_config,
  string& config,
  const bool debug_filter)
{
  path input_path ( input );
  path input_path_abs ( input_path.parent_path() );
  path input_path_rel;
  path config_name ( input_path.stem() );

  input_path_rel = UTIL::get_relative_path(input_path_abs, current_path());
  config_name += ".conf";

  debug_m(debug_filter, "         input file: [" + input + "]");
  debug_m(debug_filter, "   auto config path: [" + auto_config + "]");
  debug_m(debug_filter, " target config name: [" + config_name.string() + "]");
  debug_m(debug_filter, "       current path: [" + current_path().string() + "]");
  debug_m(debug_filter, "absolute input path: [" + input_path_abs.string() + "]");
  debug_m(debug_filter, "relative input path: [" + input_path_rel.string() + "]");

  vector<path> try_paths;

  // search order:
  // (1) <auto-config-path>
  // (2) <auto-config-path>/<relative-input-path>
  // (3) <auto-config-path>/<absolute-input-path>
  // (4) <relative-input-path>
  // (5) <absolute-input-path>
  // (6) <./>

  try_paths.push_back( path(auto_config) );
  try_paths.push_back( path(auto_config) / input_path_rel );
  try_paths.push_back( path(auto_config) / input_path_abs.relative_path() );
  try_paths.push_back( input_path_rel );
  try_paths.push_back( input_path_abs );
  try_paths.push_back( "." );

  debug_m(debug_filter, "searching for configuration file.");

  bool found = false;

  for(
    vector<path>::iterator it = try_paths.begin();
    (found == false) && (it != try_paths.end());
    ++it
  )
  {
      path try_file ( *it / config_name );

      debug_m(debug_filter, "  trying: [" + try_file.string() + "]");

      if ( exists(try_file) && is_regular_file(try_file) )
      {
        config = try_file.string();
        found = true;
      }
  }

  return found;
}


//! program main.
int
main(int argc, char** argv)
{
  try
  {
    ////////////////////////////////////////////////////////////////////////////
    // setup command line arguments
    ////////////////////////////////////////////////////////////////////////////
    string command_name = basename( argv[0] );

    // command line
    string input;
    bool search           = true;
    vector<string> include_path;
    string doxygen_output;
    string html_output    = ODIF::NO_FORMAT_OUTPUT;
    string latex_output   = ODIF::NO_FORMAT_OUTPUT;
    string docbook_output = ODIF::NO_FORMAT_OUTPUT;
    string rtf_output     = ODIF::NO_FORMAT_OUTPUT;
    string lib_path       = __LIB_PATH__;
    string auto_config;
    string config;

    bool debug_filter     = false;

    // configuration file
    string scope;
    string joiner         = "_";
    vector<string> scope_id;
    bool prefix_scripts   = false;
    string output_prefix;
    string make_path      = __MAKE_PATH__;
    string makefile_ext   = ".makefile";
    string openscad_path  = __OPENSCAD_PATH__;
    string openscad_ext   = ".scad";

    // other
    vector<string> scope_id_mf;

    po::options_description opts_cli("Options (visible)");
    opts_cli.add_options()
      ("input,i",
          po::value<string>(&input)->required(),
          "Input source file name.\n")
      ("search,s",
          po::value<bool>(&search)->default_value(search),
          "Search make targets for references.")
      ("include-path,I",
          po::value<vector<string> >(&include_path),
          "Explicit search paths for references.\n")
      ("doxygen-output",
          po::value<string>(&doxygen_output),
          "Doxygen output rootpath.\n")
      ("html-output",
          po::value<string>(&html_output)->default_value(html_output),
          "HTML output path.")
      ("latex-output",
          po::value<string>(&latex_output)->default_value(latex_output),
          "Latex output path.")
      ("docbook-output",
          po::value<string>(&docbook_output)->default_value(docbook_output),
          "Docbook output path.")
      ("rtf-output",
          po::value<string>(&rtf_output)->default_value(rtf_output),
          "RTF output path.\n")
      ("lib-path",
          po::value<string>(&lib_path)->default_value(lib_path),
          "Makefile script library path.\n")
      ("auto-config,a",
          po::value<string>(&auto_config),
          "Filter Auto configuration path.")
      ("config,c",
          po::value<string>(&config),
          "Read configuration file.\n")
      ("debug-scanner",
          "Run scanner in debug mode.")
      ("debug-filter",
          "Turn on filter debugging output.\n")
      ("verbose,V",
          "Run in version mode.")
      ("version,v",
          "Report tool version.")
      ("help,h",
          "Print this help message.")
    ;

    po::options_description opts_conf("Options (hidden)");
    opts_conf.add_options()
      ("scope",
          po::value<string>(&scope),
          "Scope root name.")
      ("joiner",
          po::value<string>(&joiner)->default_value(joiner),
          "Scope joiner. String used to conjoin scope hierarchies.")
      ("scope-id",
          po::value<vector<string> >(&scope_id),
          "One or more scope identifiers.\n")
      ("prefix-scripts",
          po::value<bool>(&prefix_scripts)->default_value(prefix_scripts),
          "Output path prefix were prepended to extracted scripts.")
      ("output-prefix",
          po::value<string>(&output_prefix),
          "Output prefixed used by seam.\n")
      ("make-path",
          po::value<string>(&make_path)->default_value(make_path),
          "GNU Make executable path.")
      ("makefile-ext",
          po::value<string>(&makefile_ext)->default_value(makefile_ext),
          "Makefile file extension.\n")
      ("openscad-path",
          po::value<string>(&openscad_path)->default_value(openscad_path),
          "OpenSCAD executable path.")
      ("openscad-ext",
          po::value<string>(&openscad_ext)->default_value(openscad_ext),
          "OpenSCAD script extension.")
    ;

    // all parsed options
    po::options_description opts("Options (all)");
    opts.add(opts_cli).add(opts_conf);

    // positional options
    po::positional_options_description opts_pos;
    opts_pos.add("input", 1);


    ////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    ////////////////////////////////////////////////////////////////////////////
    po::variables_map vm;

    try
    {
      // parse command line options
      po::store(po::command_line_parser(argc, argv).options(opts)
                  .positional(opts_pos).run(), vm);

      // output help and exit
      if ( vm.count("help") || (argc==1) )
      {
        cout << command_name << " " << PACKAGE_VERSION << endl
             << endl
             <<
  "OpenSCAD source script Doxygen input filter (dif).\n\n"
  "An OpenSCAD design scripts preprocesspr input filter for the Doxygen\n"
  "documentation generaton tool.\n"
             << endl
             << "Examples:" << endl
             << "  INPUT_FILTER = <prefix>/bin/" << command_name << endl
             << "  INPUT_FILTER = \"<prefix>/bin/" << command_name
             << " --config <config>\"" << endl
             << "  FILTER_PATTERNS = *.scad=\"<prefix>/bin/" << command_name
             << " --config <config>\"" << endl
             << endl;

        if ( vm.count("verbose") )  cout << opts     << endl;
        else                        cout << opts_cli << endl;

        exit( SUCCESS );
      }

      // output version and exit
      if ( vm.count("version") )
      {
        if ( vm.count("verbose") ) {
          build_info( cout, command_name );
        } else {
          cout << PACKAGE_VERSION << endl;
        }

        exit( SUCCESS );
      }

      debug_filter = ( vm.count("debug-filter")>0 );

      // begin filter debugging page
      debug_hf( debug_filter, true, command_name );

      // auto configuration
      if ( vm.count("auto-config") )
      {
        // early partial validation
        option_set_conflict( vm, "config", " not allowed with auto configuration");
        option_depend( vm, "auto-config", "input");

        // notify not called to prevent exception from required program options,
        // get values from variable map directly.
        input = vm["input"].as<string>();
        auto_config = vm["auto-config"].as<string>();

        debug_m(debug_filter, "attempting auto-configuration");
        if ( find_config(input, auto_config, config, debug_filter) )
        {
          debug_m(debug_filter, "configuration found.");

          // insert configuration file name into variable map to be parsed later.
          vm.insert( make_pair("config", po::variable_value(config, true)) );
        } else {
          debug_m(debug_filter, "configuration not found.");
        }
      }

      // parse configuration file options
      if ( vm.count("config") )
      {
        // notify not called to prevent exception from required program options,
        // get values from variable map directly.
        config = vm["config"].as<string>();

        debug_m(debug_filter, "reading configuration file: [" + config + "]");

        std::ifstream config_file ( config.c_str() );

        if ( config_file.good() )
        { // parse configuration file options
          po::store(po::parse_config_file(config_file, opts, true), vm);
        } else {
          cerr << "ERROR: unable to open configuration file [" << config
               << "]" << endl;

          exit( ERROR_UNABLE_TO_OPEN_FILE );
        }

        config_file.close();
      }

      po::notify(vm);
    }
    catch(po::required_option& e)
    {
      cerr << "ERROR: " << e.what() << endl;
      exit( ERROR_IN_COMMAND_LINE );
    }
    catch(po::error& e)
    {
      cerr << "ERROR: " << e.what() << endl;
      exit( ERROR_IN_COMMAND_LINE );
    }


    ////////////////////////////////////////////////////////////////////////////
    // validate arguments
    ////////////////////////////////////////////////////////////////////////////
     option_depend( vm, "verbose", "version");

    // set doxygen-output when not specified for backwards compatibility
    if ( !vm.count("doxygen-output") )
    {
      debug_m(debug_filter, "doxygen-output not specified, setting to"
                            " output-prefix=[" + output_prefix + "]");

      doxygen_output = output_prefix;
      vm.insert( make_pair("doxygen-output", po::variable_value(doxygen_output, true)) );
    }


    ////////////////////////////////////////////////////////////////////////////
    // include-path auto-configuration
    ////////////////////////////////////////////////////////////////////////////
    scope_id_mf.clear();

    if ( search )
    {
      debug_m(debug_filter, "auto-search: configuring include paths.");

      //
      // add target directories for for each scope
      //
      debug_m(debug_filter, "checking each scope for generated makefile:");

      map<string,string> path_map;

      // check each scope identifier
      for ( vector<string>::iterator vit=scope_id.begin(); vit != scope_id.end(); ++vit)
      {
        string scope_name( *vit );
        string target_prefix = "echo_targetsdir";

        // assemble system command
        string scmd;
        string opts = " --no-print-directory";

        path makefile_path;

        // identify path prefix to makefile
        if ( prefix_scripts )
        {
          makefile_path /= output_prefix;
          opts += " --directory=" + output_prefix;
        }
        else if ( auto_config.length() )
        {
          makefile_path /= auto_config;
          opts += " --directory=" + auto_config;
        }

        makefile_path /= scope_name + makefile_ext;

        scmd = make_path + opts
             + " --makefile=" + scope_name + makefile_ext
             + " " + target_prefix;

        debug_m(debug_filter, "scope: " + scope_name);

        // when makefile exists
        if ( exists(makefile_path) && is_regular_file(makefile_path) )
        {
          debug_m(debug_filter, "  makefile [" + makefile_path.string() + "] exists");

          // save to list of scopes with makefile
          debug_m(debug_filter, "  recording scope identifier [" + scope_name + "] in scope-id-mf");
          scope_id_mf.push_back( scope_name );

          // run make to discover the target output directories
          string result;
          bool good=false;

          debug_m(debug_filter, "  running: " + scmd );
          UTIL::sys_command( scmd, result, good, false, true );

          if ( good )
          {
            debug_m(debug_filter, "   result: " + result, true );
          }
          else
          {
            debug_m(debug_filter, "    error: " + result, true );

            // system command error, don't add directories.
            continue;
          }

          // tokenize result string of directories
          typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
          boost::char_separator<char> fsep(" \n");
          tokenizer rt_tok( result, fsep );

          //  add each include path if directory exists
          for ( tokenizer::iterator it=rt_tok.begin(); it!=rt_tok.end(); ++it )
          {
            path tp( *it );

            debug_m(debug_filter, "  path [" + tp.string() + "] ", false);
            if ( exists(tp) && is_directory(tp) )
            { // add to include path map if it does not already exists
              if ( path_map.find(*it) == path_map.end() )
                path_map.insert( make_pair( *it, tp.string() ) );

              debug_m(debug_filter, "exists.");
            }
            else
            {
              debug_m(debug_filter, "does not exists.");
            }
          }
        }
        else
        {
          debug_m(debug_filter, "  makefile [" + makefile_path.string() + "] does not exists");
        }
      }

      //
      // add search results to include path
      //

      // add unique target directories in map
      debug_m(debug_filter, "adding unique target directories to include-path:");
      for( map<string,string>::iterator mit=path_map.begin(); mit != path_map.end(); ++mit)
      {
        debug_m(debug_filter, " added [" + mit->second + "]");
        include_path.push_back( mit->second );
      }

      // add prefixed output directory
      if ( prefix_scripts )
      {
        debug_m(debug_filter, " added [" + output_prefix + "] (prefix)");
        include_path.push_back( output_prefix );
      }

      // add auto-config output root directory
      if ( auto_config.length() )
      {
        debug_m(debug_filter, " added [" + auto_config + "] (auto-config)");
        include_path.push_back( auto_config );
      }

      debug_m(debug_filter, "adding source input directories to include-path:");

      path input_path ( input );
      path input_path_rel = UTIL::get_relative_path(input_path.parent_path(), current_path());

      // add source relative input path if exists
      if ( !input_path_rel.empty() )
      {
        debug_m(debug_filter, " added [" + input_path_rel.string() + "] (prefix)");
        include_path.push_back( input_path_rel.string() );
      }

      // add source root directory
      debug_m(debug_filter, " added [.] (root)");
      include_path.push_back( "." );

      // insert discovered paths into a new program option identifier.
      // this is for informational purposes only for use with debugging.
      // do not notify so as to not overwrite the same additions to include_path.
      vm.insert(make_pair("auto-path", po::variable_value(include_path, false)));
    }
    else
    {
      debug_m(debug_filter, "auto-search: inactive.");
    }


    ////////////////////////////////////////////////////////////////////////////
    // show configuration
    ////////////////////////////////////////////////////////////////////////////
    if ( debug_filter )
    {
      cout << endl << "program options:" << endl << endl;

      for(po::variables_map::const_iterator it = vm.begin(); it != vm.end(); ++it)
      {
        // skip some options
        if (  !it->first.compare("debug-filter") |
              !it->first.compare("debug-scanner") ) continue;

        int w=15;
        cout << setw(w) << it->first << " = [";
        if        (((boost::any)it->second.value()).type() == typeid(int)) {
          cout << it->second.as<int>();
        } else if (((boost::any)it->second.value()).type() == typeid(bool)) {
          cout << it->second.as<bool>();
        } else if (((boost::any)it->second.value()).type() == typeid(string)) {
          cout << it->second.as<string>();
        } else if (((boost::any)it->second.value()).type() == typeid(vector<string>)) {
          vector<string> v = it->second.as<vector<string> >();
          for ( vector<string>::iterator vit=v.begin(); vit != v.end(); ++vit) {
            if ( (vit) != v.begin() ) cout << setw(w+4) << "+ [";
            cout << *vit;
            if ( (vit+1) != v.end() ) cout << "]" << endl;
          }
        } else {
          cout << "<unknown-value-type>";
        }
        cout << "]";

        if ( it->second.defaulted() ) cout << " (defaulted)";

        cout << endl;
      }
    }

    // end filter debugging page
    debug_hf( debug_filter, false );


    ////////////////////////////////////////////////////////////////////////////
    // setup and run scanner
    ////////////////////////////////////////////////////////////////////////////
    ODIF::ODIF_Scanner scanner( input, command_name + ": " );

    // command line
    scanner.set_include_path( include_path );
    scanner.set_doxygen_output( doxygen_output );
    scanner.set_html_output( html_output );
    scanner.set_latex_output( latex_output );
    scanner.set_docbook_output( docbook_output );
    scanner.set_rtf_output( rtf_output );
    scanner.set_lib_path( lib_path );
    scanner.set_config_prefix( auto_config );
    scanner.set_debug( vm.count("debug-scanner")>0 );
    scanner.set_debug_filter( debug_filter );

    // configuration file
    scanner.set_rootscope( scope );
    scanner.set_scopejoiner( joiner );
    scanner.set_scope_id( scope_id );
    scanner.set_prefix_scripts( prefix_scripts );
    scanner.set_output_prefix( output_prefix );
    scanner.set_make_path( make_path );
    scanner.set_makefile_ext( makefile_ext );
    scanner.set_openscad_path( openscad_path );
    scanner.set_openscad_ext( openscad_ext );

    // other
    scanner.set_scope_id_mf( scope_id_mf );

    // update global environment variable map
    scanner.update_gevm();

    // process input file
    while( scanner.scan() != 0 )
      ;
  }
  catch(exception& e)
  {
    cerr << "Unhandled exception reached the top of main:" << endl
         << e.what() << "," << endl
         << "exiting..." << endl;
    exit( ERROR_UNHANDLED_EXCEPTION );
  }

  exit( SUCCESS );
}

//! @}


/*******************************************************************************
// eof
*******************************************************************************/
