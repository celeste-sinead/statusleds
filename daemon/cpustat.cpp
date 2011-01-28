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

#include <string>
#include <iostream>
#include <fstream>

#include "cpustat.h"
#include "main.h"

using namespace std;

CPUUtilization::CPUUtilization() :
    m_user(0),
    m_nice(0),
    m_system(0),
    m_idle(0),
    m_iowait(0),
    m_irq(0),
    m_softirq(0)
{}

CPUUtilization::CPUUtilization(const CPUUtilization& other) :
    m_user(other.m_user),
    m_nice(other.m_nice),
    m_system(other.m_system),
    m_idle(other.m_idle),
    m_iowait(other.m_iowait),
    m_irq(other.m_irq),
    m_softirq(other.m_softirq)
{ }

CPUUtilization& CPUUtilization::operator=(const CPUUtilization& other) 
{
    m_user = other.m_user;
    m_nice = other.m_nice;
    m_system = other.m_system;
    m_idle = other.m_idle;
    m_iowait = other.m_iowait;
    m_irq = other.m_irq;
    m_softirq = other.m_softirq;
    return *this;
}

CPUUtilization CPUUtilization::operator-(const CPUUtilization& other)
{
    CPUUtilization out ( *this );

    out.m_user -= other.m_user;
    out.m_nice -= other.m_nice;
    out.m_system -= other.m_system;
    out.m_idle -= other.m_idle;
    out.m_iowait -= other.m_iowait;
    out.m_irq -= other.m_irq;
    out.m_softirq -= other.m_softirq;

    return out;
}

double CPUUtilization::getUtilization()
{
    double totalJiffies = getTotal();
    double usedJiffies = totalJiffies - m_idle;
    return usedJiffies / totalJiffies;
}

ostream& operator<<(ostream& stream, const CPUUtilization& cpu) 
{
    stream << cpu.m_user << ", " << cpu.m_nice << ", " << cpu.m_system << ", " << cpu.m_idle << ", ";
    stream << cpu.m_iowait << ", " << cpu.m_irq << ", " << cpu.m_softirq;
    return stream;
}

int CPUStat::update() 
{
    fstream procStat ("/proc/stat", ios_base::in);

    /* Read the total cpu utilization data */
    CPUUtilization newTotal;
    string cpuname;
    procStat >> cpuname;  // (ignored)
    procStat >> newTotal.m_user;
    procStat >> newTotal.m_nice;
    procStat >> newTotal.m_system;
    procStat >> newTotal.m_idle;
    procStat >> newTotal.m_iowait;
    procStat >> newTotal.m_irq;
    procStat >> newTotal.m_softirq;
    clearLine(procStat);
    if(procStat.fail()) {
        cerr << "Parse error reading total CPU utilization" << endl;
        return -1;
    }

    /* Update the total cpu utilization */
    m_allCPUDiff = newTotal - m_allCPUTotal;
    m_allCPUTotal = newTotal;

    /* Read and update individual CPU utilization data */
    CPUUtilization curCpu;
    for(size_t i = 0; 1; ++i) {
        /* Read the cpu data */
        procStat >> cpuname; // (ignored)
        if( cpuname.find("cpu") == string::npos ) {
            //First token in line doesn't contain 'cpu', we've read all cpu data
            break;
        }
        procStat >> curCpu.m_user; 
        procStat >> curCpu.m_nice;
        procStat >> curCpu.m_system;
        procStat >> curCpu.m_idle;
        procStat >> curCpu.m_iowait;
        procStat >> curCpu.m_irq;
        procStat >> curCpu.m_softirq;
        clearLine(procStat);
        if(procStat.fail()) {
            cerr << "Parse error reading CPU " << i << " utilization." << endl;
            break;
        }
       
        /* Update the current cpu's data */
        if( i >= m_cpuTotals.size() ) {
            /* Haven't seen this cpu before. (Probably first run) */
            m_cpuTotals.push_back(curCpu);
            m_cpuDiffs.push_back(curCpu);
        } else {
            m_cpuDiffs[i] = curCpu - m_cpuTotals[i];
            m_cpuTotals[i] = curCpu;
        }
    }

    return 0;
}

