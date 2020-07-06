
#include "defs.h"

#include "stm32f429i_discovery.h"

#define MTU 1500

#define TYPE_LEN           0xFE
#define TYPE_DATA          16
#define MAX_LEN            MTU*TYPE_DATA

#define T1  250

#define TYPE_STATUS 0xfd

static uint8_t buf[MTU+5];

static int get_status(int code)
{
	unsigned int crc_data = 0;
        int r_code;

	if(spi_raw_receive(buf,9,T1)<0)
        {
                 return -1;
        }
	if(buf[0] != TYPE_STATUS)
  	{
  		return -1;
  	}
        memcpy(&r_code,buf+1,4);
        if(r_code != code)
        {
                 return -1;
        }
  	memcpy(&crc_data,buf+5,4);

  	if(crc(buf,5) != crc_data)
  	{
  		return -1;
  	}

  	return 0;
  	
}

static int send_status(int code)
{
  unsigned int crc_data = 0;
  buf[0]= TYPE_STATUS;
  memcpy(buf+1,&code,4);
  crc_data = crc(buf,5);
  memcpy(buf+5,&crc_data,4);
  
  if(spi_raw_send(buf,9,T1)<0)
  {
    return -1;
  }
  return 0;
  
}

 static int x_send(const char *in,unsigned int len)
{
  unsigned int crc_data;
  int n,i;
  int last;
  
  if(len > MAX_LEN)
    return -1;
  n = len/MTU;
  last = len%MTU;
  
  buf[0]= TYPE_LEN;
  memcpy(buf+1,&len,4);
  crc_data = crc(buf,5);
  memcpy(buf+5,&crc_data,4);
  
  if(spi_raw_send(buf,9,T1)<0)
  {
    return -1;
  }
  if(get_status(0)<0)
    return -1;
  
  
  for(i=0;i<n;i++)
  {
    buf[0] = i+1;
    memcpy(buf+1,in+MTU*i,MTU);
    crc_data = crc(buf,MTU+1);
    memcpy(buf+MTU+1,&crc_data,4);
    if(spi_raw_send(buf,MTU+5,T1)<0)
    {
      return -1;
    }
    if(get_status(i+1)<0)
    return -1;
   
  }
  
  if(last != 0)
  {
    buf[0] = n+1;
    memcpy(buf+1,in+MTU*n,last);
    crc_data = crc(buf,last+1);
    memcpy(buf+last+1,&crc_data,4);
    if(spi_raw_send(buf,last+5,T1)<0)
    {
      return -1;
    }
    if(get_status(n+1)<0)
    return -1;
    
  }
  
  if(send_status(-2)<0)
  {
    return -1;
  }
  
  return 0;
  
}





 static int x_recv(char *out,unsigned int *len)
{
        unsigned int crc_data = 0;
	int n,i;
  	int last;


  	memset(buf,0,MTU+5);
        
        if(send_status(-1)<0)
        {
          return -1;
        }
        
        if(spi_raw_receive(buf,9,T1)<0)
        {
                return -1;
        }
        if(buf[0] != TYPE_LEN)
  	{
  		return -1;
  	}
  	memcpy(&crc_data,buf+5,4);

  	if(crc(buf,5) != crc_data)
  	{
  		return -1;
  	}
        memcpy(len,buf+1,4);
        if (*len > MAX_LEN || *len == 0)
  	{
  		return -1;
  	}
        if(send_status(0)<0)
        {
                return -1;
        }
        
        n = *len/MTU;
  	last = *len%MTU;

  	for(i=0;i<n;i++)
  	{
  		memset(buf,0,MTU+5);

  		if(spi_raw_receive(buf,MTU+5,T1)<0)
                {
                        return -1;
                }

  		if(buf[0] != (i+1))
  		{
  			return -1;
  		}
  		memcpy(&crc_data,buf+MTU+1,4);

  		if(crc(buf,MTU+1) != crc_data)
  		{
  			return -1;
  		}
  		memcpy(out+i*MTU,buf+1,MTU);
                
                if(send_status(i+1)<0)
                {
                        return -1;
                }
  
  	}
        
        if (last != 0)
  	{
  		memset(buf,0,MTU+5);

  		if(spi_raw_receive(buf,last+5,T1)<0)
                {
                        return -1;
                }

  		if(buf[0] !=(n+1))
  		{
  			return -1;
  		}
  		memcpy(&crc_data,buf+last+1,4);

  		if(crc(buf,last+1)!=crc_data)
  		{
  			return -1;
  		}
  		memcpy(out+n*MTU,buf+1,last);
                if(send_status(n+1)<0)
                {
                        return -1;
                }
  	}
        
        return 0;
        

}

int spi_recv_packet(char *out,unsigned int *len,unsigned int timeout)
{
   unsigned int count = HAL_GetTick() +timeout;
   
   while(x_recv(out,len)<0)
   {
     if(count< HAL_GetTick())
       return -1;
   }
   return (HAL_GetTick()-count+timeout);
}

int spi_send_packet(const char *in,unsigned int len,unsigned int timeout)
{
  unsigned int count = HAL_GetTick() +timeout;
   
   while(x_send(in,len)<0)
   {
     if(count< HAL_GetTick())
       return -1;
   }
   return (HAL_GetTick()-count+timeout);
}






