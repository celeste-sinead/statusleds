/******************************************************************************
 * blinky.cpp
 * Copyright 2011 Iain Peet
 *
 * Controls the blinky status LED peripheral.
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

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "blinky.h"

Blinky::Blinky(const char* blinkyDev) : m_blinkyDev(blinkyDev), m_blinkyfd(-1)
{
    openBlinky();
}

Blinky::~Blinky()
{
    closeBlinky();
}

void Blinky::openBlinky()
{
    /* Open the serial dev */
    m_blinkyfd = open(m_blinkyDev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(m_blinkyfd < 0) {
        perror("open");
    }
    if(!isatty(m_blinkyfd)) {
        fprintf(stderr, "%s is not a TTY!\n", m_blinkyDev);
        closeBlinky();
    }

    /* Set up serial config goop */
    struct termios blinky_term;
    if( tcgetattr(m_blinkyfd, &blinky_term) ) {
        perror("tcgetattr");
        closeBlinky();
    }
    blinky_term.c_iflag = 0;
    blinky_term.c_oflag = 0;
    blinky_term.c_cflag = CREAD | CLOCAL | CS8;
    blinky_term.c_lflag = 0;
    cfsetispeed(&blinky_term, B115200);
    cfsetospeed(&blinky_term, B115200);
    if( tcsetattr(m_blinkyfd, TCSANOW, &blinky_term) ) {
        perror("tcsetatr");
        closeBlinky();
    } 
    
    /* The TTY driver very helpfully sends DTR when you open the blinky device.
     * Unfortunately, Arduino uses DTR to signal a processor reset.  So, after
     * opening, wait a bit (empirically, ~1.5s) so the Arduino can start up and 
     * be properly ready to receive commands */
    sleep(2);
}

void Blinky::closeBlinky()
{
    close(m_blinkyfd);
    m_blinkyfd = -1;
}

void Blinky::setLED(int led, unsigned int pwm)
{
    if( !ready() ) return;
    if( (led<0) || (led>5) ) return;
    if(pwm > 255) pwm = 255;

    char buf[32];
    int len = sprintf(buf, "led%d %d\n", led, pwm);
    if( write(m_blinkyfd, buf, len) < 0 ) {
        perror("write");
        closeBlinky();
    }
}

void Blinky::setLEDs(unsigned int pwm)
{
    for(int i=0; i<LED_COUNT; ++i) {
        setLED(i, pwm);
    }
}

void Blinky::setLED(int led, double intensity)
{
    /* Empirically, the subjective intensity is logarithmic with
     * the PWM duty cycle.  So, we use the intensity as an exponent,
     * scaled so that the output is in [0,255] if the input is in [0,1] */

    // pow(2, intensity*8) has range [1,256] if intensity is in [0,1]
    setLED(led, (unsigned int)(pow(2, intensity*8) - 1) );
}

void Blinky::setTimeout(bool enable)
{
    if( !ready() ) return;
    const char* cmd = enable ? "timeout on\n" : "timeout off\n";
    int len = strlen(cmd);
    if( write(m_blinkyfd, cmd, len) < 0 ) {
        perror("write");
        closeBlinky();
    }
}

void Blinky::setRed(bool on) 
{
    if( !ready() ) return;
    const char* cmd = on ? "red on\n" : "red off\n";
    int len = strlen(cmd);
    if( write(m_blinkyfd, cmd, len) < 0 ) {
        perror("write");
        closeBlinky();
    }
}

void Blinky::setYellow(bool on)
{
    if( ! ready() ) return;
    const char* cmd = on ? "yellow on\n" : "yellow off\n";
    int len = strlen(cmd);
    if( write(m_blinkyfd, cmd, len) < 0 ) {
        perror("write");
        closeBlinky();
    }
}

bool Blinky::ready()
{
    // Is the blinky serial device open?
    if( m_blinkyfd >= 0 ) return true;
    // Not currently open.  Try to open it.
    openBlinky();
    // Success ?
    return m_blinkyfd >= 0;
}

bool Blinky::isBlinky()
{
    /* If the blinky sees a ? as the first character on
     * a line, it will immediately respond with "rgblinky\n"
     * which we use to check whether the connected serial
     * device is actually the blinky */

    int status;
    char ch;

    /* First, clear any data which might be sitting in the buffer 
     * NB: the file should be in non-blocking mode */
    while( 0 < (status = read(m_blinkyfd, &ch, 1)) );
    if( (status < 0) && (errno != EAGAIN) ) {
        perror("read");
        closeBlinky();
        return false;
    }

    /* Now, write the query */
    if( write(m_blinkyfd, "\n?", 2) != 2 ) {
        perror("write");
        closeBlinky();
        return false;
    }

    /* See if we get the reply */
    // File is non-blocking, need to give blinky sufficient time to respond:
    usleep(250 * 1000);
    char buf[10];
    status = read(m_blinkyfd, buf, 9);
    if ( (status < 0) && (errno != EAGAIN) ) {
        perror("read");
        closeBlinky();
        return false;
    }
    if( status != 9 ) {
        // too few characters!
        return false;
    }
    buf[status] = '\0';
   
    return strcmp(buf, "rgblinky\n") == 0;
}

