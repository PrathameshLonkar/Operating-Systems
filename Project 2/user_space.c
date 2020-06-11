#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>


#define buffer_length 250

int main(){
int i=1;
int No_Of_Process=0,status=1;
static char buffer[buffer_length];
int fd = open("/dev/process",O_RDONLY);
if(fd<0){
printf("Failed to open the process");
}
printf("Printing from user_space \n");
while(status !=0){
        status=read(fd,buffer,buffer_length);
	
	if(strlen(buffer)==0){
	printf("No more process to display");
	}else{
	printf("%s\n",buffer);
	memset(buffer,0,sizeof(char)*250);
	}

}
if(status<0){
perror("Error while reading the file");
return 0;
}


close(fd);
return 0;
}
