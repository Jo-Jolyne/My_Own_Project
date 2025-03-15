#include "w5500.h"	
unsigned char Gateway_IP[4];
unsigned char Sub_Mask[4];	
unsigned char Phy_Addr[6];	
unsigned char IP_Addr[4];	

uint16_t S_Port[8];			
unsigned char S0_DIP[4];	
unsigned char S0_DPort[2];	 

unsigned char UDP_DIPR[4];		
unsigned char UDP_DPORT[2];	

uint8_t S_Mode[8] = {3,3,3,3,3,3,3,3};	
#define TCP_SERVER	0x00			
#define TCP_CLIENT	0x01			
#define UDP_MODE	0x02			

uint8_t S_State[8] = {0,0,0,0,0,0,0,0};	 
#define S_INIT		0x01			
#define S_CONN		0x02

uint8_t S_Data[8] = {0,0,0,0,0,0,0,0};	 
#define S_RECEIVE	 0x01		 
#define S_TRANSMITOK 0x02

void SPI_Send_Byte(unsigned char dat)
{
	HAL_SPI_Transmit(&hspi2, &dat, 1, 1000);
}

void SPI_Send_Short(unsigned short dat)
{	
	dat = u16EdianChange(dat);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&dat, 2, 1000);
}

unsigned char SPI_Read_Byte(void)
{
	uint8_t tx=0x00;
	uint8_t result;

	HAL_SPI_TransmitReceive(&hspi2, &tx, &result, 1, 1000);
	
	return result;
}

void Write_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);

	SPI_Send_Short(reg);																
    SPI_Send_Byte(FDM1|RWB_WRITE|COMMON_R);	
	SPI_Send_Byte(dat);	

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);	
}

void Write_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);			
	SPI_Send_Short(reg);
    SPI_Send_Byte(FDM2|RWB_WRITE|COMMON_R);
    SPI_Send_Short(dat);
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);	
}

void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(VDM|RWB_WRITE|COMMON_R);
	HAL_SPI_Transmit(&hspi2, dat_ptr, size, 1000);
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);	
}

void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM1|RWB_WRITE|(s*0x20+0x08));
	SPI_Send_Byte(dat);	
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
}

void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM2|RWB_WRITE|(s*0x20+0x08));
	SPI_Send_Short(dat);
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
}

void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM4|RWB_WRITE|(s*0x20+0x08));
	HAL_SPI_Transmit(&hspi2, dat_ptr, 4, 1000);
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
}

unsigned char Read_W5500_1Byte(unsigned short reg)
{
	unsigned char i;

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM1|RWB_READ|COMMON_R);
	i = SPI_Read_Byte();
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
	return i;
}

unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char i;

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM1|RWB_READ|(s*0x20+0x08));
	i = SPI_Read_Byte();
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
	return i;										
}

unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short txdata = 0;
	unsigned short rxdata = 0;

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(reg);
	SPI_Send_Byte(FDM2|RWB_READ|(s*0x20+0x08));
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)&txdata, (uint8_t *)&rxdata, 2, 1000);

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
	rxdata = u16EdianChange(rxdata);
	return rxdata;						
}


unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;
	unsigned char j;

	rx_size=Read_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size==0) return 0;							
	if(rx_size>1460) rx_size=1460;

	offset=Read_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1=offset;
	offset&=(S_RX_SIZE-1);	

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);
	SPI_Send_Short(offset);					
	SPI_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));

	if((offset+rx_size)<S_RX_SIZE)			
	{
		for(i=0;i<rx_size;i++)				
		{
			j = SPI_Read_Byte();			
			*dat_ptr=j;					
			dat_ptr++;					
		}
	}
	else							
	{
		offset=S_RX_SIZE-offset;
		for(i=0;i<offset;i++)				
		{
			j = SPI_Read_Byte();			
			*dat_ptr=j;					
			dat_ptr++;					
		}
		HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);	
		HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);	

		SPI_Send_Short(0x00);
		SPI_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));
		for(;i<rx_size;i++)			
		{
			j = SPI_Read_Byte();	
			*dat_ptr=j;			
			dat_ptr++;			
		}
	}

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
	offset1+=rx_size;								
	Write_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, RECV);		
	return rx_size;
}

