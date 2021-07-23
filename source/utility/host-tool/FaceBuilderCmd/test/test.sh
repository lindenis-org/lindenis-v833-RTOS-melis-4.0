#!/bin/sh

./MakeScript ./init       1       1000
./MakeScript ./home       1001    2000
./MakeScript ./movie      2001    3000
./MakeScript ./explorer   3001    4000
./MakeScript ./music      4001    5000
./MakeScript ./setting    5001    6000
./MakeScript ./dialog     6001   7000
./MakeScript ./loading    7001   8000
./MakeScript ./alarm      8001   9000
./MakeScript ./linein      9001   10000
./MakeScript ./ebook      10001   11000
./MakeScript ./fm      11001   12000
./MakeScript ./photo      12001   13000
./MakeScript ./record      13001   14000
./MakeScript ./game      14001   15000
./MakeScript ./dtv  15001   16000

rm touchtheme.script
cat ./config/touchthemehdr.script   \
		./init/init.script              \
		./home/home.script              \
		./movie/movie.script            \
		./explorer/explorer.script            \
		./music/music.script            \
		./setting/setting.script            \
		./dialog/dialog.script            \
		./loading/loading.script            \
		./alarm/alarm.script            \
		./linein/linein.script            \
		./ebook/ebook.script            \
		./fm/fm.script            \
		./photo/photo.script            \
		./record/record.script            \
		./game/game.script            \
		./dtv/dtv.script            \
	>> touchtheme.script


./facebuildercmd ./config/config.ini
