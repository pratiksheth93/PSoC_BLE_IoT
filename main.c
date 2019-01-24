#include <project.h>
#include <stdio.h>

CYBLE_CONN_HANDLE_T connectionHandle;
CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
uint8 DevType,DevCode,DevParam,Reserve,s,t,y,z;
char buffer[20];
char buffer1[20];

void HandleDevice(){
if (DevType==0x01)
    {
        switch(DevCode)
        {
            case 0x01:
            ADC_StartConvert();
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            s = ADC_GetResult16(0);
            
            sprintf(buffer,"LDR = %d\n\r",s);
            UART_PutString(buffer);
            
            /*y = ADC_CountsTo_Volts(0,s);
            sprintf(buffer,"LDR = %d\n\r",y);
            UART_PutString(buffer);
            CyDelay(1000);*/
            
            break;
            
            case 0x02:
            ADC_StartConvert();
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            t=ADC_GetResult16(1);
            
            sprintf(buffer1,"Temperature = %d\n\r",t);
            UART_PutString(buffer1);
            
            /*z = ADC_CountsTo_mVolts(1,t);
            sprintf(buffer,"Temperature = %d\n\r",z);
            UART_PutString(buffer1);*/
            
            CyDelay(1000);
            break;
            
            case 0x03:
            UART_PutChar('X');
            break;
            
            case 0x04:
            UART_PutChar('Y');
            break;
            
            case 0x05:
            UART_PutChar('Z');
            
            break;
            
            default:
        
            break;
    }
    }
}
    
        
  


void CustomEventHandler(uint32 event, void * eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:   //event called when BLE stack starts
       
       CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);  //Start advertising
        break;
        
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED: CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        
        case CYBLE_EVT_GATT_CONNECT_IND:
        connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
        break;
        
        case CYBLE_EVT_GATTS_WRITE_REQ:    //writing request
        wrReqParam=(CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
        if(CYBLE_HOME_CONTROL_DEVICE_CONTROL_CHAR_HANDLE== wrReqParam->handleValPair.attrHandle)
        {
            DevType = (uint8)wrReqParam->handleValPair.value.val[0];
            DevCode = (uint8)wrReqParam->handleValPair.value.val[1];
            DevParam = (uint8)wrReqParam->handleValPair.value.val[2];
            Reserve = (uint8)wrReqParam->handleValPair.value.val[3];
            HandleDevice();
            CyBle_GattsWriteRsp(connectionHandle);
        }
        
        break;
        
        default: break;
        
        
        
    }
    
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    ADC_Start();
    UART_Start();
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBle_Start(CustomEventHandler);
    
    for(;;)
    {
        CyBle_ProcessEvents();
        /* Place your application code here. */
        
    }
}

/* [] END OF FILE */

Code in mbed :
#include "mbed.h"
#include "stdio.h"
#include "MMA8451Q.h"
#define MMA8451_I2C_ADDRESS (0x1d<<1)

MMA8451Q acc(PTE25, PTE24, MMA8451_I2C_ADDRESS);


Serial pc(PTE22,PTE23);
Serial pc1(USBTX,USBRX);//USBTX,USBRX
int main()
{   
    int16_t x1=0,z1=0;
    int16_t y1=0;
    int s=10;
    
    char choice,lux,temp;
    pc1.printf("enter \n\r 1-ldr \n\r 2-lm35 \n\r 3-x \n\r 4-y \n\r 5-z:\n\r");

    while(true)
    {        
        if(pc.readable())
        {
            choice=pc.getc();
            switch(choice)
            {
              case 'X':
                x1=-s*(acc.getAccY());     
                pc1.printf("x=%d\n\r",x1);
                break;
              case 'Y':
              y1=-s*(acc.getAccX());     
                pc1.printf("y=%d\n\r",y1);
              break;  
              case 'Z':
              z1=s*(acc.getAccZ());     
                pc1.printf("z=%d\n\r",z1);
              break;
              default:
                pc1.putc(choice);
                pc1.printf("\n\r");
                break;
            }
        }
    }
}

