//////////////////////////////////////////////////////////////////////
// Eternity persistence system
//////////////////////////////////////////////////////////////////////
// 
// 1999-2003 Nicola Santi.
//
// This software is released under the terms of Eternity Artistic 
// License. Please read the file eternity-licence to learn details. 
// 
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
// WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Except as contained in this notice, the name of Nicola Santi
// shall not be used in advertising or otherwise to promote the sale, 
// use or other dealings in this Software without prior written 
// authorization from Nicola Santi.
//////////////////////////////////////////////////////////////////////

#include "eternity/dynamic.hpp"
#ifndef DYNAMIC_CPP
#define DYNAMIC_CPP

namespace eternity
{
	/*static*/ Ifactory *Ifactory::m_pHead = 0;

	std::string get_conventional_name ( std::string compiler_name )
	{
		std::string conventional_name;
		Ifactory *pCurrent = Ifactory::m_pHead;

    	for( ; pCurrent ; pCurrent = pCurrent->m_pNext )
		{
			conventional_name = pCurrent->get_conventional_name(compiler_name);
			if( !conventional_name.empty() ) break;
		}

		return conventional_name;
	}

	/* move here to fix bcc32 Warning W8026 */
	eternity::exception::exception(std::string swhat ) :m_swhat(swhat) {};
	eternity::exception::~exception( ) throw() {}
	const char* eternity::exception::what( ) const throw() 
	{
		return m_swhat.c_str();
	}


}

#endif // DYNAMIC_CPP 









