#include "vvdn_tsu.h"

int update_tod(unsigned int sec, unsigned int nano_sec)
{
        int ret = -1, fd = 0;
        volatile void *tod;
        struct timespec tp;
        int page_addr, page_offset;

        fd = open("/dev/mem", O_RDWR);
        if (fd < 1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

        page_addr = (ToD_REG & ~(PAGE_SIZE - 1));

        tod = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_addr);
        if ((int)tod == -1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

        page_offset = ToD_REG - page_addr;

        tod += page_offset;

        printf(" %s : ToD Seconds = %ld Nanoseconds = %ld\n", __func__, sec, nano_sec);

        *((unsigned *)(tod + ToD_S_LO_OFF)) = sec;
        *((unsigned *)(tod + ToD_S_HI_OFF)) = nano_sec;
        *((unsigned *)(tod + ToD_EN)) = (*((unsigned *)(tod + ToD_EN)) | 0x1);
	usleep(1);
        *((unsigned *)(tod + ToD_EN)) = (*((unsigned *)(tod + ToD_EN)) & 0x0);

        printf(" ToD \n -----\n");
	
        printf(" ToD Seconds_LO = %lx\n", *((long *)(tod + ToD_VER)));
        printf(" ToD Seconds_LO = %lx\n", *((long *)(tod + ToD_S_LO_OFF)));
        printf(" ToD Seconds_HI = %lx\n", *((long *)(tod + ToD_S_HI_OFF)));

	close(fd);

        return 0;
}

int getTimestampFromFPGA(unsigned int mode, TimeInternal *tim)
{
        int ret = -1, fd = 0;
        volatile void *ts;
        int page_addr, page_offset;

        fd = open("/dev/mem", O_RDWR);
        if (fd < 1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

        if (mode == MODE_TX)
                page_addr = (DELAY_REQ_TX_TS_ADDR & ~(PAGE_SIZE - 1));
        else
                page_addr = (SYNC_RX_TS_ADDR & ~(PAGE_SIZE - 1));

        ts = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_addr);
        if ((int)ts == -1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

        if (mode == MODE_TX)
                page_offset = DELAY_REQ_TX_TS_ADDR - page_addr;
        else
                page_offset = SYNC_RX_TS_ADDR - page_addr;

        ts += page_offset;


	printf(" %s : version = %x\n", *((unsigned *)(ts + VERSION)));

        tim->nanoseconds = *((unsigned *)(ts + NSEC));
        tim->seconds = *((unsigned *)(ts + SEC_LO));

        printf("%s : sec_lo = %lx sec_hi = %lx nsec = %lx\n", __func__, tim->seconds, *((unsigned *)(ts + SEC_HI)), tim->nanoseconds);
	
	close(fd);

        return 0;
}

void adjust_mean_pathdelay(PtpClock *ptpClock)
{
	int ret = -1, fd = 0;
        volatile void *adj;
        int page_addr, page_offset;
	long int crct_time;


        fd = open("/dev/mem", O_RDWR);
        if (fd < 1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

	crct_time = ((ptpClock->delaySM.nanoseconds)-(ptpClock->currentDS.meanPathDelay.nanoseconds));
	printf("------Correction: %llx      %lld\n", crct_time, crct_time);
	
        page_addr = (PTP_DELAY_ADJ & ~(PAGE_SIZE - 1));

        adj = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_addr);
        if ((int)adj == -1) {
                printf(" /dev/mem not found \n");
                return -1;
        }

        page_offset = PTP_DELAY_ADJ - page_addr;

        adj += page_offset;

	if (crct_time < 0) {
		crct_time *= -1;
                printf("Positive Value: %ld\n", crct_time);
                *((unsigned *)adj) = crct_time | 0x80000000;
	} else {
		*((unsigned *)adj) = crct_time;
	}

} 
 
