if [ "X$1" = "X" ]
then
    echo "No Source"
    exit 1
fi

if [ "X$2" = "X" ]
then
    echo "No Destination"
    exit 1
fi

if [ ! -d $1 ]
then
    echo "$1 not a directory"
    exit 1
fi

if [ ! -d $2 ]
then
    echo "$2 not a directory"
    exit 1
fi

mkdir $2/lib
mkdir $2/ctree
cp -R $1/* $2/ctree
cd $2/ctree
CTMP=/tmp/$$
find . -name "libmtclient.*" -print  >$CTMP
for i in `cat $CTMP`
do 
    mv $i ../lib
done
