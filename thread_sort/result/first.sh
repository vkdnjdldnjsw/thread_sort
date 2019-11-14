sudo dmesg -C

sudo insmod ../thread_sort.ko
sudo insmod ../thread_sort_depth.ko
sudo insmod ../sort_compare.ko

echo "insult done"

dmesg > dmesg.txt

echo "dmesg copy doen"
./parsing.o

echo "parsing done"