void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	//unsigned short i;
	// unsigned short TxSize = (s < 2)?S0_TX_MEM_SIZE:Sn_TX_MEM_SIZE;
	#ifdef USE_UDP
	if((Read_W5500_SOCK_1Byte(s,Sn_MR)&0x0f) == MR_UDP)	
	{		
		Write_W5500_SOCK_4Byte(s, Sn_DIPR, UDP_DIPR);		 		
		Write_W5500_SOCK_2Byte(s, Sn_DPORTR, ((uint16_t)(UDP_DPORT[0])<<8)+UDP_DPORT[1]);	
    }
	#endif

	offset = Read_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1 = offset;
	offset &= (S_TX_SIZE-1);

	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);			
    SPI_Send_Short(offset);						
	SPI_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));

	if((offset + size) < S_TX_SIZE)						
	{
		HAL_SPI_Transmit(&hspi2, dat_ptr, size, 1000);
	}
	else											
	{
		offset = S_TX_SIZE - offset;
		HAL_SPI_Transmit(&hspi2, dat_ptr, offset, 1000);
		HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_RESET);	
		SPI_Send_Short(0x00);											
		SPI_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));
		HAL_SPI_Transmit(&hspi2, (dat_ptr + offset), (size - offset), 1000);
	}
	HAL_GPIO_WritePin(W5500_SPI2_CS_GPIO_Port, W5500_SPI2_CS_Pin, GPIO_PIN_SET);	
	offset1 += size;														
	Write_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_W5500_SOCK_1Byte(s, Sn_CR, SEND);						
}

void W5500_Hardware_Reset(void)
{	
	HAL_GPIO_WritePin(W5500_SPI2_RST_GPIO_Port, W5500_SPI2_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(W5500_SPI2_RST_GPIO_Port, W5500_SPI2_RST_Pin, GPIO_PIN_SET);	
	HAL_Delay(200);
//	while((Read_W5500_1Byte(PHYCFGR)&LINK)==0);							
}

uint8_t W5500_Link_Detect(void)
{	
	return (Read_W5500_1Byte(PHYCFGR)&LINK);
}

void W5500_Init(void)
{
	uint8_t i=0;
	Write_W5500_1Byte(MR, RST);
	HAL_Delay(10);
	Write_W5500_nByte(GAR, Gateway_IP, 4);
	Write_W5500_nByte(SUBR,Sub_Mask,4);
	Write_W5500_nByte(SHAR,Phy_Addr,6);
	Write_W5500_nByte(SIPR,IP_Addr,4);
	for(i=0;i<8;i++)
	{
		Write_W5500_SOCK_1Byte(i,Sn_RXBUF_SIZE, 0x02);//Socket Rx memory size=2k
		Write_W5500_SOCK_1Byte(i,Sn_TXBUF_SIZE, 0x02);//Socket Tx mempry size=2k
	}
	Write_W5500_2Byte(RTR, 0x07d0);
	Write_W5500_1Byte(RCR,8);
}

unsigned char Detect_Gateway(void)
{
	unsigned char ip_adde[4];
	ip_adde[0]=IP_Addr[0]+1;
	ip_adde[1]=IP_Addr[1]+1;
	ip_adde[2]=IP_Addr[2]+1;
	ip_adde[3]=IP_Addr[3]+1;

	Write_W5500_SOCK_4Byte(0,Sn_DIPR,ip_adde);
	Write_W5500_SOCK_1Byte(0,Sn_MR,MR_TCP);
	Write_W5500_SOCK_1Byte(0,Sn_CR,OPEN);
	HAL_Delay(5);
	if(Read_W5500_SOCK_1Byte(0,Sn_SR) != SOCK_INIT)	
	{
		Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);
		return FALSE;
	}

	Write_W5500_SOCK_1Byte(0,Sn_CR,CONNECT);
    
	do
	{
		uint8_t j=0;
		j=Read_W5500_SOCK_1Byte(0,Sn_IR);
		if(j!=0)
		Write_W5500_SOCK_1Byte(0,Sn_IR,j);
		HAL_Delay(5);
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			return FALSE;	
		}
		else if(Read_W5500_SOCK_1Byte(0,Sn_DHAR) != 0xff)
		{
			Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);
			return TRUE;
		}
	}while(1);
}

