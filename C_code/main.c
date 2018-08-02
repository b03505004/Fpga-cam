#include <stdio.h>
#include "I2C_core.h"
#include "terasic_includes.h"
#include "mipi_camera_config.h"
#include "mipi_bridge_config.h"
#include "auto_focus.h"
#include "altera_msgdma.h"
#include "dma.h"


void* lw_virtual_base = (void*)0;
#include <sys/mman.h>
#include <fcntl.h>

#define DEFAULT_LEVEL 2

#define MIPI_REG_PHYClkCtl		0x0056
#define MIPI_REG_PHYData0Ctl	0x0058
#define MIPI_REG_PHYData1Ctl	0x005A
#define MIPI_REG_PHYData2Ctl	0x005C
#define MIPI_REG_PHYData3Ctl	0x005E
#define MIPI_REG_PHYTimDly		0x0060
#define MIPI_REG_PHYSta			0x0062
#define MIPI_REG_CSIStatus		0x0064
#define MIPI_REG_CSIErrEn		0x0066
#define MIPI_REG_MDLSynErr		0x0068
#define MIPI_REG_FrmErrCnt		0x0080
#define MIPI_REG_MDLErrCnt		0x0090

//settings for the lightweight HPS-to-FPGA bridge
#define ALT_STM_OFST 0xFC000000
#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 ) //64 MB with 32 bit adress space this is 256 MB

#define DMA_CONTROLLER 0x20000



void mipi_clear_error(void){
	MipiBridgeRegWrite(MIPI_REG_CSIStatus,0x01FF); // clear error
	MipiBridgeRegWrite(MIPI_REG_MDLSynErr,0x0000); // clear error
	MipiBridgeRegWrite(MIPI_REG_FrmErrCnt,0x0000); // clear error
	MipiBridgeRegWrite(MIPI_REG_MDLErrCnt, 0x0000); // clear error

  	MipiBridgeRegWrite(0x0082,0x00);
  	MipiBridgeRegWrite(0x0084,0x00);
  	MipiBridgeRegWrite(0x0086,0x00);
  	MipiBridgeRegWrite(0x0088,0x00);
  	MipiBridgeRegWrite(0x008A,0x00);
  	MipiBridgeRegWrite(0x008C,0x00);
  	MipiBridgeRegWrite(0x008E,0x00);
  	MipiBridgeRegWrite(0x0090,0x00);
}

void mipi_show_error_info(void){

	alt_u16 PHY_status, SCI_status, MDLSynErr, FrmErrCnt, MDLErrCnt;

	PHY_status = MipiBridgeRegRead(MIPI_REG_PHYSta);
	SCI_status = MipiBridgeRegRead(MIPI_REG_CSIStatus);
	MDLSynErr = MipiBridgeRegRead(MIPI_REG_MDLSynErr);
	FrmErrCnt = MipiBridgeRegRead(MIPI_REG_FrmErrCnt);
	MDLErrCnt = MipiBridgeRegRead(MIPI_REG_MDLErrCnt);
	printf("PHY_status=%xh, CSI_status=%xh, MDLSynErr=%xh, FrmErrCnt=%xh, MDLErrCnt=%xh\r\n", PHY_status, SCI_status, MDLSynErr,FrmErrCnt, MDLErrCnt);
}

void mipi_show_error_info_more(void){
    printf("FrmErrCnt = %d\n",MipiBridgeRegRead(0x0080));
    printf("CRCErrCnt = %d\n",MipiBridgeRegRead(0x0082));
    printf("CorErrCnt = %d\n",MipiBridgeRegRead(0x0084));
    printf("HdrErrCnt = %d\n",MipiBridgeRegRead(0x0086));
    printf("EIDErrCnt = %d\n",MipiBridgeRegRead(0x0088));
    printf("CtlErrCnt = %d\n",MipiBridgeRegRead(0x008A));
    printf("SoTErrCnt = %d\n",MipiBridgeRegRead(0x008C));
    printf("SynErrCnt = %d\n",MipiBridgeRegRead(0x008E));
    printf("MDLErrCnt = %d\n",MipiBridgeRegRead(0x0090));
    printf("FIFOSTATUS = %d\n",MipiBridgeRegRead(0x00F8));
    printf("DataType = 0x%04x\n",MipiBridgeRegRead(0x006A));
    printf("CSIPktLen = %d\n",MipiBridgeRegRead(0x006E));
}



