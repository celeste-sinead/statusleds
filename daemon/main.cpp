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

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"
#include "cpustat.h"
#include "meminfo.h"
#include "blinky.h"

using namespace std;

// Fork a child, close inherited files, blah blah blah
int daemonize() {
    int ret = fork();
    if (ret < 0) {
        // Fork failed.
        return ret;
    }
    if (ret > 0) {
        // This is parent, exit now.
        exit(0);
    }

    /* Child process logic */
    umask(0);

    int sid = setsid();  // separate from parent.
    if (sid < 0) {
        // Failed to separate.
        return -1;
    }

    /* Redirect standard I/O to null */
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    return 0;
}

// Print usage message
void usage(const char *bin) {
    cout << "Usage:" << endl;
    cout << bin << " [-f] -p port" << endl;
    cout << "-f  Run in foreground" << endl;
    cout << "-p  Specify serial port to use to commmunicate with blinky" << endl;
}

int main(int argc, char *argv[])
{
    /* Parse args */
    bool shouldDaemonize = true;
    char* blinkyPort = 0;
    for (int i=1; i < argc; ++i) {
        if (strcmp("-f", argv[i]) == 0) {
            shouldDaemonize = false;
        } else if (strcmp("-p", argv[i]) == 0) {
            if (i+1 >= argc) {
                usage(argv[0]);
                exit(1);
            }
            blinkyPort = argv[++i];
        } else {
            usage(argv[0]);
            exit(1);
        }
    }

    if (!blinkyPort) {
        usage(argv[0]);
        exit(1);
    }
    
    if (shouldDaemonize && daemonize()) {
        cerr << "Failed to daemonize.  Exiting." << endl;
        exit(1);
    }

    CPUStat cpustat;
    if(cpustat.update()) {
        cerr <<  "Failed to obtain cpu utilization." << endl;
    }

    Meminfo meminfo;
    if(meminfo.update()) {
        cerr << "Failed to obtain memory utilization." << endl;
    }

    Blinky blinky(blinkyPort);
    if( ! blinky.ready() ) {
        cerr << "Failed to open blinky device." << endl;
    }
    if( ! blinky.isBlinky() ) {
        cerr << "Blinky is not a blinky." << endl;
    } else {
        cout << "Successfully opened communications with blinky!" << endl;
    }
    blinky.setLEDs(0);

    cout << setprecision(3);
    while(1) {
        usleep(500 * 1000);

        cpustat.update();
        cout << "\rTotal: " << setw(5) << cpustat.totalDiff().getUtilization() * 100 << "%";
        for(int i=0; i<cpustat.cpuCount(); ++i) {
            cout << " CPU " << i << ": ";
            cout << setw(5) << cpustat.cpuDiff(i).getUtilization() * 100 << "%";
        }

        /* If 2 cores, we use both green LEDs for load.  Otherwise, we use both
         * and set them to total system utilization */
        if(cpustat.cpuCount() == 2) {
            blinky.setLED(5, cpustat.cpuDiff(0).getUtilization());
            blinky.setLED(4, cpustat.cpuDiff(1).getUtilization());
        } else {
            blinky.setLED(5, cpustat.totalDiff().getUtilization());
            blinky.setLED(4, cpustat.totalDiff().getUtilization());
        }

        meminfo.update();
        cout << " Mem: " << meminfo.getUtilization() * 100 << "%";
        blinky.setLED(3, meminfo.getUtilization());

        cout << flush;
    }
    return 0;
}

void clearLine(std::istream& stream)
{
    char ch;
    do {
        stream.get(ch);
    } while( (!stream.eof()) && (ch != '\n') );
}


