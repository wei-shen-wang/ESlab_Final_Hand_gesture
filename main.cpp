


#include "mbed.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

InterruptIn button(BUTTON1);
int button_switch = -1;
DigitalOut led1(LED1);//v
DigitalOut led2(LED2);//v
Thread T1;
Timer t;

static BufferedSerial serial_port(USBTX, USBRX);
FileHandle *mbed::mbed_override_console(int fd){
    return &serial_port; 
}
void car_motion(){
    BSP_ACCELERO_Init();
    uint32_t i = 0;
    uint32_t index = 0;
    int limit = 300;
    const int amount = 30;
    
    /* Initialize input and output buffer pointers */
    int16_t Acc[3] = {0};
    float sumax = 0,sumay = 0,sumaz = 0;
    float tempx[amount]={0};
    float tempy[amount]={0};
    float tempz[amount]={0};
    int state = 0;
    int prestate = 0;
    printf("Start sensing\n");
    while(true){
        if(button_switch==1){
            break;
        }
        sumax = 0;
        sumay = 0;
        sumaz = 0;
        BSP_ACCELERO_AccGetXYZ(Acc);
        int temp;
        temp = index % amount;
        tempx[temp] = (float)Acc[0];
        tempy[temp] = (float)Acc[1];
        tempz[temp] = (float)Acc[2];
        for(int i=0;i<amount;i++){
            sumax += tempx[i];
            sumay += tempy[i];
            sumaz += tempz[i];
        }
        sumax = sumax/amount;
        sumay = sumay/amount;
        sumaz = sumaz/amount;
        if(sumax<-limit && sumax<sumay){
            state = 1;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("car right\n");
        }
        else if(sumax>limit && sumax>sumay){
            state = 2;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("car left\n");
        }
        else if(sumay<-limit && sumax>=sumay){
            state = 3;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("car forward\n");
        }
        else if(sumay>limit && sumax<=sumay){
            state = 4;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("car backward\n");
        }
        else if(sumaz<0){
            printf("upsidedown\n");
            state = 0;
        }
        else{
            printf("stable or unknown\n");
            state = 0;
        }         
        ThisThread::sleep_for(10);
        prestate = state;
        index++;
    }
};
void camera_motion(){
    BSP_ACCELERO_Init();
    uint32_t i = 0;
    uint32_t index = 0;
    int limit = 300;
    const int amount = 30;
    
    /* Initialize input and output buffer pointers */
    int16_t Acc[3] = {0};
    float sumax = 0,sumay = 0,sumaz = 0;
    float tempx[amount]={0};
    float tempy[amount]={0};
    float tempz[amount]={0};
    int state = 0;
    int prestate = 0;
    printf("Start sensing\n");
    while(true){
        if(button_switch==-1){
            break;
        }
        sumax = 0;
        sumay = 0;
        sumaz = 0;
        BSP_ACCELERO_AccGetXYZ(Acc);
        int temp;
        temp = index % amount;
        tempx[temp] = (float)Acc[0];
        tempy[temp] = (float)Acc[1];
        tempz[temp] = (float)Acc[2];
        for(int i=0;i<amount;i++){
            sumax += tempx[i];
            sumay += tempy[i];
            sumaz += tempz[i];
        }
        sumax = sumax/amount;
        sumay = sumay/amount;
        sumaz = sumaz/amount;
        if(sumax<-limit && sumax<sumay){
            state = 1;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("camera right\n");
        }
        else if(sumax>limit && sumax>sumay){
            state = 2;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("camera left\n");
        }
        else if(sumay<-limit && sumax>=sumay){
            state = 3;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("camera forward\n");
        }
        else if(sumay>limit && sumax<=sumay){
            state = 4;
            if(state != prestate){
                ThisThread::sleep_for(1000);
            }
            printf("camera backward\n");
        }
        else if(sumaz<0){
            printf("upsidedown\n");
            state = 0;
        }
        else{
            printf("stable or unknown\n");
            state = 0;
        }         
        ThisThread::sleep_for(10);
        prestate = state;
        index++;
    }
};
void led_thread(){
    while(true){
        while(button_switch==1){
            led1 = !led1;
            ThisThread::sleep_for(500);
        }
        while(button_switch==-1){
            led2 = !led2;
            ThisThread::sleep_for(500);
        }
        led1 = 0;
        led2 = 0;
    }
}

void button_pressed()
{
    
}

void button_released()
{
    button_switch = -button_switch;
}
int main()
{
    led1 = 0;
    led2 = 0;
    button.fall(&button_pressed); // Change led delay
    button.rise(&button_released);
    T1.start(callback(led_thread));
    while(true){
        if(button_switch==-1){
            car_motion();
        }
        if(button_switch==1){
            camera_motion();
        }
    }

}