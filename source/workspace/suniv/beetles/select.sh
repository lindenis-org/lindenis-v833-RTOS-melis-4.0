#=====================================================================================
#
#      Filename:  select.sh
#
#   Description:  seletct platform.
#
#       Version:  2.0 
#        Create:  2017-12-22 16:57:46
#      Revision:  none
#      Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
#
#        Author:  zhijinzeng@allwinnertech.com
#  Organization:  BU1-PSW
# Last Modified:  2018-10-11 13:23:56
#
#=====================================================================================
. $MELIS_BASE/.config

BOARD_NAME=""
PLATFORM_NAME=""

function do_select_platform()
{
	if [ $CONFIG_SOC_SUN3IW2  ]; then
		PLATFORM_NAME="sun3iw2"
	fi
	if [ $CONFIG_SOC_SUN3IW1  ]; then
		PLATFORM_NAME="sun3iw1"
	fi
	if [ $CONFIG_SOC_SUN8IW19  ]; then
		PLATFORM_NAME="sun8iw19"
	fi
	echo $PLATFORM_NAME
	echo $BOARD_NAME
}

