function hmm() {
cat <<EOF
Invoke ". source/melis-env.sh" from your shell to add the following functions to your environment:

== before all ==
- lunch:        lunch <product_name>-<build_variant>

== build project ==
- Cleaning targets:
-   clean	  - Remove most generated files but keep the config and
-                     enough build support to build external modules
-   mrproper	  - Remove all generated files + config + various backup files
-   distclean	  - mrproper + remove editor backup and patch files
-
- Configuration targets:
-   make menuconfig to to do the customilize configuration.
-
- Other generic targets:
-   all	  - Build all targets marked with [*]
- * melis	  - Build the bare kernel
- * modules	  - Build all modules
-   gtags        - Generate GNU GLOBAL index
-
- Static analysers:
-   checkstack      - Generate a list of stack hogs
-   namespacecheck  - Name space analysis on compiled kernel
-   versioncheck    - Sanity check on version.h usage
-   includecheck    - Check for duplicate included header files
-   export_report   - List the usages of all exported symbols
-   headers_check   - Sanity check on exported headers
-   headerdep       - Detect inclusion cycles in headers
-   coccicheck      - Check with Coccinelle
-
- Kernel selftest:
-                     running kselftest on it
-   kselftest-clean - Remove all generated kselftest files
-   kselftest-merge - Merge all the config dependencies of kselftest to existing

- jump directory:
- croot:    Jump to the top of the tree.
- cboot:    Jump to uboot.
- cboot0:   Jump to boot0.
- cdts:     Jump to device tree.
- cbin:     Jump to uboot/boot0 bin directory.
- ckernel:  Jump to kernel.
- cdevice:  Jump to target.
- ccommon:  Jump to platform common.
- cconfigs: Jump to configs of target.
- cout:     Jump to out directory of target.
- ctarget:  Jump to target of compile directory.
- crootfs:  Jump to rootfs of compile directory.
- ctoolchain: Jump to toolchain directory.
- callwinnerpk: Jump to package allwinner directory.
- ctinatest:  Jump to tinateset directory.
- godir:    Go to the directory containing a file.

== grep file ==
- cgrep:    Greps on all local C/C++ files.

Look at the source to view more functions. The complete list is:
EOF
    T=$(gettop)
    local A
    A=""
    for i in `cat $T/envsetup.sh | sed -n "/^[ \t]*function /s/function \([a-z_]*\).*/\1/p" | sort | uniq`; do
      A="$A $i"
    done
    echo $A
}

function gettop()
{
    local source_path=${MELIS_BASE}
    if [ "$source_path"  ]; then
	(\cd $source_path; PWD= /bin/pwd)
    else
        echo "Couldn't locate the melis-sdk top dir."
    fi
    unset source_path
}

function ctoolchain()
{
    local T=$(gettop)
    [ -z "$T" ] \
        && echo "Couldn't locate the top of the tree.  Try setting TOP." \
        && return

    \cd $T/../toolchain/bin
}

function make()
{
    local start_time=$(date +"%s")
    command make V=s "$@"
    local ret=$?
    local end_time=$(date +"%s")
    local tdiff=$(($end_time-$start_time))
    local hours=$(($tdiff / 3600 ))
    local mins=$((($tdiff % 3600) / 60))
    local secs=$(($tdiff % 60))
    echo
    if [ $ret -eq 0 ] ; then
        echo -n -e "#### make completed successfully "
    else
        echo -n -e "#### make failed to build some targets "
    fi
    if [ $hours -gt 0 ] ; then
        printf "(%02g:%02g:%02g (hh:mm:ss))" $hours $mins $secs
    elif [ $mins -gt 0 ] ; then
        printf "(%02g:%02g (mm:ss))" $mins $secs
    elif [ $secs -gt 0 ] ; then
        printf "(%s seconds)" $secs
    fi
    echo -e " ####"
    echo
    return $ret
}

function get_char()
{
	SAVEDSTTY=`stty -g`
	stty -echo
	stty cbreak
	dd if=/dev/tty bs=1 count=1 2> /dev/null
	stty -raw
	stty echo
	stty $SAVEDSTTY
}

function pause()
{
	if [ "x$1" != "x" ] ;then
		echo $1
	fi
	echo "Press any key to continue!"
	char=`get_char`
}


