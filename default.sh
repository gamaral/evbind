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
		531) # KEY_TOUCHPAD_ON
			synclient TouchpadOff=2
			;;
		532) # KEY_TOUCHPAD_OFF
			synclient TouchpadOff=1
			;;
		539) # KEY_ATTENDANT_ON
			rfkill unblock all
			;;
		540) # KEY_ATTENDANT_OFF
			rfkill block all
			;;
		esac
		;;
esac

