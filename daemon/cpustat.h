/******************************************************************************
 * cpustat.h
 * Copyright 2010 Iain Peet
 *
 * Obtains CPU utilization information from /proc/stat
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

#ifndef CPUSTAT_H_
#define CPUSTAT_H_

#include <vector>
#include <iostream>

/* The various classes of utilization provided in /proc/stat */
class CPUUtilization {
public:
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;

    CPUUtilization();
    CPUUtilization(const CPUUtilization& other);
    CPUUtilization& operator=(const CPUUtilization& other);
    CPUUtilization  operator-(const CPUUtilization& other);
    friend std::ostream& operator<<(const CPUUtilization& cpu, std::ostream & stream);

    long getTotal() 
        { return user+nice+system+idle+iowait+irq+softirq; }
    double getUtilization();
};

class CPUStat {
private:
    // The total utilization of all cpus since startup:
    CPUUtilization allCPUTotal;
    // The change in allCPUTotal between the last two updates.
    CPUUtilization allCPUDiff;

    // The total utilization of each CPU at last update:
    std::vector<CPUUtilization> cpuTotals;
    // The change in each CPU's utilization between the last two updates
    std::vector<CPUUtilization> cpuDiffs;

public:
    int update();

    int cpuCount()
        { return cpuTotals.size(); }

    CPUUtilization& total()
        { return allCPUTotal; }
    CPUUtilization& totalDiff()
        { return allCPUDiff; }
    CPUUtilization& cpu(int cpu)
        { return cpuTotals[cpu]; }
    CPUUtilization& cpuDiff(int cpu)
        { return cpuDiffs[cpu]; }
};

#endif //CPUSTAT_H_