bool MIPI_Init(void){
	bool bSuccess;
	
	int i;
    for (i=0; i<8;i++){
        printf("%x:\t%x\n", i, IORD(I2C_OPENCORES_MIPI_BASE, i));
    }
	printf("AFTER________________________________________________\n");
	printf("100\n");
	bSuccess = oc_i2c_init_ex(I2C_OPENCORES_MIPI_BASE, 50*1000*1000,400*1000); //I2C: 400K
	if (!bSuccess)
		printf("failed to init MIPI- Bridge i2c\r\n");
    for (i=0; i<8;i++){
        printf("%x:\t%x\n", i, IORD(I2C_OPENCORES_MIPI_BASE, i));
    }
    usleep(50*1000);
    MipiBridgeInit();

    usleep(500*1000);

//	bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
//	if (!bSuccess)
//		printf("failed to init MIPI- Camera i2c\r\n");

    MipiCameraInit();
    MIPI_BIN_LEVEL(DEFAULT_LEVEL);
//    OV8865_FOCUS_Move_to(340);

//    oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);  // Release I2C bus , due to two I2C master shared!


 	usleep(1000);


//    oc_i2c_uninit(I2C_OPENCORES_MIPI_BASE);

	return bSuccess;
}



alt_msgdma_dev * STDATA_MSGDMA;
alt_msgdma_standard_descriptor STDATA_MSGDMA_DESC;
#define LEN 168960 //640*88*24/8 --> 88lines
//#define LEN 100
alt_u32 *WRITE_ADDRESS = (alt_u32*) 0x80000;
alt_u32 *WRITE_ADDRESS_2 = (alt_u32*) 0xA9408;
#define SDRAM_16_BASE 0xA6000000
#define SDRAM_16_SPAN 0xFFFFFF

void msgdma_init(alt_msgdma_dev *dev){
	dev->llist = (alt_llist){0,0};
	dev->name = "/dev/msgdma_0_csr";
	dev->csr_base = (alt_u32*)0x30020;
	dev->descriptor_base = (alt_u32*)0x30040;
	dev->response_base = (alt_u32*)0x0;
	dev->prefetcher_base = (alt_u32*)0x0;
	dev->irq_controller_ID = (alt_u32)0x0;
	dev->irq_ID = (alt_u32)0x4;
	dev->descriptor_fifo_depth = (alt_u32)0x80;
	dev->response_fifo_depth = (alt_u32)0x0;
	dev->callback = (void*)0x0;
	dev->callback_context = (alt_u32*)0x0;
	dev->control = (alt_u32)0x0;
	dev->burst_enable = (alt_u8)0x0;
	dev->burst_wrapping_support = (alt_u32)0x0;
	dev->data_fifo_depth = (alt_u32)0x20;
	dev->data_width = (alt_u32)0x8;
	dev->max_burst_count = (alt_u32)0x2;
	dev->max_byte = (alt_u32)0x200000;
	dev->max_stride = (alt_u64)0x1;
	dev->programmable_burst_enable = (alt_u8)0;
	dev->stride_enable = (alt_u8)0;
	dev->transfer_type = "Aligned Accesses";
	dev->enhanced_features = (alt_u8)0;
	dev->response_port = (alt_u8)0x2;
	dev->prefetcher_enable = (alt_u8)0;
	return;
}