function godir ()
{
    if [[ -z "$1" ]]; then
        echo "Usage: godir <regex>"
        return
    fi
    local T=$(gettop)
    if [[ ! -f $T/filelist ]]; then
        echo -n "Creating index..."
        (\cd $T; find . -type f > filelist)
        echo " Done"
        echo ""
    fi
    local lines
    lines=($(\grep "$1" $T/filelist | sed -e 's/\/[^/]*$//' | sort | uniq))
    if [[ ${#lines[@]} = 0 ]]; then
        echo "Not found"
        return
    fi
    local pathname
    local choice
    if [[ ${#lines[@]} > 1 ]]; then
        while [[ -z "$pathname" ]]; do
            local index=1
            local line
            for line in ${lines[@]}; do
                printf "%6s %s\n" "[$index]" $line
                index=$(($index + 1))
            done
            echo
            echo -n "Select one: "
            unset choice
            read choice
            if [[ $choice -gt ${#lines[@]} || $choice -lt 1 ]]; then
                echo "Invalid choice"
                continue
        fi
            pathname=${lines[$(($choice-1))]}
        done
    else
        pathname=${lines[0]}
    fi
    \cd $T/$pathname
}

pack_usage()
{
	printf "Usage: pack [-cCHIP] [-pPLATFORM] [-bBOARD] [-d] [-s] [-m] [-w] [-i] [-h]
	-c CHIP (default: $chip)
	-p PLATFORM (default: $platform)
	-b BOARD (default: $board)
	-d pack firmware with debug info output to card0
	-s pack firmware with signature
	-m pack dump firmware
	-w pack programmer firmware
	-i pack sys_partition.fex downloadfile img.tar.gz
	-h print this help message
"
}

function pack() {
	local T=$(gettop)
	local chip=$(get_platform)
	local platform=melis-v3
	local board_platform=none
	local board=none
	local debug=uart0
	local sigmode=none
	local securemode=none
	local mode=normal
	local programmer=none
	local tar_image=none
	unset OPTIND
	while getopts "dsvmwih" arg
	do
		case $arg in
			d)
				debug=card0
				;;
			s)
				sigmode=secure
				;;
			v)
				securemode=secure
				;;
			m)
				mode=dump
				;;
			w)
				programmer=programmer
				;;
			i)
				tar_image=tar_image
				;;
			h)
				pack_usage
				return 0
				;;
			?)
			return 1
			;;
		esac
	done

	if [ "x$chip" = "x" ]; then
		echo "platform($TARGET_PLATFORM) not support"
		#return
	fi

	pushd $T/workspace/suniv/beetles >> /dev/null
	if [ -f ./pack_img.sh ]; then
		./pack_img.sh -c $chip -p $platform -b $board -d $debug -s $sigmode -m $mode -w $programmer -v $securemode -i $tar_image -t $T
		#pause
	else
		echo "pack_img script not exist!"
	fi
	popd >> /dev/null
}

function cconfigs()
{
    select_config_path=${MELIS_BASE}/projects/defconfig/${board_name}
    if [ "$select_config_path"  ]; then
        \cd $select_config_path
    else
        echo "Couldn't locate the ${select_config_path}."
    fi
    unset select_config_path
}


function print_lunch_menu(){
    board_config_path=${MELIS_BASE}/projects/defconfig
    local j=0
    for i in `ls -1 $board_config_path`
    do
        if [[ -d ${MELIS_BASE}/projects/defconfig/$i ]]; then
            board_name=${i#*config_}
            config_list[j]=$board_name
            j=`expr $j + 1`
        fi
    done

    local i=1
    for config_list_id in ${config_list[@]}
    do
        echo "    $i. $config_list_id"
        i=$(($i+1))
    done
}

function copy_config(){
    board_name=$1
    echo "You have select $board_name "
    export TARGET_BOARD=$board_name

    dotconfig=${MELIS_BASE}/projects/defconfig/${board_name}/config_${board_name}
    parttable=${MELIS_BASE}/projects/defconfig/${board_name}/aw_${board_name}/sys_partition.fex
    sysconfig=${MELIS_BASE}/projects/defconfig/${board_name}/aw_${board_name}/sys_config.fex

    if [ -f ${dotconfig} ]; then
        cp -rf ${dotconfig} ${MELIS_BASE}/.config
    else
        echo "fatal error, no default .config file."
        return -1
    fi
    cp -fr ${sysconfig} ${MELIS_BASE}/ekernel/arch/boot/fex/
}

function get_platform() {
    unset CONFIG_SOC_SUN3IW2P2
    unset CONFIG_SOC_SUN3IW2P1
    unset CONFIG_SOC_SUN3IW1P1
    unset CONFIG_SOC_SUN8IW18P1
    unset CONFIG_SOC_SUN8IW19P1

    . .config

    YES="y"

    if [ "$CONFIG_SOC_SUN3IW2P2" = "$YES" ]; then
	echo "sun3iw2p2"
    elif [ "$CONFIG_SOC_SUN3IW2P1" = "$YES" ]; then
	echo "sun3iw2p1"
    elif [ "$CONFIG_SOC_SUN3IW1P1" = "$YES" ]; then
	echo "sun3iw1p1"
    elif [ "$CONFIG_SOC_SUN8IW18P1" = "$YES" ]; then
	echo "sun8iw18p1"
    elif [ "$CONFIG_SOC_SUN8IW19P1" = "$YES" ]; then
	echo "sun8iw19p1"
    fi
}

function platform_verbose(){
    local platformid=$(get_platform)

    echo "============================================"
    echo "Project Based On Platform" $platformid
    echo "============================================"
}
function lunch(){
    local uname=$(uname -a)
    echo
    echo "You're building on" $uname
    echo
    echo "Lunch menu... pick a combo:"
    echo "The supported board:"
    unset config_list

    print_lunch_menu
    echo -n "What is your choice? "
    read board_choice

    for config_list_id in ${config_list[@]}
    do
        if [ "$config_list_id" == "$board_choice" ] ; then
            copy_config $config_list_id
            platform_verbose
            return 0
        fi
    done

    if  [ "$board_choice" -gt ${#config_list[@]} ]; then
	echo "Too much number, exceed the maxium support board!"
	return -1
    fi

    if [ "$board_choice" -gt 0  ] ;then
        echo "The $board_choice is number." > /dev/null
    else
        echo "The soc family [$board_choice] not supported!"
        return -1
    fi

    config_list_id=""
    for config_list_id in ${config_list[@]}
    do
        if [ "${config_list[$board_choice-1]}" == "$config_list_id" ] ; then
            copy_config $config_list_id
        fi
    done

    platform_verbose
}
