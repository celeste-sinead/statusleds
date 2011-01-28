/******************************************************************************
 * meminfo.cpp
 * Copyright 2010 Iain Peet
 *
 * Obtains some basic memory utilization information from /proc/meminfo
 ******************************************************************************
 * This program is distributed under the of the GNU Lesser Public License. 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>

#include "meminfo.h"

using namespace std;

int Meminfo::update()
{
    fstream procMeminfo ("/proc/meminfo", ios_base::in);
    if( procMeminfo.fail() ) {
        cout << "Failed to open /proc/meminfo" << endl;
        return -1;
    }

    /* Run through /proc/meminfo, seeking the info we actually care about */
    string key;
    while( ! procMeminfo.eof() ) {
        procMeminfo >> key; 
        // Usually caused by empty line at eof:
        if(procMeminfo.fail()) break;

        /* Is this a key we're looking for? */
        if( key == "MemTotal:" ) {
            procMeminfo >> m_total;
        } else if( key == "MemFree:" ) {
            procMeminfo >> m_free;
        } else if( key == "Buffers:" ) {
            procMeminfo >> m_buffers;
        } else if( key == "Cached:" ) {
            procMeminfo >> m_cached;
        }

        clearLine(procMeminfo);
    }

    return 0;
}

double Meminfo::getUtilization()
{
    double used = m_total - m_free - m_buffers - m_cached;
    return used / m_total;
}

void Meminfo::clearLine(std::istream& stream)
{
    char ch;
    do {
        stream.get(ch);
    } while( (!stream.eof()) && (ch != '\n') );
}


