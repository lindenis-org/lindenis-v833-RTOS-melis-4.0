DATETIME=`date -R`
commitid=`git rev-parse HEAD`
echo $DATETIME > .ver1
echo \#define SDK_UTS_VERSION ' '\"`cat .ver1 `\" > .ver
echo \#define SDK_COMPILE_TIME   \"`date +%T`\" >> .ver
echo \#define SDK_COMPILE_BY '  '\"`whoami`\" >> .ver
echo \#define SDK_GIT_VERSION ' '\"$commitid\" >> .ver	
cat   .ver
rm -f .ver
