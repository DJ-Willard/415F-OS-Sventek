# create a process to populate /proc files
sleep 600&
pid=`ps | grep sleep | awk '{print $1}'`
echo ===== contents of /proc/$pid/cmdline
cat /proc/$pid/cmdline
echo
echo ===== contents of /proc/$pid/stat
cat /proc/$pid/stat
echo ===== contents of /proc/$pid/status
cat /proc/$pid/status
echo ===== contents of /proc/$pid/io
cat /proc/$pid/io
echo ===== contents of /proc/$pid/maps
cat /proc/$pid/maps
echo ===== showing link for /proc/$pid/exe
ls -l /proc/$pid/exe
kill $pid
