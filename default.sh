#!/bin/sh

# Example Event Handler

set $*

event=${1}
code=${2}
value=${3}

export DISPLAY=":0"
export XAUTHORITY="/home/<user>/.Xauthority"

case "$event" in
	KEY)
		case "$code" in
		530) # KEY_TOUCHPAD_TOGGLE
			synclient -l | grep -q -e 'TouchpadOff \+= 1'
			synclient TouchpadOff=$((2 - $?))
			;;
		esac
		;;
esac

