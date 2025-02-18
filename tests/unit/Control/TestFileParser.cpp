// *****************************************************************************
/*!
  \file      tests/unit/Control/TestFileParser.cpp
  \copyright 2012-2015 J. Bakosi,
             2016-2018 Los Alamos National Security, LLC.,
             2019-2021 Triad National Security, LLC.
             All rights reserved. See the LICENSE file for details.
  \brief     Unit tests for Control/FileParser
  \details   Unit tests for Control/FileParser
*/
// *****************************************************************************

#include "NoWarning/tut.hpp"

#include "TUTConfig.hpp"
#include "FileParser.hpp"
#include "Print.hpp"

namespace unittest {

extern std::string g_executable;

} // unittest::

#ifndef DOXYGEN_GENERATING_OUTPUT

namespace tut {

//! All tests in group inherited from this base
struct FileParser_common {
  // tk::FileParser only has a protected constructor: designed to be used as a
  // base class
  struct parser : tk::FileParser {
    explicit parser( const std::string& f ) : FileParser( f ) {}
    void diagnostics( const tk::Print& print,
                      const std::vector< std::string >& messages ) {
      tk::FileParser::diagnostics( print, messages );
    }
  };
};

//! Test group shortcuts
using FileParser_group = test_group< FileParser_common, MAX_TESTS_IN_GROUP >;
using FileParser_object = FileParser_group::object;

//! Define test group
static FileParser_group FileParser( "Control/FileParser" );

//! Test definitions for group

//! Test if constructor finds and can open an existing file (the executable)
template<> template<>
void FileParser_object::test< 1 >() {
  set_test_name( "ctor finds and opens executable" );

  // Throws exception if something goes wrong, which Template Unit Test catches
  parser p( unittest::g_executable );
}

//! Test if constructor throws an exception if empty filename is given
template<> template<>
void FileParser_object::test< 2 >() {
  set_test_name( "ctor throws if filename empty" );

  // Correctly throws exception in DEBUG mode if empty filename string is given
  try {

    parser p( "" );

  } catch( tk::Exception& e ) {
    // exception thrown, test ok
    // find  out if exception was thrown due to the correct reason: testing on
    // whether the filename is empty is an Assert and compiled away in RELEASE
    // mode, in which case the ErrChk throws when it fails to open the file
    #ifdef NDEBUG
    ensure( std::string("wrong exception thrown: ") + e.what(),
            std::string( e.what() ).find( "Failed to open file" ) !=
              std::string::npos );
    #else
    ensure( std::string("wrong exception thrown: ") + e.what(),
            std::string( e.what() ).find( "No filename specified" ) !=
              std::string::npos );
    #endif
  } // if any other type of exception is thrown, test fails with except
}

//! Test if constructor throws exception if file does not exist
template<> template<>
void FileParser_object::test< 3 >() {
  set_test_name( "ctor throws if file doesn't exist" );

  // Correctly throws exception if file does not exist
  try {

    parser p( "very_little_chance_that_a_file_with_this_name_exists" );

  } catch( tk::Exception& e ) {
    // exception thrown, test ok
    // find  out if exception was thrown due to the correct reason
    ensure( std::string("wrong exception thrown: ") + e.what(),
            std::string( e.what() ).find( "Failed to open file" ) !=
              std::string::npos ||
            std::string( e.what() ).find( "Failed to read from file" ) !=
              std::string::npos );
  } // if any other type of exception is thrown, test fails with except
}

//! Test if constructor throws exception if cannot read from file
template<> template<>
void FileParser_object::test< 4 >() {
  set_test_name( "ctor throws if cannot read from file" );

  // Correctly throws exception if cannot read from file
  try {

    parser p( "." );    // Nasty: trying open an existing directory as a file

  } catch( tk::Exception& e ) {
    // exception thrown, test ok
    // find  out if exception was thrown due to the correct reason
    ensure( std::string("wrong exception thrown: ") + e.what(),
            std::string( e.what() ).find( "Failed to read from file" ) !=
              std::string::npos );
  } // if any other type of exception is thrown, test fails with except
}

//! Test if function diagnostics() throws exception if error occurred
template<> template<>
void FileParser_object::test< 5 >() {
  set_test_name( "diagnostics throws on error" );

  // Correctly throws exception on error
  try {

    // Open some file, does not matter what
    parser p( unittest::g_executable );
    // Feed "Error" to diagnostics()
    p.diagnostics( tk::Print(), { { "Error" } } );
    fail( "should throw exception" );

  } catch( tk::Exception& e ) {
    // exception thrown, test ok
    // if any other type of exception is thrown, test fails with except
    // find  out if exception was thrown due to the correct reason
    ensure( std::string("wrong exception thrown: ") + e.what(),
            std::string( e.what() ).find( "Error(s) occurred while parsing" ) !=
              std::string::npos );
  }
}

//! Test if function diagnostics() does not throw exception if warning occurred
template<> template<>
void FileParser_object::test< 6 >() {
  set_test_name( "diagnostics doesn't throw on warning" );

  // Does not throw exception on warning
  try {

    // Open some file, does not matter what
    parser p( unittest::g_executable );
    // Feed "Warning" to diagnostics()
    p.diagnostics( tk::Print(), { { "Warning" } } );

  } catch( tk::Exception& ) {
    fail( "should not throw exception" );
  } // if any other type of exception is thrown, test fails with except
}

//! Test if function diagnostics() does not break on empty messages vector
template<> template<>
void FileParser_object::test< 7 >() {
  set_test_name( "diagnostics with no messages" );

  // Does not throw exception on warning
  try {

    // Open some file, does not matter what
    parser p( unittest::g_executable );
    // Feed empty vector to diagnostics()
    p.diagnostics( tk::Print(), { {  } } );

  } catch( tk::Exception& ) {
    fail( "should not throw exception" );
  } // if any other type of exception is thrown, test fails with except
}

} // tut::

#endif  // DOXYGEN_GENERATING_OUTPUT
