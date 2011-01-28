/******************************************************************************
 * cpustat.cpp
 * Copyright 2010 Iain Peet
 *
 * Obtains CPU usage information from /proc/stat
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

#include "cpustat.h"

using namespace std;

CPUUtilization::CPUUtilization() :
    user(0),
    nice(0),
    system(0),
    idle(0),
    iowait(0),
    irq(0),
    softirq(0)
{}

CPUUtilization::CPUUtilization(const CPUUtilization& other) :
    user(other.user),
    nice(other.nice),
    system(other.system),
    idle(other.idle),
    iowait(other.iowait),
    irq(other.irq),
    softirq(other.softirq)
{ }

CPUUtilization& CPUUtilization::operator=(const CPUUtilization& other) 
{
    user = other.user;
    nice = other.nice;
    system = other.system;
    idle = other.idle;
    iowait = other.iowait;
    irq = other.irq;
    softirq = other.softirq;
    return *this;
}

CPUUtilization CPUUtilization::operator-(const CPUUtilization& other)
{
    CPUUtilization out ( *this );

    out.user -= other.user;
    out.nice -= other.nice;
    out.system -= other.system;
    out.idle -= other.idle;
    out.iowait -= other.iowait;
    out.irq -= other.irq;
    out.softirq -= other.softirq;

    return out;
}

double CPUUtilization::getUtilization()
{
    double totalJiffies = getTotal();
    double usedJiffies = totalJiffies - idle;
    return usedJiffies / totalJiffies;
}

ostream& operator<<(ostream& stream, const CPUUtilization& cpu) 
{
    stream << cpu.user << ", " << cpu.nice << ", " << cpu.system << ", " << cpu.idle << ", ";
    stream << cpu.iowait << ", " << cpu.irq << ", " << cpu.softirq;
    return stream;
}

int CPUStat::update() 
{
    FILE* procstat = fopen("/proc/stat","r");
    if(!procstat) {
        perror("fopen");
        return -1;
    }

    /* Read the total cpu utilization data */
    CPUUtilization newTotal;
    int fieldsRead = fscanf(procstat, "cpu %ld %ld %ld %ld %ld %ld %ld %*d %*d", 
            &newTotal.user,
            &newTotal.nice,
            &newTotal.system,
            &newTotal.idle,
            &newTotal.iowait,
            &newTotal.irq,
            &newTotal.softirq);
    if( fieldsRead != 7 ) {
        cerr << "Reading cpu total: expected 7 fields but found " << fieldsRead << endl;
        return -1;
    }

    /* Update the total cpu utilization */
    allCPUDiff = newTotal - allCPUTotal;
    allCPUTotal = newTotal;

    /* Read and update individual CPU utilization data */
    CPUUtilization curCpu;
    for(size_t i = 0; 1; ++i) {
        /* Read the cpu data */
        fieldsRead = fscanf(procstat, " cpu%*d %ld %ld %ld %ld %ld %ld %ld %*d %*d",
            &curCpu.user, 
            &curCpu.nice,
            &curCpu.system,
            &curCpu.idle,
            &curCpu.iowait,
            &curCpu.irq,
            &curCpu.softirq);
        if(fieldsRead != 7) {
            // scanf failed means we're beyond the cpu data
            break;
        }
       
        /* Update the current cpu's data */
        if( i >= cpuTotals.size() ) {
            /* Haven't seen this cpu before. (Probably first run) */
            cpuTotals.push_back(curCpu);
            cpuDiffs.push_back(curCpu);
        } else {
            cpuDiffs[i] = curCpu - cpuTotals[i];
            cpuTotals[i] = curCpu;
        }
    }

    return 0;
}

