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

/* Container for the various types of CPU jiffies accounted in
 * /proc/stat.  This class may store jiffies for a particular CPU,
 * or for all CPUs.  It may store total jiffies since startup,
 * or the deltas between successive reads of /proc/stat. */
class CPUUtilization {
public:
    long m_user;
    long m_nice;
    long m_system;
    long m_idle;
    long m_iowait;
    long m_irq;
    long m_softirq;

    CPUUtilization();
    CPUUtilization(const CPUUtilization& other);
    CPUUtilization& operator=(const CPUUtilization& other);
    CPUUtilization  operator-(const CPUUtilization& other);
    friend std::ostream& operator<<(const CPUUtilization& cpu, std::ostream & stream);

    long getTotal() 
        { return m_user+m_nice+m_system+m_idle+m_iowait+m_irq+m_softirq; }
    // Total non-m_idle time, as a fraction in the range [0,1] :
    double getUtilization();
};

/* Class for obtaining CPU utilization information from /proc/stat.
 * /proc/stat is read by calling the update() function.  */
class CPUStat {
private:
    // The total utilization of all cpus since startup:
    CPUUtilization m_allCPUTotal;
    // The change in m_allCPUTotal between the last two updates.
    CPUUtilization m_allCPUDiff;

    // The total utilization of each CPU at last update:
    std::vector<CPUUtilization> m_cpuTotals;
    // The change in each CPU's utilization between the last two updates
    std::vector<CPUUtilization> m_cpuDiffs;

public:
    /* Obtain new utilization info from /proc/stat
     * Utilization diffs will be calculated from the data read on the
     * last call to update().  (The first call to update() will give
     * diffs relative to 0).
     * @return  0 on success, -1 on failure */
    int update();

    int cpuCount()
        { return m_cpuTotals.size(); }

    // Total utilization of all CPUs since startup
    CPUUtilization& total()
        { return m_allCPUTotal; }
    // Change in total utilization between last two updates
    CPUUtilization& totalDiff()
        { return m_allCPUDiff; }
    // Utilization for a particular CPU since startup
    CPUUtilization& cpu(int cpu)
        { return m_cpuTotals[cpu]; }
    // Change in utilization for a particualr CPU between last two updates
    CPUUtilization& cpuDiff(int cpu)
        { return m_cpuDiffs[cpu]; }
};

#endif //CPUSTAT_H_