void debugMSGDMA(){
	printf("***************************************DEBUG***************************************\n");
	printf("size: %x\n const char: %x \n", sizeof(alt_msgdma_dev), sizeof(STDATA_MSGDMA->name));
	printf("MSGDMA: %x\n", (alt_u32*)STDATA_MSGDMA);
	printf("dev->list = %x\n", STDATA_MSGDMA->llist);
	printf("dev->name = %s\n", STDATA_MSGDMA->name);
	printf("dev->csr_base = %x\n", (alt_u32*)STDATA_MSGDMA->csr_base);
	printf("dev->descriptor_base = %x\n", (alt_u32*)STDATA_MSGDMA->descriptor_base);
	printf("dev->response_base = %x\n", (alt_u32*)STDATA_MSGDMA->response_base);
	printf("dev->prefetcher_base = %x\n", (alt_u32*)STDATA_MSGDMA->prefetcher_base);
	printf("dev->irq_controller_ID = %x\n", (alt_u32)STDATA_MSGDMA->irq_controller_ID);
	printf("dev->irq_ID = %x\n", (alt_u32)STDATA_MSGDMA->irq_ID);
	printf("dev->descriptor_fifo_depth = %x\n", (alt_u32)STDATA_MSGDMA->descriptor_fifo_depth);
	printf("dev->response_fifo_depth = %x\n", (alt_u32)STDATA_MSGDMA->response_fifo_depth);
	printf("dev->callback = %x\n", (alt_u32*)STDATA_MSGDMA->callback);
	printf("dev->callback_context = %x\n", (alt_u32*)STDATA_MSGDMA->callback_context);
	printf("dev->control = %x\n", (alt_u32)STDATA_MSGDMA->control);
	printf("dev->burst_enable = %x\n", (alt_u8)STDATA_MSGDMA->burst_enable);
	printf("dev->burst_wrapping_support = %x\n", (alt_u8)STDATA_MSGDMA->burst_wrapping_support);
	printf("dev->data_fifo_depth = %x\n", (alt_u32)STDATA_MSGDMA->data_fifo_depth);
	printf("dev->data_width = %x\n", (alt_u32)STDATA_MSGDMA->data_width);
	printf("dev->max_burst_count = %x\n", (alt_u32)STDATA_MSGDMA->max_burst_count);
	printf("dev->max_byte = %x\n", (alt_u32)STDATA_MSGDMA->max_byte);
	printf("dev->max_stride = %x\n", (alt_u64)STDATA_MSGDMA->max_stride);
	printf("dev->programmable_burst_enable = %x\n", (alt_u8)STDATA_MSGDMA->programmable_burst_enable);
	printf("dev->stride_enable = %x\n", (alt_u8)STDATA_MSGDMA->stride_enable);
	printf("dev->transfer_type = %s\n", STDATA_MSGDMA->transfer_type);
	printf("dev->enhanced_features = %x\n", (alt_u8)STDATA_MSGDMA->enhanced_features);
	printf("dev->response_port = %x\n", (alt_u8)STDATA_MSGDMA->response_port);
	printf("dev->prefetcher_enable = %x\n", (alt_u8)STDATA_MSGDMA->prefetcher_enable);
	printf("***************************************DEBUG***************************************\n");
}


