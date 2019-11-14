#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(){
	int fd_result, fd_dmesg, num, flag = 0, new_line = 0;
	char buf[4];
	
	fd_dmesg = open("dmesg.txt", O_RDONLY);
	
	fd_result = open("result.txt", O_CREAT| O_WRONLY | O_TRUNC, 0644);

    	write(fd_result, "l_sort\t\tt1_sort\t\tt2_sort\t\tt3_sort\t\tt4_sort\t\tt5_sort\t\tt6_sort\t\tt7_sort\t\tt8_sort\t\td1_sort\t\td2_sort\t\td3_sort\n", 106);
	while((num = read(fd_dmesg, buf, 1)) > 0){
		if(flag){
			if(buf[0] == '\n'){
				if(!new_line){
					write(fd_result, "\t", num);
				}
				flag = 0;
			}
			else if(buf[0] == '!'){
				write(fd_result, "\n", num);
				new_line = 1;
			}
			else{
				write(fd_result, buf, num);
				new_line = 0;
			}
		}
		if(buf[0] == ']'){
			read(fd_dmesg, buf, 1);
			flag = 1;
		}
	}
	close(fd_dmesg);
	close(fd_result);
	return 0;
}
