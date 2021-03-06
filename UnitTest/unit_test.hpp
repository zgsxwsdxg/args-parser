
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2013-2017 Igor Mironchik

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef UNITTEST__UNIT_TEST_HPP__INCLUDED
#define UNITTEST__UNIT_TEST_HPP__INCLUDED

// C++ include.
#include <string>
#include <map>
#include <list>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <exception>
#include <iostream>


//
// Test
//

//! Test.
class Test {
public:
	explicit Test( const std::string & name );

	virtual ~Test();

	//! \return name of the test.
	const std::string & name() const;

	//! Run test.
	void runTest();

	//! Test body.
	virtual void testBody() = 0;

private:
	//! Name of the test.
	std::string m_name;
}; // class Test


//
// TestInfo
//

//! Test info.
class TestInfo final {
public:
	TestInfo( const std::string & testCaseName,
		std::shared_ptr< Test > test );
}; // class TestInfo


//
// TestCase
//

//! Test case.
class TestCase final {
public:
	explicit TestCase( const std::string & name );

	~TestCase();

	//! \return Name of the test case.
	const std::string & name() const;

	//! Add new test.
	void addTest( std::shared_ptr< Test > test );

	//! Run all tests.
	void runAllTests();

private:
	TestCase( const TestCase & );

	TestCase & operator= ( const TestCase & );

private:
	//! Tests.
	std::list< std::shared_ptr< Test > > m_tests;
	//! Test case name.
	std::string m_name;
}; // class TestCase


//
// UnitTest
//

//! Unit test.
class UnitTest final {
private:
	UnitTest();

	~UnitTest();

	UnitTest( const UnitTest & );

	UnitTest & operator= ( const UnitTest & );

public:
	//! \return Instance of the singleton of this class.
	static UnitTest & instance();

	//! \return Test case for the given \arg name.
	TestCase * createTestCaseIfNotExists( const std::string & name );

	//! Run all tests.
	void runAllTests();

private:
	//! Test cases.
	std::map< std::string, std::shared_ptr< TestCase > > m_testCases;
}; // class UnitTest


//
// TEST
//

#define TEST( TestCaseName, TestName ) \
class TestName##Class final \
	:	public Test \
{ \
public: \
	TestName##Class() \
		:	Test( #TestName ) \
	{ \
	} \
 \
	~TestName##Class() \
	{ \
	} \
 \
	void testBody(); \
 \
	static TestInfo testInfo; \
}; \
 \
TestInfo TestName##Class::testInfo = \
	TestInfo( #TestCaseName, std::make_shared< TestName##Class > () ); \
void TestName##Class::testBody()


//
// RUN_ALL_TESTS
//

#define RUN_ALL_TESTS() \
	UnitTest::instance().runAllTests();


//
// CHECK_CONDITION
//

#define CHECK_CONDITION( Condition ) \
	if( !(Condition) ) \
	{ \
		std::ostringstream stream; \
		stream << "     file: \"" << __FILE__ \
			<< "\" line: " << __LINE__ << "\n" \
			<< "     ( " << #Condition << " ) [FAILED]"; \
 \
		throw std::runtime_error( stream.str() ); \
	}


//
// CHECK_THROW
//

#define CHECK_THROW( Func, Exception ) \
{ \
	bool exception_thrown__ = false; \
	try { \
		Func; \
	} \
	catch( const Exception & ) { \
		exception_thrown__ = true; \
	} \
	catch( const std::exception & x ) { \
		std::ostringstream stream; \
		stream << "     file: \"" << __FILE__ \
			<< "\" line: " << __LINE__ << "\n" \
			<< "     expected exception: " << #Exception << "\n" \
			<< "     but caught: " << typeid( x ).name() << " [FAILED]"; \
 \
		throw std::runtime_error( stream.str() ); \
	} \
	catch( ... ) { \
		std::ostringstream stream; \
		stream << "     file: \"" << __FILE__ \
			<< "\" line: " << __LINE__ << "\n" \
			<< "     expected exception: " << #Exception << "\n" \
			<< "     but caught: unknown exception [FAILED]"; \
 \
		throw std::runtime_error( stream.str() ); \
	} \
	if( !exception_thrown__ ) { \
		std::ostringstream stream; \
		stream << "     file: \"" << __FILE__ \
			<< "\" line: " << __LINE__ << "\n" \
			<< "     expected exception: " << #Exception << "\n" \
			<< "     but nothing thrown [FAILED]"; \
 \
		throw std::runtime_error( stream.str() ); \
	} \
}


//
// Test
//

inline
Test::Test( const std::string & name )
	:	m_name( name )
{
}

inline
Test::~Test()
{
}

inline const std::string &
Test::name() const
{
	return m_name;
}

inline void
Test::runTest()
{
	try {
		testBody();
	}
	catch( const std::exception & x )
	{
		std::cout << "[FAILED]" << std::endl << std::endl;
		std::cout << x.what() << std::endl;

		exit( 1 );
	}
	catch( ... )
	{
		std::cout << "[FAILED]" << std::endl << std::endl;
		std::cout << "  Unexpected exception." << std::endl;

		exit( 1 );
	}
}


//
// TestInfo
//

inline
TestInfo::TestInfo( const std::string & testCaseName,
	std::shared_ptr< Test > test )
{
	UnitTest::instance().createTestCaseIfNotExists( testCaseName )->
		addTest( test );
}


//
// TestCase
//

inline
TestCase::TestCase( const std::string & name )
	:	m_name( name )
{
}

inline
TestCase::~TestCase()
{
}

inline const std::string &
TestCase::name() const
{
	return m_name;
}

inline void
TestCase::addTest( std::shared_ptr< Test > test )
{
	m_tests.push_back( test );
}

inline void
TestCase::runAllTests()
{
	std::cout << "Test case \"" << name() << "\" started..." << std::endl;

	size_t i = 1;

	for( auto & t : m_tests )
	{
		std::cout << " ";

		std::cout.width( 2 );
		std::cout.fill( '0' );

		std::cout << i << ". \""
			<< t->name() << "\"... ";

		t->runTest();

		std::cout << "[OK]" << std::endl;

		++i;
	}

	std::cout << "Test case \"" << name() << "\" finished..." << std::endl
		<< std::endl;
}


//
// UnitTest
//

inline
UnitTest::UnitTest()
{
}

inline
UnitTest::~UnitTest()
{
}

inline UnitTest &
UnitTest::instance()
{
	static UnitTest unit;

	return unit;
}

inline TestCase *
UnitTest::createTestCaseIfNotExists( const std::string & name )
{
	auto it = m_testCases.find( name );

	if( it == m_testCases.end() )
	{
		auto p = m_testCases.insert(
			std::pair< std::string, std::shared_ptr< TestCase > >( name,
				std::make_shared< TestCase > ( name ) ) );

		it = p.first;
	}

	return it->second.get();
}

inline void
UnitTest::runAllTests()
{
	for( auto & p : m_testCases )
		p.second->runAllTests();
}

#endif // UNITTEST__UNIT_TEST_HPP__INCLUDED
