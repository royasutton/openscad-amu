/// This is the brief.
/***************************************************************************//**

  \file   openscad_seam_main.cpp

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

  \details

  \todo add command line option to extract scripts that match specific
        scope name and script type: {MFScript and/or Openscad}.

*******************************************************************************/

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include "openscad_seam_scanner.hpp"

namespace
{
  const size_t SUCCESS = 0;
  const size_t ERROR_SCRIPT_COUNT_ZERO = 1;
  const size_t ERROR_IN_COMMAND_LINE = 2;
  const size_t ERROR_UNABLE_TO_OPEN_FILE = 3;
  const size_t ERROR_UNHANDLED_EXCEPTION = 4;
}

using namespace std;


/***************************************************************************//**
  \breif check if an option is set and throws exception if so.
  \private
*******************************************************************************/
void option_set_conflict(
  const boost::program_options::variables_map& vm,
  const char* opt,
  const char* msg)
{
  if ( vm.count(opt) && !vm[opt].defaulted() )
    throw std::logic_error( std::string("Conflicting option '--")
          + opt + "'" + msg );
}


/***************************************************************************//**
  \breif check if both options are set and throws exception if so.
  \private
*******************************************************************************/
void option_conflict(
  const boost::program_options::variables_map& vm,
  const char* opt1,
  const char* opt2)
{
  if ( vm.count(opt1) && !vm[opt1].defaulted()
    && vm.count(opt2) && !vm[opt2].defaulted() )
    throw std::logic_error( std::string("Conflicting options '--")
          + opt1 + "' and '--" + opt2 + "'" );
}


/***************************************************************************//**
  \breif check for opt2 if opt1 is set and throws exception if not.
  \private
*******************************************************************************/
void option_depend(
  const boost::program_options::variables_map& vm,
  const char* opt1,
  const char* opt2)
{
  if ( vm.count(opt1) && !vm[opt1].defaulted() )
    if ( vm.count(opt2) == 0 || vm[opt2].defaulted() )
      throw std::logic_error( std::string("Option '--" ) + opt1
            + "' requires option '--" + opt2 + "'" );
}

