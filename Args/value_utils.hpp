
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

#ifndef ARGS__VALUE_UTILS_HPP__INCLUDED
#define ARGS__VALUE_UTILS_HPP__INCLUDED

// Args include.
#include "utils.hpp"
#include "exceptions.hpp"
#include "types.hpp"

// C++ include.
#include <algorithm>


namespace Args {

//
// eatValues
//

//! Eat values in context.
template< typename Container, typename Cmd, typename Ctx >
bool eatValues( Ctx & context, Container & container,
	const String & errorDescription, Cmd * cmdLine )
{
	if( !cmdLine )
		throw BaseException( SL( "Argument is not under command line parser." ) );

	if( !context.atEnd() )
	{
		auto begin = context.begin();

		auto last = std::find_if( context.begin(), context.end(),
			[ & ] ( const String & v ) -> bool
			{
				if( isArgument( v ) || isFlag( v ) )
					return true;
				else
				{
					try {
						cmdLine->findArgument( v );

						return true;
					}
					catch( const BaseException & )
					{
						return false;
					}
				}
			}
		);

		if( last != begin )
		{
			begin = context.next();

			while( begin != last )
			{
				container.push_back( *begin );

				begin = context.next();
			}

			if( last != context.end() )
				context.putBack();

			return true;
		}
		else
			throw BaseException( errorDescription );
	}
	else
		throw BaseException( errorDescription );
}


//
// eatOneValue
//

//! Eat one value.
template< typename Cmd, typename Ctx >
String eatOneValue( Ctx & context, Cmd * cmdLine )
{
	if( !cmdLine )
		throw BaseException( SL( "Argument is not under command line parser." ) );

	if( !context.atEnd() )
	{
		auto val = context.next();

		if( !isArgument( *val ) && !isFlag( *val ) )
		{
			try {
				cmdLine->findArgument( *val );
			}
			catch( const BaseException & )
			{
				return *val;
			}
		}

		context.putBack();

		throw BaseException( SL( "Can't eat value." ) );
	}
	else
		throw BaseException( SL( "Can't eat value." ) );
}

} /* namespace Args */

#endif // ARGS__VALUE_UTILS_HPP__INCLUDED
