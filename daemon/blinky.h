/******************************************************************************
 * blinky.
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

#ifndef BLINKY_H_
#define BLINKY_H_

#define LED_COUNT 6

class Blinky {
private:
    const char* m_blinkyDev;
    // The POSIX file descriptor for the blinky's serial line
    int m_blinkyfd;

    /* Functions managing the blinky's file descriptor.
     * open() and close() do exactly what it says on the box. */
    void openBlinky();
    void closeBlinky();

public:
    Blinky(const char* blinkyDev);
    ~Blinky();

    // Set the 8bit PWM duty cycle of one of the 6 pwm LEDs
    void setLED(int led, unsigned int pwm);
    // Set the 8bit PWM duty cycle of all of the LEDs
    void setLEDs(unsigned int pwm);
    /* Set the intensity of an LED.  PWM values are calculated so that
     * intensity will vary approximately linearly.
     * @param intensity the intensity, in [0,1] */
    void setLED(int led, double intensity);
    // Determine whether the blinky times out if it goes 4sec without instructions.
    void setTimeout(bool enable=true);
    // Turn the red 'debug' LED on or off
    void setRed(bool on=true);
    // Turn the yellow 'debug' LED on or off
    void setYellow(bool on=true);

    /* Check if the file is open, and if not, attempt to open it.  It will 
     * return false if the file is not open and could not be opened, and true 
     * if it is ready for writing */
    bool ready();
    // Check if the connected device is actually the blinky
    bool isBlinky();
};

#endif // BLINKY_H_

