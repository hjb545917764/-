#include "io2iic.h"
#include "gpio.h"
#include "common.h"

#include <stdint.h>


#define SCL(x) GPIO_WriteBit(IIC_SCL_PORT ,IIC_SCL_PIN ,x)
#define SDA(x) GPIO_WriteBit(IIC_SDA_PORT ,IIC_SDA_PIN ,x)




//---------------------------------------
void i2c_start()  
{  
    SCL(0);  
    DelayUs (2);  
    SDA(1);                            
    DelayUs (2);    
    SCL(1);        
    DelayUs (2);                                                                         
    SDA(0);        
    DelayUs (2);                                                                          
    SCL(0);                                                                                  
}  
//---------------------------------------  
void i2c_stop()  
{  
    SCL(0); 
    SDA(0);  
    DelayUs (2);   
    SCL(1);  
    DelayUs (2);   
    SDA(1);    
    DelayUs (2);   
    SCL(0);
    DelayUs (2);  
    SDA(0);
                      
}     
//----------------------------------------
int get_ack()  
{  
    uint16_t timeout=0;
    GPIO_QuickInit (IIC_SDA_PORT ,IIC_SDA_PIN ,kGPIO_Mode_IPU );
    SCL(0);
    DelayUs (2);
    while((GPIO_ReadBit (IIC_SDA_PORT ,IIC_SDA_PIN ))==1)
    {
        timeout++;
        if(timeout>400)
            return 1;
    }
    GPIO_QuickInit (IIC_SDA_PORT ,IIC_SDA_PIN ,kGPIO_Mode_OPP);
    GPIO_ResetBit (IIC_SDA_PORT ,IIC_SDA_PIN );
    DelayUs (2);
    SCL(1);
    DelayUs (2);
    SCL(0);
    return 0;           /**工作正常**/
}  
//----------------------------------------  
int i2c_send_byte(uint8_t byte)  
{  
  
    uint8_t i,r;  
    for(i=0;i<8;i++)  
        {  
            SCL(0);  
            DelayUs (2);   
            SDA((((byte&0x80)==0x80)?1:0));  
            DelayUs (2);   
            SCL(1);    
            DelayUs (2);   
            SCL(0);  
            DelayUs (2);   
            byte=byte<<1;  
        } 
        
    r=get_ack ();        
    if(r)   
        return 1;   
    else 
        return 0;
}  



/**********IO模拟IIC写数据**************/
int i2c_write_byte(uint8_t dev_addr_wr,uint8_t word_addr,uint8_t mydata)  
{  
    int r=0;
    i2c_start();  
    r+=i2c_send_byte(dev_addr_wr);  
    r+=i2c_send_byte(word_addr);  
    r+=i2c_send_byte(mydata);  
    i2c_stop();
    if(r)
        return 1;
    else
        return 0;
}  
/********** IO模拟IIC读数据**************/ 
uint8_t  i2c_read_byte(uint8_t dev_addr_wr,uint8_t word_addr)  
{  
    uint8_t temp,i;  
    temp=0;  
    //---------------  
    i2c_start();  
    i2c_send_byte(dev_addr_wr);  
    i2c_send_byte(word_addr);  
    i2c_stop();                        //inital the address of device and word  
    //-------------------------------------------------------------------------  
    i2c_start();  
    i2c_send_byte(dev_addr_wr+1);  
    for(i=0;i<8;i++)  
    {  
        SCL(0);  
        DelayUs (2);  
        GPIO_QuickInit (IIC_SDA_PORT ,IIC_SDA_PIN ,kGPIO_Mode_IPU );
        DelayUs (2);  
        SCL(1);  
        DelayUs (2);   
        if((GPIO_ReadBit (IIC_SDA_PORT ,IIC_SDA_PIN ))==1)
        {  
           temp+=1; 
        }
        DelayUs (2);   
        SCL(0);  
        DelayUs (2);  
        temp=temp<<1;  
    }  
    GPIO_QuickInit (IIC_SDA_PORT ,IIC_SDA_PIN ,kGPIO_Mode_OPP);
    GPIO_SetBit(IIC_SDA_PORT ,IIC_SDA_PIN );
    SCL(0); 
    DelayUs (2); 
    SCL(1);  
    DelayUs (2); 
    SCL(0);                      //read data from device  
     //-----------------------------------------------------------------------    
    i2c_stop();                 //stop the device  
    return temp;   
}  
//-------------------------------------------- 
