sudo rmmod sort_compare
sudo rmmod thread_sort
sudo rmmod thread_sort_depth 

echo "remove done"

sudo dmesg -C

echo "insult start"

sudo insmod ../thread_sort.ko
sudo insmod ../thread_sort_depth.ko
sudo insmod ../sort_compare.ko

echo "insult done"

dmesg > dmesg.txt

echo "dmesg copy done"
./parsing.o

echo "parsing done"

