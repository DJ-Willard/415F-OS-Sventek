#include <BoundedBuffer.h>
#include <diskdriver.h>
#include <diskdevice__full.h>
#include <freesectordescriptorstore__full.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlid.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))


/* any global variables required for use by your threads and your driver routines */
#define NVOUCHERS 30
#define BBSIZE 10
#define DD_ID
//number of vouchers
int freeVouchers = 0;
//array of voucher based off nember of free sectors 30
Voucher vouchers[NVOUCHERS]; 
//pointer to all the availbe free vouchers
Voucher *freeVouchersList = NULL;
// protect tied to wait conditional
pthread_mutex_t w_lock = PTHREAD_MUTEX_INITIALIZER;
// wait till voucher is vouch is complete
pthread_cond_t wait_flag = PTHREAD_COND_INITIALIZER;
bool complete = false;
 
/* you must define struct voucher and allocate a sufficient number of them in your 
 * declared data; the structure must enable application threads to block until the 
 * associated read/write has completed 
 * status: 0 not assigned, 1 assigned, 2 if in w/r queue, 3 succesful write, -1 failure to write
 * request_t: 0 = write 1 = read -1= default value
*/ 
struct voucher{
	sectordescriptor *sd; // pointer to the sector discriptor handed to us.
	int status;
	int request_t;  
	pthread_mutex_t v_lock; // protect data
	pthread_cond_t status_flag; // protect critcial region.
	struct voucher *next; // pointer to next ticket in buff
}Voucher;

//This function take in and modifies each voucher to a initialized set of values.  resourced form lab5
void initFreeVoucher(void)
{
	int i;

	for(i=0; i<NVOUCHERS; i++)
	{
			vouchers[i].sd = NULL;
			vouchers[i].status = 0;
			vouchers[i].request_t = -1;
			vouchers[i].v_lock = PTHREAD_MUTEX_INITIALIZER;
			vouchers[i].status_flag = PTHREAD_COND_INITIALIZER;
			vouchers[i].next = freeVouchersList;
			freeVouchersList = vouchers +1
			freeVouchers++;
	}
}
/* definition[s] of function[s] required for your thread[s] */ 
//helper fuction assign vouvher form free voucher list 
//resourced form lab5  blocking, each process needs a voucher to continue
void voucherAssignment(void)
{
	voucher *stub;
	pthread_mutex_lock(&stub.v_lock)
	stub = freeVouchers;
	while(freeVouchers == 0)
	{
		pthread_cond_wait(&stub.status_flag, &lock);
	}
	stub = freeVouchers;
	freeVouchersList = stub->next;
	freeVouchers--;
	pthread_cond_broacast(&stub.status_flag);
	pthread_mutex_unlock(&stub.v_lock);
	return stub
}
//helper fuction recycle used vouchers to freevoucher list  resourced form lab5 blocking
void voucherRecyle(Voucher *v)
{
	pthread_mutex_lock(&v.v_lock);
	v.sd = NULL;
	v.status = 0;
	v.request_t = -1;
	v.next = freeVouchersList;
	freeVouchersList = v
	freeVouchers++;
	pthread_cond_broacast(&v.status_flag)
	pthread_mutex_unlock(&v.v_lock);	
}
// fxn for read thread cosmuner of bbRead
void fxn_readThrd(void)
{
	voucher *v;
	pthread_mutex_lock(&w_lock);
	pthread_cond_wait(&wait_flag);
	bbRead->blockingRead(bbRead, (void **) &v);
	if(read_sector(DD_ID, *v->sd))
	{
		*v->status = 3;
	}
	else
	{
		*v->status = -1;
	}
	pthread_mutex_unlock(&w_lock);
	pthread_cond_broacast(&wait_flag);

}
// fxn for write thread cosmuner of bbWrite
void fxn_writeThrd(void)
{
	voucher *v;
	pthread_mutex_lock(&w_lock);
	pthread_cond_wait(&wait_flag);
	bbWrite->blockingRead(bbWrite, (void **) &v);
	if(read_sector(*dd, *v->sd))
	{
		*v->status = 3;
	}
	else
	{
		*v->status = -1;
	}
	pthread_mutex_unlock(&w_lock);
	pthread_cond_broacast(&wait_flag);

}
/* queue up sector descriptor for reading 
 * return a Voucher through *v for eventual synchronization by application 
 * queue up sector descriptor for reading 
 * return a Voucher through *v for eventual synchronization by application 
 */ 
