#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
int i;
//same datastructure as declared in kernel module
struct kernelData{
        int pid;
        unsigned long vaddress;
        long long time;

}kerndata;
//#define read_value _IOR(i,1,struct kernelData *)
int main()
{
        int fd,i=0;
	int size=0,nos=0;
	struct kernelData dataArr[1000];
	struct kernelData * buff;
	
        printf("\nOpening Driver\n");
        fd = open("/dev/character_device", O_RDONLY);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        } 
 
        printf("Reading Value from Driver\n");
	//reading the data from user	
	size=read(fd,dataArr,sizeof(dataArr));
	
	nos=size/sizeof(buff);
	printf("\n Creating %s.csv file","plot_data");
	FILE *fp;
	fp=fopen("plot_data.csv","w+");
	//printing the data and creating a csv file in userspace for ploting
	while(dataArr[i].pid!=0){
        printf("user Pid->%d \n",dataArr[i].pid);
        printf("user address->%lu \n",dataArr[i].vaddress);
        printf("user time->%lld \n",dataArr[i].time);
        //dataArr[i]=buff;
	fprintf(fp,"\n%lld",dataArr[i].time);
	fprintf(fp,",%lu",dataArr[i].vaddress);
        i++;
        }
	
	fclose(fp);
	printf("File Created!!\n");
	printf("Size of single buffer %ld\n",sizeof(buff));
	printf("Size of the dataArray recieved is%d\n",size);
 	printf("Number of process %d\n",nos);
        printf("Closing Driver\n");
        close(fd);
}
