
all:
	make -C signboot
	make -C script
	make -C update_boot0_sun3iw1
	make -C update_boot0_sun3iw2
	make -C update_boot1
	make -C update_uboot
	make -C update_mbr
	make -C update_fes1
	make -C minfs_tool
	make -C fileAddSum
	make -C fat_tool
	make -C fix_file
	make -C burnMBR
	make -C FaceBuilderCmd
	make -C langBuilder
	make -C MakeScript
	make -C mklfs

clean:
	make -C signboot clean
	make -C script clean
	make -C update_boot0_sun3iw1 clean
	make -C update_boot0_sun3iw2 clean
	make -C update_boot1 clean
	make -C update_uboot clean
	make -C update_mbr clean
	make -C update_fes1 clean
	make -C minfs_tool clean
	make -C fileAddSum clean
	make -C fat_tool clean
	make -C burnMBR clean
	make -C FaceBuilderCmd clean
	make -C langBuilder clean
	make -C MakeScript clean
	make -C mklfs clean

