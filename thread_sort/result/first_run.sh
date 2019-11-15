sudo dmesg -C

echo "insult start"

sudo insmod ../thread_sort.ko
sudo insmod ../thread_sort_depth.ko
sudo insmod ../sort_compare.ko

echo "insult done"

dmesg > dmesg.txt

echo "dmesg copy doen"
gcc parsing.c -o parsing.o
./parsing.o

echo "parsing done"

