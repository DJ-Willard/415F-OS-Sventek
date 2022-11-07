#include "BoundedBuffer.h"
#include "diskdriver.h"
//#include "diskdevice__full.h"
//#include "freesectordescriptorstore__full.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))

#define NVOUCHERS 30
#define BBSIZE 10
/* any global variables required for use by your threads and your driver routines */
// queue of write jobs
bounded_buffer *bbWrite = NULL;
//queue of read jobs
bounded_buffer *bbRead = NULL;
//free voucher list stored in queue
bounded_buffer *bbVoucher = NULL;
//for diskdevice form init
DiskDevice *DD_ID = NULL;
 
/* you must define struct voucher and allocate a sufficient number of them in your 
 * declared data; the structure must enable application threads to block until the 
 * associated read/write has completed 
 * status: 0 not assigned, 1 assigned, 2 if in w/r queue, 3 succesful write, -1 failure to write
 * request_t: 0 = write 1 = read -1= default value
*/ 
struct voucher{
	SectorDescriptor *sd; // pointer to the sector discriptor handed to us.
	int status;
	int request_t;  
	pthread_mutex_t v_lock; // protect data
	pthread_cond_t status_flag; // protect critcial region.
};

struct wtArgs{
}WTArgs;

struct rtArgs{
}RTArgs;

//array of voucher based off nember of free sectors 30 for init.
Voucher vouchers[NVOUCHERS];

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
		bbVoucher->blockingWrite(bbVoucher, (void *) (&vouchers[i]));
	}
}
/* definition[s] of function[s] required for your thread[s] */ 
//helper fuction assign vouvher form free voucher list 
//resourced form lab5  blocking, each process needs a voucher to continue
void voucherAssignment(Voucher *stub)
{
	bbVoucher->blockingRead(bbVoucher, (void **) &stub);
}
//helper fuction recycle used vouchers to freevoucher list  resourced form lab5 blocking
void voucherRecyle(Voucher *v)
{
	pthread_mutex_lock(&v->v_lock);
	v->sd = NULL;
	v->status = 0;
	v->request_t = -1;
	bbVoucher->blockingWrite(bbVoucher, (void *) v);
	pthread_cond_broacast(&v->status_flag);
	pthread_mutex_unlock(&v->v_lock);	
}
// fxn for read thread cosmuner of bbRead
void fxn_readThrd(void)
{
	voucher *v;
	bbRead->blockingRead(bbRead, (void **) &v);
	pthread_mutex_lock(&v->v_lock);
	pthread_cond_wait(&v->status_flag);
	if(read_sector(DD_ID, *v->sd))
	{
		*v->status = 3;
	}
	else
	{
		*v->status = -1;
	}
	pthread_mutex_unlock(&v->v_lock);
	pthread_cond_broacast(&v->status_flag);

}
// fxn for write thread cosmuner of bbWrite
void fxn_writeThrd(void)
{
	voucher *v;
	bbWrite->blockingRead(bbWrite, (void **) &v);
	pthread_mutex_lock(&v->v_lock);
	pthread_cond_wait(&v->status_flag);
	if(read_sector(*dd, *v->sd))
	{
		*v->status = 3;
	}
	else
	{
		*v->status = -1;
	}
	pthread_mutex_unlock(&v->v_lock);
	pthread_cond_broacast(&v->status_flag);

}
/* queue up sector descriptor for reading 
 * return a Voucher through *v for eventual synchronization by application 
 * queue up sector descriptor for reading 
 * return a Voucher through *v for eventual synchronization by application 
 */ 
void blocking_read_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	Voucher *r;
	voucherAssignment(r);
	r->sd = sd;
	r->status++;
	r->request_t = 1;
	bbRead->blockingWrite(bbRead, (void *) r);
	r->status++;
	request_status = 1;
	v = &r;
	return request_status
}
/* if you are able to queue up sector descriptor immediately 
 *     return a Voucher through *v and return 1 
 * otherwise, return 0 
 */
int nonblocking_read_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	Voucher *r;
	voucherAssignment(r);
	r->sd = sd;
	r->status++;
	r->request_t = 1;
	if(bbRead->nonblockingWrite(bbRead, (void *) r) == 1)
	{
		r->status++;
		v = &r;
		request_status = 1;
	}
	else
	{
		voucherRecyle(r);
		request_status = 0;
	}
	return request_status;
}
/* queue up sector descriptor for writing 
 * return a Voucher through *v for eventual synchronization by application 
 * do not return until it has been successfully queued 
 */ 
void blocking_write_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;
	Voucher *w;
	voucherAssignment(w);
	w->sd = sd;
	w->status++;
	w->request_t = 0;
	bbWrite->blockingWrite(bbWrite, (void *) w);
	w->status++;
	request_status = 1;
	v = &w;
	return request_status
}
/* if you are able to queue up sector descriptor immediately 
 *     return a Voucher through *v and return 1
 * otherwise, return 0 
 */
int nonblocking_write_sector(SectorDescriptor *sd, Voucher **v)
{
	int request_status;

	pthread_mutex_lock(w->v_lock);
	w->sd = sd;
	w->status++;
	w->request_t = 0;
	if(bbWrite->nonblockingWrite(bbWrite, (void *)w) == 1)
	{
		w->status++;
		request_status = 1;
	}
	else
	{
		request_status = 0;
	}
	pthread_mutex_unlock(w->v_lock);
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

	pthread_mutex_lock(&s.v_lock);
	while(v->status > -1 || v->status < 3)
	{
		pthread_cond_wait(&s.status_flag);
	}
	if(v->status == -1)
		{
			//block calling app
			//fpirntf();
			value = 0;
		}
		if(v->status == 3 && v->request_t == 1)
		{
			sd = v.sd;
			value = 1;
			voucherRecyle(v);
		}
		if(v->status == 3 && v->request_t == 0)
		{
			value = 1;
			voucherRecyle(v);
		}
	return value;
}
/* create Free Sector Descriptor Store 
 * load FSDS with packet descriptors constructed from mem_start/mem_length
 * create any buffers required by your thread[s] 
 * create any threads you require for your implementation  
 * return the FSDS to the code that called you 
 */
void init_disk_driver(DiskDevice *dd, void *mem_start, unsigned long mem_length, FreeSectorDescriptorStore **fsds)
{
	DD_ID = dd;
	*fsds =  FreeSectorDescriptorStore_create(mem_start,mem_length);
	if
	bounded_buffer *bbWrite[BBSIZE] = BoundedBuffer_create(BBSIZE); //max pid form document 10
	bounded_buffer *bbRead[BBSIZE] = BoundedBuffer_create(BBSIZE);
	bounded_buffer *bbVoucher[NVOUCHERS] = BoundedBuffer_create(NVOUCHERS);
	initFreeVoucher();
	pthread_t WriteThrd_id;
	pthread_t ReadThrd_id;
	pthread_create(&WriteThrd_id, NULL, &fxn_writeThrd, &WTArgs);
	pthread_create(&ReadThrd_id, NULL, &fxn_readThrd, &RTArgs);

}

// int main(int UNUSED argc, char UNUSED const *argv[])
// {

// }