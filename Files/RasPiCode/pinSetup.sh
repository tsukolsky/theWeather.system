#!/bin/sh
########################################################################
## pinSetup.sh
## Author: Todd Sukolsky
## Copyright of Todd Sukolsky
## Date Created: 4/25/13
## Last Reviced: 4/25/13
########################################################################
## Description: This should setup the needed pins on the Rasberry Pi
##	to communicate to theWeather.system board.
########################################################################
## Revisions:
##		4/25: Initial build
########################################################################
## Notes:
########################################################################

##Get GPIO25 as an output pin.
echo 25 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio25/direction
echo 0 > /sys/class/gpio/gpio25/value

echo "Finished setting pins" > /dev/kmsg
exit