/***************************************************************************//**
  \breif main program.
  \public
*******************************************************************************/
int main(int argc, char** argv)
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

    namespace po = boost::program_options;

    po::options_description opts("Options");
    opts.add_options()
      ("mode,m",
          po::value<string>(&mode)->default_value(mode),
          "Scanner mode: one of (count | extract | return). Count "
          "outputs the number of scripts found in the input. Return "
          "sets the command exit value to this count. Extract is the "
          "default mode.\n")
      ("input,i",
          po::value<string>(&input)->required(),
          "Input file containing annotated script(s) embedded within "
          "comments.\n")
      ("scope,s",
          po::value<string>(&scope),
          "Scope root name. When not specified, the input file stem "
          "name is used.")
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
          "Makefile file extention.")
      ("mfscript-ext",
          po::value<string>(&mfscript_ext)->default_value(mfscript_ext),
          "Makefile scripts file extention.")
      ("openscad-ext",
          po::value<string>(&openscad_ext)->default_value(openscad_ext),
          "OpenSCAD scripts file extention.\n")
      ("config,c",
          po::value<string>(&config),
          "Configuration file with one or more option value pairs.\n")
      ("debug",
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
      if ( vm.count("help") || (argc==1) ) {
        cout << command_name << " v" << PACKAGE_VERSION << endl
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
      if ( vm.count("version")  ) {

        if ( vm.count("verbose")  )
          cout << command_name << " " << PACKAGE_VERSION << endl
               << endl
               << "       package: " << PACKAGE_NAME << endl
               << "       version: " << PACKAGE_VERSION << endl
               << "    bug report: " << PACKAGE_BUGREPORT << endl
               << "      site url: " << PACKAGE_URL << endl
               << endl
               << "      lib path: " << __LIB_PATH__ << endl
               << " openscad path: " << __OPENSCAD_PATH__ << endl
               << "     bash path: " << __BASH_PATH__ << endl
               << " gnu make path: " << __MAKE_PATH__ << endl
               << endl;
        else
          cout << PACKAGE_VERSION << endl;

        exit( SUCCESS );
      }

      // parse options from supplied config file
      if ( vm.count("config")  ) {
        config = vm["config"].as<string>();

        ifstream config_file ( config.c_str() );

        if ( config_file.good() ) {
          if ( vm.count("verbose")  )
            cout << "reading configuration file " << config << endl;

          po::store(po::parse_config_file(config_file, opts), vm);
        } else {
          cerr << "ERROR: unable to open config file [" << config
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
    char mode_char;

    if      ( mode.compare(0, mode.length(), "count", 0, mode.length()) == 0 )
      mode_char = 'c';
    else if ( mode.compare(0, mode.length(), "extract", 0, mode.length()) == 0 )
      mode_char = 'e';
    else if ( mode.compare(0, mode.length(), "return", 0, mode.length()) == 0 )
      mode_char = 'r';
    else
      throw std::logic_error( std::string("invalid option '--mode=" )
              + mode + "', may be one of ( count | extract | return )" );

    // validate for 'extract' mode
    if ( mode_char == 'e' )
    {
      option_depend( vm, "prefix-ipp", "prefix");

      if ( make && !run )
          throw std::logic_error( std::string("Option '--make=yes" )
                + "' requires option '--run=yes'" );

      if ( make && target.empty() )
          throw std::logic_error( std::string("Option '--make=yes" )
                + "' requires option '--target=arg'" );
    }

    // validate for 'count' or 'return' modes
    if ( mode_char == 'c'  || mode_char == 'r' )
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

     // make sure none of these options have been specified
      for( vector<string>::iterator it = va_v.begin(); it != va_v.end(); ++it )
        option_set_conflict( vm, it->c_str(), ", only valid for --mode='extract'");
    }

    // validate for 'return' mode
    if ( mode_char == 'r' )
    {
      vector<string> va_v;

      va_v.push_back("debug");
      va_v.push_back("verbose");

     // make sure none of these options have been specified
      for( vector<string>::iterator it = va_v.begin(); it != va_v.end(); ++it )
        option_set_conflict( vm, it->c_str(), ", not valid for --mode='return'");
    }


    ////////////////////////////////////////////////////////////////////////////
    // setup configurations
    ////////////////////////////////////////////////////////////////////////////
    using namespace boost::filesystem;

    try
    {
      path input_path (input);

      // scope - set to input file stem name when not specified
      if ( !vm.count("scope") ) {
        if ( vm.count("verbose")  )
          cout << "** root scope unspecified, assigning input file stem name..."
               << endl;

        scope = input_path.stem().string();
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
      } else {
        if ( vm.count("verbose")  )
          cout << "** prefix unspecified, using input file path..." << endl;

        output_prefix = input_path.parent_path().string();
      }
    }
    catch (const filesystem_error& ex)
    {
      cout << ex.what() << endl;
    }


    ////////////////////////////////////////////////////////////////////////////
    // show configuration
    ////////////////////////////////////////////////////////////////////////////
    #define MF_FKEY( k ) \
            ( \
              string("**                ").substr( 0, (17-string(k).length()) ) + \
              string(k) + \
              string(": ") \
            )
    #define MF_ENDL ("**\n")
    #define MF_BOOL( b ) ((b==true) ? "yes" : "no")
    #define MF_SSTR( s ) (s.empty() ? "*<not specified>*" : s)
    #define MF_SINT( i ) \
            ( (i==-1) \
              ? "*<not specified>*" \
              : static_cast<ostringstream &>((ostringstream()<<dec<<i)).str() \
            )

    if ( vm.count("verbose")  )
    {
      cout  << "** configuration:" << endl
            << MF_ENDL
            << MF_FKEY("mode") << MF_SSTR(mode) << endl
            << MF_ENDL
            << MF_FKEY("input") << MF_SSTR(input) << endl
            << MF_ENDL
            << MF_FKEY("root scope") << MF_SSTR(scope) << endl
            << MF_FKEY("prefix") << MF_SSTR(prefix) << endl
            << MF_FKEY("prefix-ipp") << MF_SINT(prefix_ipp) << endl
            << MF_FKEY("prefix-scripts") << MF_BOOL(prefix_scripts) << endl
            << MF_FKEY("output prefix") << MF_SSTR(output_prefix) << endl
            << MF_ENDL;

      if ( define.size() > 0 )
      {
        for(size_t i=0; i<define.size(); ++i) {
          cout << MF_FKEY("define") << MF_SSTR(define[i]) << endl;
        }
        cout << MF_ENDL;
      }

       cout << MF_FKEY("comments") << MF_BOOL(comments) << endl
            << MF_FKEY("show") << MF_BOOL(show) << endl
            << MF_FKEY("run mfscripts")   << MF_BOOL(run) << endl
            << MF_FKEY("run make")   << MF_BOOL(make) << endl
            << MF_ENDL
            << MF_FKEY("make target") << MF_SSTR(target) << endl
            << MF_ENDL
            << MF_FKEY("lib path") << MF_SSTR(lib_path) << endl
            << MF_FKEY("openscad path") << MF_SSTR(openscad_path) << endl
            << MF_FKEY("bash path") << MF_SSTR(bash_path) << endl
            << MF_FKEY("make path") << MF_SSTR(make_path) << endl
            << MF_ENDL
            << MF_FKEY("makefile ext") << MF_SSTR(makefile_ext) << endl
            << MF_FKEY("mfscript ext") << MF_SSTR(mfscript_ext) << endl
            << MF_FKEY("openscad ext") << MF_SSTR(openscad_ext) << endl
            << MF_ENDL
            << MF_FKEY("config file") << MF_SSTR(config) << endl
            << MF_ENDL
            << MF_FKEY("debug") << MF_BOOL((vm.count("debug")>0)) << endl
            << MF_FKEY("verbose") << MF_BOOL((vm.count("verbose")>0)) << endl
            << endl;
    }


    ////////////////////////////////////////////////////////////////////////////
    // setup and run scanner
    ////////////////////////////////////////////////////////////////////////////

    //
    // 'count' or 'return' mode
    //
    if ( mode_char == 'c' || mode_char == 'r' )
    {
      SEAM::SEAM_Scanner scanner( input, true, command_name + ": " );

      scanner.set_rootscope( scope );

      scanner.set_makefile_ext( makefile_ext );
      scanner.set_mfscript_ext( mfscript_ext );
      scanner.set_openscad_ext( openscad_ext );

      scanner.set_debug( vm.count("debug")>0 );
      scanner.set_verbose( vm.count("verbose")>0 );

      while( scanner.scan() != 0 )
        ;

      int script_count = scanner.get_script_count();

      if ( mode_char == 'c' )
      {
        cout << "script count = " <<  script_count << endl;

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

      scanner.set_debug( vm.count("debug")>0 );
      scanner.set_verbose( vm.count("verbose")>0 );

      while( scanner.scan() != 0 )
        ;
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


/*******************************************************************************
// eof
*******************************************************************************/
