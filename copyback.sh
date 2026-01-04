while [ "$1" != "" ]; do
    [ "$1" == "c" ] && COMPARE=true
    [ "$1" == "d" ] && DEBUG=echo
    shift 1
done

if [ "$COMPARE" = "true" ]; then
    diff -ru gcc/bliss/ gcc-15.1.0/gcc/bliss/
    diff -ru gcc/testsuite/ gcc-15.1.0/gcc/testsuite/
    exit
fi

diff -ru gcc/bliss/ gcc-15.1.0/gcc/bliss/|grep ^d|cut -f4 -d\ | while read line; do
$DEBUG cp -p $line gcc/bliss
done

diff -ru gcc/testsuite/ gcc-15.1.0/gcc/testsuite/|grep ^d|cut -f3 -d\ | while read line; do
$DEBUG cp -p gcc-15.1.0/$line $line
done

