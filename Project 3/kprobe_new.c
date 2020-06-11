#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/init.h>
//#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include<linux/sched.h>
#include<linux/sched/signal.h>
//#include <linux/ioctl.h>
//#include <linux/wrapper.h>
int a;

//Structure used to store the data to transfer to the user space
struct kernelData{
        int pid;
        unsigned long vaddress;
        long long time;

}kerndata;
#define DEVICE_NAME "character_device"
#define MAX_SYMBOL_LEN  64

static int pid=11806;
static char symbol[MAX_SYMBOL_LEN] = "handle_mm_fault";
//Accept the pid from command prompt
module_param(pid,int,S_IRUGO);
int count=0;
int i=0;
int j=0;

//kprobe structure for registration
static struct kprobe kp = {
        .symbol_name    = symbol,
};

struct kernelData dataArr[1000];


static int     open_device(struct inode *, struct file *);
static int     release_device(struct inode *, struct file *);
static ssize_t device_read(struct file *,char *, size_t , loff_t *);
//static long device_ioctl(struct file *,unsigned int ,unsigned long );


//assigning the file ops to particular functions
static struct file_operations fops =
{
   .open = open_device,
   .release = release_device,
   .read= device_read
};
//registering the charachter device
static struct miscdevice charachter_device={
	.minor=	MISC_DYNAMIC_MINOR,
	.name=DEVICE_NAME,
	.fops=&fops

};

static int open_device(struct inode *inode,struct file *file){
	printk("Device Opened!!");
	return 0;

}

static int release_device(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device successfully closed\n");
   return 0;
}


static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	ktime_t time= ktime_get();
	if (!current->mm)
                return 1;       
	
	if(pid==current->pid){
	s64 t = ktime_to_ns(time);

	printk("Time->%lld",t);
	printk("Process Pid ->%d",current->pid);
	printk("VMA->%lu",regs->si);
	count++;
	printk("Count->%d",count);
	dataArr[i].pid=current->pid;
	dataArr[i].vaddress=regs->si;
	dataArr[i].time=t;
	
	i++;
	}
	
	return 0;
}

//Function to transfer data to user space
static ssize_t device_read(struct file *file,char *out , size_t size, loff_t * off){
        int ret2;
        struct kernelData *data;
        //char buff1[10];
	printk("Number of structure in daaArr are-> %d",count);
        ret2=copy_to_user(out,dataArr,sizeof(data)*count);
        if(ret2!=0){
        printk("Error while copying data to user");
        }
        return sizeof(data)*count;
}



static int __init kprobe_init(void)
{
	int ret,ret_val;
	kp.pre_handler = handler_pre;
	

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	pr_info("Planted kprobe at %p\n", kp.addr);

	ret_val=misc_register(&charachter_device);
	if(ret_val<0){
	printk("Failed the registration of device");
	return 0;
	}
	return 0;
}

static void __exit kprobe_exit(void)
{
	
	unregister_kprobe(&kp);
	pr_info("kprobe at %p unregistered\n", kp.addr);
	misc_deregister(&charachter_device);
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");