void blocking_read_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	pthread_mutex_lock(*v->v_lock);
	*v->sd = sd;
	*v->status++;
	*v->request_t = 1;
	bbRead->blockingWrite(bbRead, (void *)*v);
	*v->status++;
	request_status = 1;
	return request_status
}
/* if you are able to queue up sector descriptor immediately 
 *     return a Voucher through *v and return 1 
 * otherwise, return 0 
 */
int nonblocking_read_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	pthread_mutex_lock(*v->v_lock);
	*v->sd = sd;
	*v->status++;
	*v->request_t = 1;
	if(bbRead->nonblockingWrite(bbRead, (void *)*v) == 1)
	{
		*v->status++;
		request_status = 1;
	}
	else
	{
		request_status = 0;
	}
	pthread_mutex_unlock(*v->v_lock);
	return request_status;
}
/* queue up sector descriptor for writing 
 * return a Voucher through *v for eventual synchronization by application 
 * do not return until it has been successfully queued 
 */ 
void blocking_write_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	pthread_mutex_lock(*v->v_lock);
	*v->sd = sd;
	*v->status++;
	*v->request_t = 0;
	bbWrite->blockingWrite(bbWrite, (void *)*v);
	*v->status++;
	request_status = 1;
	return request_status
}
/* if you are able to queue up sector descriptor immediately 
 *     return a Voucher through *v and return 1
 * otherwise, return 0 
 */
int nonblocking_write_sector(SectorDescriptor *sd, Voucher **v)
{
nt request_status;
	pthread_mutex_lock(*v->v_lock);
	*v->sd = sd;
	*v->status++;
	*v->request_t = 0;
	if(bbWrite->nonblockingWrite(bbWrite, (void *)*v) == 1)
	{
		*v->status++;
		request_status = 1;
	}
	else
	{
		request_status = 0;
	}
	pthread_mutex_unlock(*v->v_lock);
	return request_status;
}
/* 
 * the following call is used to retrieve the status of the read or write 
 * the return value is 1 if successful, 0 if not 
 * the calling application is blocked until the read/write has completed 
 * if a successful read, the associated SectorDescriptor is returned in *sd 
 */ 
int redeem_voucher(Voucher *v, SectorDescriptor **sd)
{
	int value;

	while(complete)
	{

		if(v.status == -1)
		{
			//block calling app
			fpirntf();
			value = 0;
		}
		if(v.status == 3 && v.request_t == 1)
		{
			sd = v.sd;
			value = 1;
			voucherRecyle(v);
		}
		if(v.status == 3 && v.request_t == 0)
		{
			value = 1;
			voucherRecyle(v);
		}
	}
	return value;
}
/* create Free Sector Descriptor Store 
 * load FSDS with packet descriptors constructed from mem_start/mem_length
 * create any buffers required by your thread[s] 
 * create any threads you require for your implementation  
 * return the FSDS to the code that called you 
 */
void init_disk_driver(DiskDevice *dd, void *mem_start, unsigned long mem_length, FreeSectorDesriptorStore **fsds)
{
	DD_ID = dd;
	*fsds =  FreeSectorDesriptorStore_create(mem_start,mem_length);
	bounded_buffer *bbWrite[BBSIZE]; //max pid form document 10
	bounded_buffer *bbRead[BBSIZE];
	Voucher vouchers[NVOUCHERS]; // for demo 30 free sectors to  write to arrray of vouchers
	initFreeVoucher();
	pthread_t WriteThrd_id;
	pthread_t ReadThrd_id;
	pthread_create(&WriteThrd_id, );
	pthread_create(&ReadThrd_id, );

}

int int main(int argc, char const *argv[])
{

}