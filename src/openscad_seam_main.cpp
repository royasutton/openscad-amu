/***************************************************************************//**

  \file   openscad_seam_main.cpp

  \author Roy Allen Sutton
  \date   2016-2017

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
    Script extractor main source.

  \todo add command line option to extract scripts that match specific
        scope name and script type: {MFScript and/or Openscad}.

  \ingroup openscad_seam_src
*******************************************************************************/

#include "openscad_seam_scanner.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/any.hpp>

#include <iomanip>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

//! \ingroup openscad_seam_src
//! @{

using namespace std;
namespace po = boost::program_options;


// program constants.
namespace
{
  // program return value constants.
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 1;
  const size_t ERROR_IN_COMMAND_LINE = 2;
  const size_t ERROR_UNABLE_TO_OPEN_FILE = 3;
  const size_t ERROR_SCRIPT_COUNT_ZERO = 4;

  // program run mode constants.
  const size_t MODE_COUNT   = 1;
  const size_t MODE_EXTRACT = 2;
  const size_t MODE_LIST    = 4;
  const size_t MODE_RETURN  = 8;
  const size_t MODE_SCOPES  = 16;
  const size_t MODE_ALL     = 31;
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
       << setw(w) << "default lib path: " << __LIB_PATH__ << endl
       << setw(w) << "openscad path: " << __OPENSCAD_PATH__ << endl
       << setw(w) << "bash path: " << __BASH_PATH__ << endl
       << setw(w) << "gnu make path: " << __MAKE_PATH__ << endl << endl;
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


//! Structure to specify a program option name and its mode contexts.
typedef struct {
    int format;                     //!< line output format encoding.
        // bit  use                 0         1
        //  1   new section         no        yes
        //  2   quote strings       no        yes
    string name;                    //!< name.
    string description;             //!< description.
    size_t modes;                   //!< mode contexts.
} po_modes;


//! Output the program options in (more) human readable format.
void
format_options(
  ostream& sout,
  const string &title,
  const string &ops,
  const vector<po_modes> &ov,
  const size_t mode,
  const po::variables_map& vm)
{
  sout << ops << endl
       << ops << " " << title << endl;

  int max_length=0;
  for(vector<po_modes>::const_iterator it = ov.begin(); it != ov.end(); ++it)
    if ( it->description.length() > max_length )
      max_length = it->description.length();

  for(vector<po_modes>::const_iterator it = ov.begin(); it != ov.end(); ++it) {
    if (it->modes & mode ) {

      switch ( it->format & 1 )
      {
        case 0 : sout << ops; break;
        case 1 : sout << ops << endl << ops; break;
      }

      if ( it->description.length() != 0 ) {
        if ( vm[it->name].defaulted() ) sout << " (d)";
        else                            sout << "    ";

        sout << setw(max_length+1) << it->description << ": ";

        if        (((boost::any)vm[it->name].value()).type() == typeid(int)) {
          sout << vm[it->name].as<int>();
        } else if (((boost::any)vm[it->name].value()).type() == typeid(bool)) {
          sout << ((vm[it->name].as<bool>()==true) ? "yes" : "no");
        } else if (((boost::any)vm[it->name].value()).type() == typeid(string)) {
          if (it->format & 2) sout << "\"";
          sout << vm[it->name].as<string>();
          if (it->format & 2) sout << "\"";
        } else if (((boost::any)vm[it->name].value()).type() == typeid(vector<string>)) {
          vector<string> v = vm[it->name].as<vector<string> >();
          for ( vector<string>::iterator vit=v.begin(); vit != v.end(); ++vit) {
            if ( (vit) != v.begin() ) sout << ops << setw(max_length+(1+4+2)) << "+ ";
            sout << *vit;
            if ( (vit+1) != v.end() ) sout << endl;
          }
        } else {
          sout << "<unspecified>";
        }
      }

      sout << endl;
    }
  }

  sout << ops << endl;
}


//! Output the program options in configuration file format.
void
write_options(
  ostream& sout,
  const po::variables_map& vm)
{
  sout << "#" << endl
       << "# input: " << vm["input"].as<string>() << endl
       << "#" << endl
       << endl;

  for(po::variables_map::const_iterator it = vm.begin(); it != vm.end(); ++it)
  {
    // skip some options
    if (  !it->first.compare("config") |
          !it->first.compare("write-config") |
          !it->first.compare("verbose") |
          !it->first.compare("debug-scanner") ) continue;

    sout << "# " << it->first;
    if ( it->second.defaulted() ) sout << " (defaulted)";
    sout << endl;

    if        (((boost::any)it->second.value()).type() == typeid(int)) {
      sout << it->first << "=" << it->second.as<int>();
    } else if (((boost::any)it->second.value()).type() == typeid(bool)) {
      sout << it->first << "=" << it->second.as<bool>();
    } else if (((boost::any)it->second.value()).type() == typeid(string)) {
      sout << it->first << "=" << it->second.as<string>();
    } else if (((boost::any)it->second.value()).type() == typeid(vector<string>)) {
      vector<string> v = it->second.as<vector<string> >();
      for ( vector<string>::iterator vit=v.begin(); vit != v.end(); ++vit) {
        sout << it->first << "=" << *vit;
        if ( (vit+1) != v.end() ) sout << endl;
      }
    } else {
      sout << it->first << "=unknown-value-type";
    }

    sout << endl << endl;
  }
  sout << "#" << endl
       << "# eof" << endl
       << "#" << endl;
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
    string command_name = boost::filesystem::basename( argv[0] );

    string mode            = "extract";

    string input;
    string scope;
    string joiner         = "_";
    string prefix;
    int prefix_ipp        = -1;
    bool prefix_scripts   = true;

    string output_prefix;

    vector<string> define;

    bool comments         = true;
    bool show             = false;
    bool run              = false;
    bool make             = false;

    string target         = "all";

    string lib_path       = __LIB_PATH__;
    string openscad_path;
    string bash_path      = __BASH_PATH__;
    string make_path      = __MAKE_PATH__;

    string makefile_ext   = ".makefile";
    string mfscript_ext   = ".bash";
    string openscad_ext   = ".scad";

    string config;
    bool write_config     = false;

    po::options_description opts("Options");
    opts.add_options()
      ("mode,m",
          po::value<string>(&mode)->default_value(mode),
          "Mode: one of (count | extract | list | return | scopes). "
          "Count outputs the number of scripts found in the input. List "
          "enumerates the script names found. Return sets the command "
          "exit value to the script count. Scopes enumerate the scope "
          "names found. Extract write the scripts to separate files.\n")
      ("input,i",
          po::value<string>(&input)->required(),
          "Input file containing annotated script(s) embedded within "
          "comments.\n")
      ("scope,s",
          po::value<string>(&scope),
          "Scope root name. When not specified, the input file stem "
          "name is used.")
      ("joiner,j",
          po::value<string>(&joiner)->default_value(joiner),
          "Scope joiner. String used to conjoin scope hierarchies.")
      ("prefix,p",
          po::value<string>(&prefix),
          "Output prefix for derived files. This path is prepended to "
          "the input file path to set the output path prefix.")
      ("prefix-ipp,a",
          po::value<int>(&prefix_ipp),
          "Number of parent directory levels of the input file path to "
          "append to the specified prefix.")
      ("prefix-scripts,x",
          po::value<bool>(&prefix_scripts)->default_value(prefix_scripts),
          "Prepend output path prefix to extracted scripts. If false, "
          "all extracted scripts will be written to current directory.\n")
      ("define,D",
          po::value<vector<string> >(&define),
          "One or more environment variable definitions to declare in each "
          "extracted makefile script: (NAME=VALUE).\n")
      ("comments",
          po::value<bool>(&comments)->default_value(comments),
          "Copy comment lines to output: (0|1, yes|no, true|false).")
      ("show",
          po::value<bool>(&show)->default_value(show),
          "Show extracted script(s).")
      ("run",
          po::value<bool>(&run)->default_value(run),
          "Run extracted makefile script(s).")
      ("make",
          po::value<bool>(&make)->default_value(make),
          "Run make on each generated makefile.\n")
      ("target",
          po::value<string>(&target)->default_value(target),
          "Makefile target name to use when running make.\n")
      ("lib-path",
          po::value<string>(&lib_path)->default_value(lib_path),
          "Makefile script library path.")
      ("openscad-path",
          po::value<string>(&openscad_path),
          "OpenSCAD executable path. When specified, "
          "this overrides that defined in the Makefile script library.")
      ("bash-path",
          po::value<string>(&bash_path)->default_value(bash_path),
          "Bash executable path.")
      ("make-path",
          po::value<string>(&make_path)->default_value(make_path),
          "GNU Make executable path.\n")
      ("makefile-ext",
          po::value<string>(&makefile_ext)->default_value(makefile_ext),
          "Makefile file extension.")
      ("mfscript-ext",
          po::value<string>(&mfscript_ext)->default_value(mfscript_ext),
          "Makefile scripts file extension.")
      ("openscad-ext",
          po::value<string>(&openscad_ext)->default_value(openscad_ext),
          "OpenSCAD scripts file extension.\n")
      ("config,c",
          po::value<string>(&config),
          "Configuration file with one or more option value pairs.\n")
      ("write-config,w",
          po::value<bool>(&write_config)->default_value(write_config),
          "Write configuration file with program options.\n")
      ("debug-scanner",
          "Run scanner in debug mode.\n")
      ("verbose,V",
          "Run in verbose mode.")
      ("version,v",
          "Report tool version.")
      ("help,h",
          "Print this help messages.")
    ;

    po::positional_options_description opts_pos;
    opts_pos.add("input", 1);


    ////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    ////////////////////////////////////////////////////////////////////////////
    po::variables_map vm;

    try
    {
      po::store(po::command_line_parser(argc, argv).options(opts)
                  .positional(opts_pos).run(), vm);

      // output help and exit
      if ( vm.count("help") || (argc==1) )
      {
        cout << command_name << " " << PACKAGE_VERSION << endl
             << endl
             <<
  "OpenSCAD Script Extractor and Auto-Make (SEAM) tool. Extracts OpenSCAD\n"
  "and/or Makefile scripts annotated within comment block sections of the\n"
  "specified input file. The extracted Makefile scripts may be run to generate\n"
  "makefiles. Subsequently, make can be run on each generated makefile with a\n"
  "named target to invoke OpenSCAD on the input source and/or extracted source\n"
  "files.\n"

             << endl
             << "Examples:" << endl
             << "  " << command_name << " <input>" << endl
             << "  " << command_name << " <input> --scope <scope> --comments=yes --run=yes" << endl
             << "  " << command_name << " --config <config>" << endl
             << endl
             << opts
             << endl;

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

      // parse configuration file options
      if ( vm.count("config") )
      {
        // notify not called to prevent exception from required program options,
        // get values from variable map directly.
        config = vm["config"].as<string>();

        if ( vm.count("verbose") )
          cout << "reading configuration file: " << config << endl;

        std::ifstream config_file ( config.c_str() );

        if ( config_file.good() ) {
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

    // validate mode
    size_t run_mode;

    if      ( mode.compare(0, mode.length(), "count", 0, mode.length()) == 0 ) {
      run_mode = MODE_COUNT;
    } else if ( mode.compare(0, mode.length(), "extract", 0, mode.length()) == 0 ) {
      run_mode = MODE_EXTRACT;
    } else if ( mode.compare(0, mode.length(), "list", 0, mode.length()) == 0 ) {
      run_mode = MODE_LIST;
    } else if ( mode.compare(0, mode.length(), "return", 0, mode.length()) == 0 ) {
      run_mode = MODE_RETURN;
    } else if ( mode.compare(0, mode.length(), "scopes", 0, mode.length()) == 0 ) {
      run_mode = MODE_SCOPES;
    } else {
      throw logic_error( string("invalid option '--mode=" )
              + mode + "', may be one of ( count | extract | list | return | scopes )" );
    }


    // validate: 'extract' mode
    if ( run_mode & MODE_EXTRACT )
    {
      option_depend( vm, "prefix-ipp", "prefix");

      if ( make && !run )
          throw logic_error( string("Option '--make=yes" )
                + "' requires option '--run=yes'" );

      if ( make && target.empty() )
          throw logic_error( string("Option '--make=yes" )
                + "' requires option '--target=arg'" );
    }

    // validate: 'count', 'list', 'return', or 'scopes' modes
    if ( run_mode & (MODE_COUNT|MODE_LIST|MODE_RETURN|MODE_SCOPES) )
    {
      vector<string> va_v;

      va_v.push_back("prefix");
      va_v.push_back("prefix-ipp");
      va_v.push_back("prefix-scripts");

      va_v.push_back("define");

      va_v.push_back("lib-path");
      va_v.push_back("openscad-path");
      va_v.push_back("bash-path");
      va_v.push_back("make-path");

      va_v.push_back("target");

      va_v.push_back("comments");
      va_v.push_back("show");
      va_v.push_back("run");
      va_v.push_back("make");

      va_v.push_back("write-config");

     // make sure none of these options have been specified
      for( vector<string>::iterator it = va_v.begin(); it != va_v.end(); ++it )
        option_set_conflict( vm, it->c_str(), ", only valid for --mode='extract'");
    }

    // validate: 'list', 'return', or 'scopes' mode
    if ( run_mode & (MODE_LIST|MODE_RETURN|MODE_SCOPES) )
    {
      vector<string> va_v;

      va_v.push_back("debug-scanner");
      va_v.push_back("verbose");

     // make sure none of these options have been specified
      for( vector<string>::iterator it = va_v.begin(); it != va_v.end(); ++it )
        option_set_conflict( vm, it->c_str(),
          ", not valid for --mode='list'|'return'|'scopes'");
    }


    ////////////////////////////////////////////////////////////////////////////
    // setup configurations
    ////////////////////////////////////////////////////////////////////////////
    using namespace boost::filesystem;

    try
    {
      path input_path (input);

      // scope: set to input file stem name when not specified
      if ( !vm.count("scope") ) {
        if ( vm.count("verbose")  )
          cout << "** root scope unspecified, assigning input file stem name..."
               << endl;

        scope = input_path.stem().string();

        // insert change into variable map
        vm.insert(make_pair("scope", po::variable_value(scope, true)));
      }

      // output prefix
      if ( vm.count("prefix") ) {

        // remove all relational operators (dot + dot-dot)
        path ido_path;
        int ido_pl=0;
        for (path::const_iterator pi=input_path.begin(); pi != input_path.end(); ++pi) {
          if ( pi->string().compare(".") && pi->string().compare("..") ) {
            ido_path /= *pi;
            ido_pl++;
          }
        }

        // input parent path
        path ipp_path;
        if ( vm.count("prefix-ipp") ) {
          // prune path according to prefix_ipp
          int pl=1;
          for (path::const_iterator pi=ido_path.begin(); pi != ido_path.end(); ++pi) {
            if ( pl >= (ido_pl - prefix_ipp) ) {
              ipp_path /= *pi;
            }
            pl++;
          }
        } else {
          // otherwise use full path
          ipp_path = ido_path;
        }

        path tmp_path;

        tmp_path /= prefix;
        tmp_path /= ipp_path;

        output_prefix = tmp_path.parent_path().string();

        // insert change into variable map
        vm.insert(make_pair("output-prefix", po::variable_value(output_prefix, false)));
      } else {
        if ( vm.count("verbose")  )
          cout << "** prefix unspecified, using input file path..." << endl;

        output_prefix = input_path.parent_path().string();

        // insert change into variable map
        vm.insert(make_pair("prefix", po::variable_value(input_path.parent_path().string(), true)));
        vm.insert(make_pair("output-prefix", po::variable_value(input_path.parent_path().string(), true)));
      }

      // notify variable map changes
      po::notify(vm);
    }
    catch (const filesystem_error& ex)
    {
      cout << ex.what() << endl;
    }


    ////////////////////////////////////////////////////////////////////////////
    // show configuration
    ////////////////////////////////////////////////////////////////////////////

    vector<po_modes> ov;

    ov.push_back((po_modes){1, "mode", "run mode", MODE_ALL});
    ov.push_back((po_modes){1, "input", "input", MODE_ALL});
    ov.push_back((po_modes){1, "scope", "root scope", MODE_ALL});
    ov.push_back((po_modes){2, "joiner", "scope joiner", MODE_ALL});
    ov.push_back((po_modes){0, "scope-id", "scope identifiers", MODE_EXTRACT});
    ov.push_back((po_modes){1, "prefix", "prefix", MODE_EXTRACT});
    ov.push_back((po_modes){0, "prefix-ipp", "prefix ipp", MODE_EXTRACT});
    ov.push_back((po_modes){0, "prefix-scripts", "prefix scripts", MODE_EXTRACT});
    ov.push_back((po_modes){1, "output-prefix", "output prefix", MODE_EXTRACT});
    ov.push_back((po_modes){1, "define", "define", MODE_EXTRACT});
    ov.push_back((po_modes){1, "comments", "comments", MODE_EXTRACT});
    ov.push_back((po_modes){0, "show", "show", MODE_EXTRACT});
    ov.push_back((po_modes){0, "run", "run mfscripts", MODE_EXTRACT});
    ov.push_back((po_modes){0, "make", "run make", MODE_EXTRACT});
    ov.push_back((po_modes){1, "target", "make target", MODE_EXTRACT});
    ov.push_back((po_modes){1, "lib-path", "lib path", MODE_EXTRACT});
    ov.push_back((po_modes){0, "openscad-path", "openscad path", MODE_EXTRACT});
    ov.push_back((po_modes){0, "bash-path", "bash path", MODE_EXTRACT});
    ov.push_back((po_modes){0, "make-path", "make path", MODE_EXTRACT});
    ov.push_back((po_modes){1, "makefile-ext", "makefile ext", MODE_EXTRACT | MODE_COUNT});
    ov.push_back((po_modes){0, "mfscript-ext", "mfscript ext", MODE_EXTRACT | MODE_COUNT});
    ov.push_back((po_modes){0, "openscad-ext", "openscad ext", MODE_EXTRACT | MODE_COUNT});
    ov.push_back((po_modes){1, "config", "config file", MODE_ALL});
    ov.push_back((po_modes){0, "write-config", "write config file", MODE_EXTRACT});
    ov.push_back((po_modes){1, "debug-scanner", "debug scanner", MODE_ALL});
    ov.push_back((po_modes){0, "verbose", "verbose", MODE_ALL});

    if ( vm.count("verbose")  ) {
      format_options( cout, "configuration:", "**", ov, run_mode, vm );
    }


    ////////////////////////////////////////////////////////////////////////////
    // setup and run scanner
    ////////////////////////////////////////////////////////////////////////////

    //
    // 'count', 'list', 'return', or 'scopes' mode
    //
    if ( run_mode & (MODE_COUNT|MODE_LIST|MODE_RETURN|MODE_SCOPES) )
    {
      SEAM::SEAM_Scanner scanner( input, true, command_name + ": " );

      scanner.set_rootscope( scope );
      scanner.set_scopejoiner( joiner );

      scanner.set_makefile_ext( makefile_ext );
      scanner.set_mfscript_ext( mfscript_ext );
      scanner.set_openscad_ext( openscad_ext );

      scanner.set_debug( vm.count("debug-scanner")>0 );
      scanner.set_verbose( vm.count("verbose")>0 );

      while( scanner.scan() != 0 )
        ;

      int script_count = scanner.get_script_count();

      if ( run_mode & (MODE_COUNT|MODE_LIST|MODE_SCOPES) )
      {

        if ( run_mode & (MODE_COUNT) )
        {
          cout << "script count = " <<  script_count << endl;
        }

        if ( run_mode & (MODE_LIST) )
        {
          vector<string> sv = scanner.get_script_id();
          for ( vector<string>::iterator it=sv.begin(); it != sv.end(); ++it)
            cout << *it << endl;
        }

        if ( run_mode & (MODE_SCOPES) )
        {
          vector<string> sv = scanner.get_scope_id();
          for ( vector<string>::iterator it=sv.begin(); it != sv.end(); ++it)
            cout << *it << endl;
        }

        if ( script_count == 0 )
          exit( ERROR_SCRIPT_COUNT_ZERO );
        else
          exit( SUCCESS );
      }
      else
      {
        exit( script_count );
      }
    }
    //
    // 'extract' mode
    //
    else
    {
      SEAM::SEAM_Scanner scanner( input, false, command_name + ": " );

      scanner.set_rootscope( scope );
      scanner.set_scopejoiner( joiner );
      scanner.set_output_prefix( output_prefix );
      scanner.set_prefix_scripts( prefix_scripts );

      scanner.set_define( define );

      scanner.set_lib_path( lib_path );
      scanner.set_openscad_path( openscad_path );
      scanner.set_bash_path( bash_path );
      scanner.set_make_path( make_path );

      scanner.set_makefile_ext( makefile_ext );
      scanner.set_mfscript_ext( mfscript_ext );
      scanner.set_openscad_ext( openscad_ext );

      scanner.set_target( target );

      scanner.set_comments( comments );
      scanner.set_show( show );
      scanner.set_run( run );
      scanner.set_make( make );

      scanner.set_debug( vm.count("debug-scanner")>0 );
      scanner.set_verbose( vm.count("verbose")>0 );

      while( scanner.scan() != 0 )
        ;

      // add the vector of scope identifiers to the program options
      // to be written to the configuration file.
      if ( write_config )
      {
        vm.insert( make_pair( "scope-id",
                              po::variable_value(scanner.get_scope_id(), true)
                            )
                 );
      }
    }


    ////////////////////////////////////////////////////////////////////////////
    // write run configuration
    ////////////////////////////////////////////////////////////////////////////

    if ( write_config )
    {
      path input_path ( input );
      path conf_path;

      // configuration file name
      if ( prefix_scripts ) {
        conf_path /= output_prefix;
        conf_path /= input_path.stem();
      } else {
        conf_path += input_path.stem();
      }

      conf_path += ".conf";

      if ( vm.count("verbose")  )
        cout << "writing configuration file: " << conf_path.string() << endl;

      std::ofstream config_file ( conf_path.c_str() );

      if ( config_file.good() ) {
        format_options( config_file, "configuration:", "#", ov, run_mode, vm );
        write_options( config_file, vm) ;
      } else {
        cerr << "ERROR: unable to open configuration file [" << conf_path.string()
             << "]" << endl;

        exit( ERROR_UNABLE_TO_OPEN_FILE );
      }
      config_file.close();
    }

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