int main()
{
	
	int fd;
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}
	lw_virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( lw_virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	
	printf("DE1-SoC D8M VGA Demo\n");
	printf("%x\n", (alt_u8*)lw_virtual_base);

	IOWR(MIPI_PWDN_N_BASE, 0x00, 0x00);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0x00);

	usleep(2000);
	IOWR(MIPI_PWDN_N_BASE, 0x00, 0xFF);
	usleep(2000);
	IOWR(MIPI_RESET_N_BASE, 0x00, 0xFF);


	usleep(2000);


	// MIPI Init
	if (!MIPI_Init()){
		printf("MIPI_Init Init failed!\r\n");
	}else{
		printf("MIPI_Init Init successfully!\r\n");
	}

	//   while(1){
		mipi_clear_error();
		usleep(50*1000);
		mipi_clear_error();
		usleep(1000*1000);
		mipi_show_error_info();
	//	    mipi_show_error_info_more();
		printf("\n");
	//   }


	#if 0  // focus sweep
		printf("\nFocus sweep\n");
		alt_u16 ii= 350;
		alt_u8  dir = 0;
		while(1){
			if(ii< 50) dir = 1;
			else if (ii> 1000) dir =0;

			if(dir) ii += 20;
			else    ii -= 20;

			printf("%d\n",ii);
			OV8865_FOCUS_Move_to(ii);
			usleep(50*1000);
		}
	#endif




	printf( "\n\n------------- mSGDMA---------------\n\n" );
	//STDATA_MSGDMA = sdram_16MB_add;

	void* onchip_addr;
	onchip_addr = lw_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ONCHIP_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	int i;
	for (i=0;i<1000;i++){
		*((alt_u8 *)onchip_addr+i)=i;
	}
	printf("written\n");
	h2p_lw_dma_addr=lw_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + DMA_CONTROLLER ) & ( unsigned long)( HW_REGS_MASK ) );
	_DMA_REG_STATUS(h2p_lw_dma_addr)=0;
	_DMA_REG_READ_ADDR(h2p_lw_dma_addr)=0x80000; //read from ROM1
	_DMA_REG_WRITE_ADDR(h2p_lw_dma_addr)=0x81000; //write to ROM2
	_DMA_REG_LENGTH(h2p_lw_dma_addr)=1000;//write 100x 4bytes since we have a 32 bit system
	_DMA_REG_CONTROL(h2p_lw_dma_addr)=_DMA_CTR_WORD | _DMA_CTR_GO | _DMA_CTR_LEEN;

	debugPrintDMARegister();

	debugPrintDMAStatus();

	//wait for DMA to be finished
	waitDMAFinish();
	stopDMA();//stop the DMA controller

	void *sdram_16MB_add;
	//void* onchip_addr;
	//onchip_addr = lw_virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + ONCHIP_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	sdram_16MB_add=mmap( NULL, SDRAM_16_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, SDRAM_16_BASE );
	STDATA_MSGDMA = sdram_16MB_add;
	
	msgdma_init(STDATA_MSGDMA);
	debugMSGDMA();
	
	if(STDATA_MSGDMA == NULL)
		printf("Could not open the mSG-DMA!\r\n");
	printf("STDATA_MSGDMA: %x", (alt_u32*)STDATA_MSGDMA);
	printf("dev fin\n");

	alt_msgdma_init(STDATA_MSGDMA, STDATA_MSGDMA->irq_controller_ID, STDATA_MSGDMA->irq_ID);
	alt_msgdma_construct_standard_st_to_mm_descriptor(
	STDATA_MSGDMA,
	&STDATA_MSGDMA_DESC,
	WRITE_ADDRESS,
	LEN,
	ALTERA_MSGDMA_DESCRIPTOR_CONTROL_PARK_WRITES_MASK);
	
	alt_u32 csr_status = 0;

	csr_status = IORD_ALTERA_MSGDMA_CSR_STATUS(STDATA_MSGDMA->csr_base);
	printf("csr status: %x\n", csr_status);

	alt_msgdma_standard_descriptor_async_transfer(STDATA_MSGDMA, &STDATA_MSGDMA_DESC);

	debugMSGDMA();

	csr_status = IORD_ALTERA_MSGDMA_CSR_STATUS(STDATA_MSGDMA->csr_base);
	printf("csr status: %x\n", csr_status);
	printf("wa: %x\n",(alt_u32)&STDATA_MSGDMA_DESC);
	printf("wa: %x\n",(alt_u32)STDATA_MSGDMA_DESC.write_address);
	printf("wa: %x\n",(alt_u32)STDATA_MSGDMA_DESC.read_address);
	printf("wa: %x\n",(alt_u32)STDATA_MSGDMA_DESC.control);
	printf("wa: %x\n",(alt_u32)STDATA_MSGDMA_DESC.transfer_length);
	int a = 0;
  //for(int i=0; i<3; i++){
	  //
		printf("%x\n", (alt_u8 *)onchip_addr+21000);
	  while(*((alt_u8 *)onchip_addr+21000)==0){
		 a+=1;

		 printf("%d\n",a);
		 //printf("%d, %d\n",a, *dataEnd);
	  }

  printf("ABC\n");
  alt_msgdma_construct_standard_st_to_mm_descriptor(
  	STDATA_MSGDMA,
  	&STDATA_MSGDMA_DESC,
	WRITE_ADDRESS_2,
  	1,
  	ALTERA_MSGDMA_DESCRIPTOR_CONTROL_PARK_WRITES_MASK);

    alt_msgdma_standard_descriptor_async_transfer(STDATA_MSGDMA, &STDATA_MSGDMA_DESC);
	//////////////////////////////////////////////////////////
		alt_u16 bin_level = DEFAULT_LEVEL;
		alt_u8  manual_focus_step = 10;
		alt_u16  current_focus = 300;
		Focus_Init();
	while(1){

		// touch KEY0 to trigger Auto focus
		if((IORD(KEY_BASE,0)&0x0F) == 0x0E){

			current_focus = Focus_Window(320,240);
		}

		// touch KEY1 to trigger Manual focus  - step
		if((IORD(KEY_BASE,0)&0x0F) == 0x0D){

			if(current_focus > manual_focus_step) current_focus -= manual_focus_step;
			else current_focus = 0;
			OV8865_FOCUS_Move_to(current_focus);

		}

		// touch KEY2 to trigger Manual focus  + step
		if((IORD(KEY_BASE,0)&0x0F) == 0x0B){
			current_focus += manual_focus_step;
			if(current_focus >1023) current_focus = 1023;
			OV8865_FOCUS_Move_to(current_focus);
		}

		// touch KEY3 to ZOOM
		if((IORD(KEY_BASE,0)&0x0F) == 0x07){
			if(bin_level == 3 )bin_level = 1;
			else bin_level ++;
			printf("set bin level to %d\n",bin_level);
			MIPI_BIN_LEVEL(bin_level);
			usleep(500000);

		}

		usleep(200000);



	};

	if( munmap( lw_virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	close( fd );
	return 0;
}