void Socket_Init(SOCKET s)
{
	Write_W5500_SOCK_2Byte(s, Sn_MSSR, 1460);
	Write_W5500_SOCK_1Byte(s, Sn_KPALVTR, 0x02);
	Write_W5500_SOCK_2Byte(s, Sn_PORT, S_Port[s]);
}

unsigned char Socket_Connect(SOCKET s)
{
	if(((S0_DIP[0] == 0xFF) && (S0_DIP[1] == 0xFF) && (S0_DIP[2] == 0xFF) && (S0_DIP[3] == 0xFF)) || \
       ((S0_DIP[0] == 0x00) && (S0_DIP[1] == 0x00) && (S0_DIP[2] == 0x00) && (S0_DIP[3] == 0x00)) || \
       ((S0_DPort[0] == 0x00)&&(S0_DPort[1] == 0x00)))
    {
      return FALSE;	
    }
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}

	Write_W5500_SOCK_4Byte(s,Sn_DIPR,S0_DIP);
	Write_W5500_SOCK_2Byte(s,Sn_DPORTR,(S0_DPort[0]<<8)+S0_DPort[1]);
	Write_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);
	return TRUE;
}

unsigned char Socket_Listen(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);	
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);	
		return FALSE;
	}	
	Write_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);	
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}
	return TRUE;
}

unsigned char Socket_UDP(SOCKET s)
{
	Write_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP);
	Write_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	if(Read_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)
	{
		Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}
	else
		return TRUE;
}

void W5500_Interrupt_Process(void)
{
	unsigned char i,j;
	SOCKET s;
IntDispose:
	i=Read_W5500_1Byte(SIR);
	for(s=0;s<SOCK_NUM;s++)
	{
		if((i & (S0_INT<<s)) == (S0_INT<<s))
		{
			j=Read_W5500_SOCK_1Byte(s,Sn_IR);
			Write_W5500_SOCK_1Byte(s,Sn_IR,j);
			if(j&IR_CON)
			{
				S_State[s]|=S_CONN;
				//注释下面代码就可以连接多个client,同时修改SOCK_NUM来限制client的数量
				//add by cyc for test 20230224
				// if(0==s)
				// {
				// 	if(S_State[1]&S_CONN)
				// 	{
				// 		Write_W5500_SOCK_1Byte(1,Sn_CR,CLOSE);
				// 		Socket_Init(1);	
				// 		S_State[1]=0;
				// 	}
				// }
				// else if(1==s)
				// {
				// 	if(S_State[0]&S_CONN)
				// 	{
				// 		Write_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);
				// 		Socket_Init(0);	
				// 		S_State[0]=0;
				// 	}
				// }
				//end add
			}
			if(j&IR_DISCON)	
			{
				Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
				Socket_Init(s);	
				S_State[s]=0;
			}
			if(j&IR_SEND_OK)
			{
				S_Data[s]|=S_TRANSMITOK;
			}
			if(j&IR_RECV)
			{
				S_Data[s]|=S_RECEIVE;
			}
			if(j&IR_TIMEOUT)
			{
				Write_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
				S_State[s]=0;
			}
		}
	}

	if(Read_W5500_1Byte(SIR) != 0)
		goto IntDispose;
}
void W5500_Initialization(void)
{
	SOCKET s;
	W5500_Init();
	Detect_Gateway();
	for(s=0;s<SOCK_NUM;s++)
	{
		Socket_Init(s);
	}
}

void W5500_Socket_Set(void)
{
	SOCKET s;
	for(s=0;s<SOCK_NUM;s++)
	{
		if(S_State[s]==0)
		{
			if(S_Mode[s]==TCP_SERVER)
			{
				if(Socket_Listen(s)==TRUE)
					S_State[s]=S_INIT;
				else
					S_State[s]=0;
			}
			else if(S_Mode[s]==TCP_CLIENT)
			{
				if(Socket_Connect(s)==TRUE)
					S_State[s]=S_INIT;
				else
					S_State[s]=0;
			}
			else
			{
				if(Socket_UDP(s)==TRUE)
					S_State[s]=S_INIT|S_CONN;
				else
					S_State[s]=0;
			}
		}
	}
}


