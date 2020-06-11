#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/uaccess.h>
#include<linux/sched.h>
#include<linux/slab.h>
#include<linux/sched/signal.h>

#define buffer_size 250
#define DEVICE_NAME "Prathamesh Code"



MODULE_AUTHOR("Prathamesh Lonkar");
MODULE_DESCRIPTION("Displaying Process List:");
MODULE_LICENSE("GPL");




/*Function Declaration*/
static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t , loff_t * );
static int dev_release(struct inode *, struct file *);

/*Global Declarations*/
struct task_struct *currtask;
static int Minor;
static int number_of_process=0;
static int count=0;

/*File Operations*/
static struct file_operations fops={
.open = dev_open,
.release =dev_release,
.read=dev_read
};

/*Character Device Structure with device name as process*/
static struct miscdevice char_device ={
	.minor = MISC_DYNAMIC_MINOR,
	.name = "process",
	.fops = &fops
};




/*Initialization*/
static int __init init_proc(void){
	Minor = misc_register(&char_device);

   if (Minor < 0) {
     printk ("Registering the character device failed with %d\n", Minor);
     return Minor;
   }

   printk("Assigned minor number %d .  \n", Minor);
  
   return 0;

}

/*Exit Function Declaration*/
static void __exit exit_proc(void){

   printk("Unregistering the device and Exiting !! Bye Cruel World");
   misc_deregister(&char_device);
   
}

/*Called when we open the charachter device using open(/dev/process)*/
static int dev_open(struct inode *i, struct file *filepointer)
{
struct task_struct *task;
printk(KERN_INFO "DEVICE OPENED");
/*Initializing the first task to current_task value*/
currtask=next_task(&init_task);
/*To calcuate the number of processes running at the time of opening the device*/
for_each_process(task){
number_of_process++;
}
return 0;
}

/*Called when read function invoked in userspace*/
static ssize_t dev_read(struct file *file, char *out, size_t size, loff_t * off)
{
	int nu_error;
	struct task_struct * task;
	char buff[buffer_size];
	
	/*setting the buffer initially to with value zero*/
	memset(buff,0,sizeof(char)*250);
	printk(KERN_INFO "Process_List: Reading the Process Information...\n");
	
	/*Will only enter until all the processes are transfered to userspace*/
	if(count<=number_of_process){
		for_each_process(task){
		
			/*As after every read function call we are starting from the first task i am incrementing the tast until it reaches the previously unused task*/
			while(currtask!=task){
				task=next_task(task);

			}
			
			memset(buff,0,sizeof(char)*250);

			/*Copying the details of a process as a string into buffer and sending it to userspace*/
			sprintf(buff, "PID=%d PPID=%d CPU=%d STATE=%ld",task->pid,task->parent->pid,task_cpu(task),task->state);
			nu_error = copy_to_user(out, buff, strlen(buff));

			/*If the there was an error in sending the data copy_to_user will send the number of bytes not transferred as return value
			if there is an error we are returning the bad address macro to the user.*/
				if(nu_error !=0)
				{	
					printk(KERN_ALERT "Error while copying data to user!");
					return -EFAULT;
				}
			currtask=next_task(task);
			count++;
			break;
			
		
		}
	}	
	return strlen(buff);
	

}

/*Called when device is closed*/
static int dev_release(struct inode *i, struct file *filepointer){
printk(KERN_INFO "Device Closed\n");

	return 0;

}

module_init(init_proc);
module_exit(exit_proc);
