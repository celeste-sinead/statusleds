/******************************************************************************
 * main.cpp
 * Copyright 2010 Iain Peet
 *
 * Statusled daemon main entry point.
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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <iostream>
#include <iomanip>

#include "cpustat.h"

using namespace std;

int main(int argc, char *argv[])
{
    CPUStat cpustat;
    if(cpustat.update()) {
        fprintf(stderr, "Failed to obtain cpu utilization.");
    }
    cout << setprecision(3);
    while(1) {
        sleep(1);
        cpustat.update();
        cout << "Total: " << setw(5) << cpustat.totalDiff().getUtilization() * 100 << "%";
        for(int i=0; i<cpustat.cpuCount(); ++i) {
            cout << " CPU " << i << ": ";
            cout << setw(5) << cpustat.cpuDiff(i).getUtilization() * 100 << "%";
        }
        cout << endl;
    }
    return 0;
}

