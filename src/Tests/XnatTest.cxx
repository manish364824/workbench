/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <vector>

#include "XnatTest.h"
#include "CiftiXnat.h"

using namespace caret;
using namespace std;

XnatTest::XnatTest(const AString& identifier) : TestInterface(identifier)
{
}

void XnatTest::execute()
{
    setFailed("no authentication info present"); return;//comment this out to run the test
    AString myUrl("");//test url goes here
    AString user("");//test auth goes here
    AString pass("");
    CiftiXnat myXnat;
    myXnat.setAuthentication(myUrl, user, pass);
    myXnat.openURL(myUrl);
    vector<float> myData;
    myData.resize(myXnat.getNumberOfColumns());
    myXnat.getRow(myData.data(), 0);
    bool success = myXnat.getRowFromNode(myData.data(), 528, StructureEnum::CORTEX_RIGHT);
    if (!success)
    {
        setFailed("error getting row by node");
    }
}
