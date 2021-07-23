#!/usr/bin/env python

import os
import sys
import subprocess
import argparse
import re
import logging
import operator

#--------------------------------------------------------------
# Global defines
#--------------------------------------------------------------
_TOOLCHIAN_PREFIX_        = "arm-melis-eabi-"
_TOOLCHIAN_GCC_           = "arm-melis-eabi-gcc"
_TOOLCHAIN_NM_            = "arm-melis-eabi-nm"
_TOOLCHAIN_NM_OPT_        = "-nlCS"
_TOOLCHAIN_SIZE_          = "arm-melis-eabi-size"
_TOOLCHAIN_SIZE_OPT_      = "-Axt"
_TOOLCHAIN_OBJDUMP_       = "arm-melis-eabi-objdump"
_TOOLCHAIN_OBJDUMP_OPT_   = "-D"
_TOOLCHAIN_ADDR2LINE_     = "arm-melis-eabi-addr2line"
_TOOLCHAIN_ADDR2LINE_OPT_ = "-pfiCe"
_TOOLCHAIN_READELF_       = "arm-melis-eabi-readelf"
_TOOLCHAIN_READELF_OPT_   = "-h"

_CRASH_LOG_ = "crash_log"

MATCH_ADDR = re.compile(r'0x[0-9a-f]{1,8}', re.IGNORECASE)

_ARCH_TYPE_ARM_         = "ARM"

g_arch = [_ARCH_TYPE_ARM_]

g_arch_toolchain = {
    'arm'    : 'arm-melis-eabi-gcc',
}

g_mm_leak_bt_cnt = 10

#--------------------------------------------------------------
# Environment and dependency check
#--------------------------------------------------------------

def get_arch_from_elf(elf_file):
    if not elf_file:
        return ""

    arch_info = subprocess.check_output(
        [_TOOLCHAIN_READELF_, _TOOLCHAIN_READELF_OPT_, elf_file])

    for line in arch_info.splitlines():
        if 'Machine' in line:
            temp = line.split()
            for arch in g_arch:
                if arch in temp:
                    print "arch : " + arch
                    return arch
    return ""

#--------------------------------------------------------------
# class Core_Dump
#--------------------------------------------------------------

class Core_Dump(object):
    """docstring for Core_Dump"""
    def __init__(self, crash_log, elf_file, toolchain_path):
        super(Core_Dump, self).__init__()
        self.crash_log       = crash_log
        self.elf_file        = elf_file.name
        self.toolchain_path  = toolchain_path

        self.parse_addr_list = []

        self.sp         = ""
        self.parse_step = 0
        self.task_info  = []
        self.crash_type = "task"
        self.arch       = _ARCH_TYPE_ARM_
        self.exc_num    = 0;

        self.arm_exc_reg    = {}
        self.xtensa_exc_reg = {}

        # mm info parse
        self.caller_list    = []
        self.caller_dictory = []

        # mm leak parse
        self.mm_leak_list    = []
        self.mm_leak_dictory = []

        # print flag
        self.print_flag = 0

        self.check_env()


    def find_pc_addr(self, pc_addr):
        try:
            pc_trans = subprocess.check_output([_TOOLCHAIN_ADDR2LINE_, _TOOLCHAIN_ADDR2LINE_OPT_, self.elf_file, pc_addr])
        except Exception as err:
            logging.exception(err)
        else:
            if not "?? ??:0" in pc_trans:
                print pc_trans
            else:
                print "addr invalid"

    def get_pc_addr(self, pc_addr):
        try:
            pc_trans = subprocess.check_output([_TOOLCHAIN_ADDR2LINE_, _TOOLCHAIN_ADDR2LINE_OPT_, self.elf_file, pc_addr])
        except Exception as err:
            logging.exception(err)
        else:
            if not "?? ??:0" in pc_trans:
                return pc_trans
            else:
                return "invalid"


    def get_value_form_line(self, line, index):
        val_list = re.findall(MATCH_ADDR, line)
        if val_list:
            if index > len(val_list):
                return ""
            return val_list[index]
        else:
            return ""

    def prase_addr(self, line, index):
        addr = self.get_value_form_line(line, index)
        if addr:
            #print line
            self.parse_addr_list.append(addr)
            self.find_pc_addr(addr)

    def check_env(self):
        global _TOOLCHIAN_GCC_
        global _TOOLCHAIN_ADDR2LINE_

        if sys.version_info.major > 2:
            error = """
            This parser tools do not support Python Version 3 and above.
            Python {py_version} detected.
            """.format(py_version=sys.version_info)

            print error
            sys.exit(1)

        '''
        cmd = _TOOLCHIAN_GCC_;
        if os.name == 'nt':
            cmd = "where " + cmd
        else:
            cmd = "which " + cmd

        retcode = subprocess.call(cmd, shell=True)
        if retcode:
            if not self.toolchain_path:
                error = """
    Can not find toolchian "{magic}" path
    Please set PATH by:
        export PATH=$PATH: ../build/compiler/../bin/
    or:
    use "-p" point to absolute toolchain path, ex:

        python coredump.py {log} {elf} -p {path}
    """.format(magic=_TOOLCHIAN_GCC_, log=self.crash_log.name, elf=self.elf_file, path="../build/compiler/../bin/")

                print error
                sys.exit(1)
            else:
                if not str(self.toolchain_path).endswith('/'):
                    self.toolchain_path = self.toolchain_path + '/'
                _TOOLCHAIN_ADDR2LINE_ = self.toolchain_path + _TOOLCHAIN_ADDR2LINE_
        '''

    def open_print_line(self):
        self.print_flag = 1

    def close_print_line(self):
        self.print_flag = 0

    def get_print_status(self):
        return self.print_flag

    def show(self):

        log_lines = iter(self.crash_log.read().splitlines())

        for line in log_lines:

            if self.get_print_status() == 1:
                print line

            #begin to parse one line

            if "backtrace" in line:
                #print line
                if "interrupt" in line:
                    self.crash_type = "interrupt"
                elif "task" in line:
                    self.crash_type = "task"
                else:
                    self.prase_addr(line, 0)
            else:
                pass

#--------------------------------------------------------------
# Main
#--------------------------------------------------------------

def main():

    parser = argparse.ArgumentParser(description='Melis crash log core dump')

    # specify arguments
    parser.add_argument(metavar='CRASH LOG', type=argparse.FileType('rb', 0),
                        dest='crash_log', help='path to crash log file')

    parser.add_argument(metavar='ELF FILE', type=argparse.FileType('rb', 0),
                        dest='elf_file', help='elf file of application')

    parser.add_argument('-p','--path', help="absolute path of build/compiler/../bin", default='')

    args = parser.parse_args()

    # parser core_dump
    core_dump_ins = Core_Dump(args.crash_log, args.elf_file, args.path)

    core_dump_ins.show()

    #close all files
    if args.crash_log:
        args.crash_log.close()

    if args.elf_file:
        args.elf_file.close()


if __name__ == "__main__":
    main()
