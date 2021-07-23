cc = gcc -g
ccflag-s = -I. -I./part -I./config -I./config/suni -I./dbg -I./include -I./fs -I./fs/b_fatfs -I./if -g -fshort-wchar

SRC = -c ./fsys_libs.c ./fsys_tool.c ./sys_fsys.c ./part/part.c ./part/partman.c   ./dbg/fs_cachedump.c ./dbg/fs_debug.c ./dbg/fs_logging.c ./dbg/fs_timer.c  ./fs/buffer.c ./fs/ctype.c ./fs/dcache.c ./fs/dir.c ./fs/file.c ./fs/fsman.c ./fs/fstime.c ./fs/fstools.c ./fs/inode.c ./fs/namei.c ./fs/nls_base.c ./fs/nls_cp936.c ./fs/page_pool.c ./fs/slab.c ./fs/super.c ./fs/b_fatfs/fat_cache.c ./fs/b_fatfs/fat_dir.c ./fs/b_fatfs/fat_entry.c ./fs/b_fatfs/fat_file.c ./fs/b_fatfs/fat_format.c ./fs/b_fatfs/fatfs.c ./fs/b_fatfs/fat_inode.c ./fs/b_fatfs/fat_misc.c ./fs/b_fatfs/fat_namei.c


AR = ar

all:clean target

target:
	@$(cc) $(ccflag-s) $(SRC)
	@ar rcs libfstool.a *.o

clean:
	@-rm *.o *.a

