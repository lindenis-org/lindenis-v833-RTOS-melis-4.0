/*
 * logging.h - Centralised logging. Part of the Linux-FS project.
 *
 * Copyright (c) 2005 Richard Russon
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-FS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __FS_LOGGING_H__
#define __FS_LOGGING_H__
#include <stdarg.h>
#include "fsys_libs.h"
#include "support.h"

BOOL fs_log_parse_option(const char *option);

/* Function prototype for the logging handlers */
typedef int (fs_log_handler)(const char *function, const char *file, int line,
                             __u32 level, void *data, const char *format, va_list args);

///* Logging handlers */
//fs_log_handler fs_log_handler_fprintf;

/* Enable/disable certain log levels */
__u32 fs_log_set_levels(__u32 levels);
__u32 fs_log_clear_levels(__u32 levels);
__u32 fs_log_get_levels(void);

/* Enable/disable certain log flags */
__u32 fs_log_set_flags(__u32 flags);
__u32 fs_log_clear_flags(__u32 flags);
__u32 fs_log_get_flags(void);

int fs_log_redirect(const char *function, const char *file, int line,
                    __u32 level, void *data, const char *format, ...);

/* Logging levels - Determine what gets logged */
#define FS_LOG_LEVEL_DEBUG      ((__u32)1 <<  0) /* x = 42 */
#define FS_LOG_LEVEL_TRACE01    ((__u32)1 <<  1) /* fs level trace */
#define FS_LOG_LEVEL_QUIET      ((__u32)1 <<  2) /* Quietable output */
#define FS_LOG_LEVEL_INFO       ((__u32)1 <<  3) /* Volume needs defragmenting */
#define FS_LOG_LEVEL_VERBOSE    ((__u32)1 <<  4) /* Forced to continue */
#define FS_LOG_LEVEL_PROGRESS   ((__u32)1 <<  5) /* 54% complete */
#define FS_LOG_LEVEL_WARNING    ((__u32)1 <<  6) /* You should backup before starting */
#define FS_LOG_LEVEL_ERROR      ((__u32)1 <<  7) /* Operation failed, no damage done */
#define FS_LOG_LEVEL_PERROR     ((__u32)1 <<  8) /* Message : standard error description */
#define FS_LOG_LEVEL_CRITICAL   ((__u32)1 <<  9) /* Operation failed,damage may have occurred */
#define FS_LOG_LEVEL_MDUMP      ((__u32)1 <<  10)/* dump data when fread/fwrite/dirread */
#define FS_LOG_LEVEL_TRACE0     ((__u32)1 <<  11) /* vfs api level trace */
#define FS_LOG_LEVEL_TRACE1     ((__u32)1 <<  12) /* part level trace */
#define FS_LOG_LEVEL_TRACE2     ((__u32)1 <<  13) /* dev level trace */
#define FS_LOG_LEVEL_TRACEF     ((__u32)1 <<  14) /* trace by function name */
#define FS_LOG_LEVEL_OBJNAME    ((__u32)1 <<  15) /* show object name like file name or dev name */

/* Logging style flags - Manage the style of the output */
#define FS_LOG_FLAG_PREFIX  ((__u32)1 << 0) /* Prefix messages with "ERROR: ", etc */
#define FS_LOG_FLAG_FILENAME    ((__u32)1 << 1) /* Show the file origin of the message */
#define FS_LOG_FLAG_LINE    ((__u32)1 << 2) /* Show the line number of the message */
#define FS_LOG_FLAG_FUNCTION    ((__u32)1 << 3) /* Show the function name containing the message */
#define FS_LOG_FLAG_ONLYNAME    ((__u32)1 << 4) /* Only display the filename, not the pathname */
#define FS_LOG_FLAG_COLOUR  ((__u32)1 << 5) /* Colour highlight some messages */

#define FS_LOG_LEVEL_TRACEX     ( FS_LOG_LEVEL_TRACE0   \
                                | FS_LOG_LEVEL_TRACE01  \
                                | FS_LOG_LEVEL_TRACE1   \
                                | FS_LOG_LEVEL_TRACE2)

/* Macros to simplify logging.  One for each level defined above.
 * Note, if DEBUG is not defined, then fs_log_debug/trace have no effect.
 */
#define fs_log_critical(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_CRITICAL,NULL,FORMAT,##ARGS)
#define fs_log_error(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_ERROR,NULL,FORMAT,##ARGS)
#define fs_log_info(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_INFO,NULL,FORMAT,##ARGS)
#define fs_log_perror(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_PERROR,NULL,FORMAT,##ARGS)
#define fs_log_progress(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_PROGRESS,NULL,FORMAT,##ARGS)
#define fs_log_quiet(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_QUIET,NULL,FORMAT,##ARGS)
#define fs_log_verbose(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_VERBOSE,NULL,FORMAT,##ARGS)
#define fs_log_warning(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_WARNING,NULL,FORMAT,##ARGS)
#define fs_log_debug(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_DEBUG,NULL,FORMAT,##ARGS)
#define fs_log_trace0(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE0,NULL,FORMAT,##ARGS)
#define fs_log_trace01(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE01,NULL,FORMAT,##ARGS)
#define fs_log_trace1(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE1,NULL,FORMAT,##ARGS)
#define fs_log_trace2(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_TRACE2,NULL,FORMAT,##ARGS)
#define fs_log_objname(FORMAT, ARGS...) fs_log_redirect(__FUNCTION__,__FILE__,__LINE__,FS_LOG_LEVEL_OBJNAME,NULL,FORMAT,##ARGS)

#endif /* __FS_LOGGING_H__ */

