/**
 * ============================================================
 *  中药分拣系统 V9.6
 *  90°=开  180°=闭
 *  串口: @数字=目标 @S=启动 @R=急停 @?=查询 @Dxxxx=延时 @Txxxx=超时 @Exxxx=提前关门量
 * ============================================================
 */
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "timer.h"
#include "oled.h"
#include "servo.h"
#include "sensor.h"
#include "Serial.h"

enum{IDLE,FEED,DROP,VERIFY,BLINK,DONE};
static uint8_t  st=IDLE, run=0, bad=0, blk=0, od=1, cd=0;
static uint16_t dm=2000, cm=5000, tg=0;
static uint8_t  lead=1;   /* 提前关门量: 上光电数到 TGT-lead 即关上闸, 补偿闸门机械关闭延迟 */
static uint32_t t0=0, to=0, tl=0;

static int ai(const char*s){int r=0;while(*s>='0'&&*s<='9'){r=r*10+*s-'0';s++;}return r;}

static void parse(void){
    uint8_t i,d;uint16_t v;char*p=Serial_RxPacket;
    if(p[0]=='@')p++;if(!p[0])return;
    switch(p[0]){
    case'?':od=1;Serial_Printf("TGT:%d DM:%d TO:%d UP:%d LO:%d ST:%d LEAD:%d TH:%d\r\n",tg,dm,cm,Sensor_Up,Sensor_Lo,st,lead,(tg>lead)?(tg-lead):1);return;
    case'D':case'd':v=(uint16_t)ai(&p[1]);if(v>=100&&v<=10000){dm=v;od=1;Serial_Printf("D=%d\r\n",v);}else Serial_Printf("ERR\r\n");return;
    case'T':case't':v=(uint16_t)ai(&p[1]);if(v>=500&&v<=60000){cm=v;od=1;Serial_Printf("T=%d\r\n",v);}else Serial_Printf("ERR\r\n");return;
    case'E':case'e':v=(uint16_t)ai(&p[1]);if(v<=30){lead=(uint8_t)v;od=1;Serial_Printf("E=%d TH=%d\r\n",v,(tg>lead)?(tg-lead):1);}else Serial_Printf("ERR\r\n");return;
    case'S':case's':if(!tg)Serial_Printf("TGT=0\r\n");else if(st!=IDLE)Serial_Printf("BUSY\r\n");else run=1;return;
    case'R':case'r':Servo_Up(GATE_CLOSE_ANGLE);Servo_Lo(GATE_CLOSE_ANGLE);Sensor_Up=0;Sensor_Lo=0;tg=0;run=0;st=IDLE;bad=0;blk=0;t0=0;to=0;tl=0;od=1;Serial_Printf("[STOP]\r\n");return;
    }d=1;for(i=0;p[i];i++)if(p[i]<'0'||p[i]>'9'){d=0;break;}
    if(d){v=(uint16_t)ai(p);if(v){tg=v;Serial_Printf("TGT=%d\r\n",v);}}else Serial_Printf("?:%s\r\n",p);
}

static void OLED_Show(void){
    char l[17];OLED_Clear();
    switch(st){case IDLE:snprintf(l,17,"IDLE     TGT:%3d",tg);break;case FEED:snprintf(l,17,"FEEDING  TGT:%3d",tg);break;case DROP:snprintf(l,17,"WAIT     TGT:%3d",tg);break;case VERIFY:snprintf(l,17,"VERIFY   TGT:%3d",tg);break;default:snprintf(l,17,"DONE!    TGT:%3d",tg);break;}OLED_ShowString(1,1,l);
    snprintf(l,17,st==FEED?"Up:OPEN  UP:%3d":"Up:CLOSE UP:%3d",Sensor_Up);OLED_ShowString(2,1,l);
    snprintf(l,17,st==VERIFY?"Lo:OPEN  LO:%3d":"Lo:CLOSE LO:%3d",Sensor_Lo);OLED_ShowString(3,1,l);
    switch(st){case IDLE:snprintf(l,17,tg?"TGT:%d 'S' 2go":"D:%d T:%d",tg?tg:dm,tg?0:cm);if(!tg)snprintf(l,17,"D:%d T:%d",dm,cm);break;case FEED:snprintf(l,17,"Feed UP:%3d/%3d",Sensor_Up,tg);break;case DROP:snprintf(l,17,"Wait %dms...",dm);break;case VERIFY:snprintf(l,17,"Ver  LO:%3d/%3d",Sensor_Lo,Sensor_Up);break;case BLINK:snprintf(l,17,(blk&1)?"** Complete! **":"                ");break;case DONE:snprintf(l,17,bad?"MISMATCH!":"** Complete! **");break;}OLED_ShowString(4,1,l);
}

int main(void){
    uint32_t now;
    RCC_DeInit();SystemCoreClock=8000000;
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);SysTick_Config(8000);

    Serial_Init();
    Servo_Init();Servo_Up(GATE_CLOSE_ANGLE);Servo_Lo(GATE_CLOSE_ANGLE);
    Sensor_Init();
    OLED_Init();OLED_Clear();OLED_ShowString(1,1,"Medicine V9.5");
    Servo_Up(GATE_MID_ANGLE);Servo_Lo(GATE_MID_ANGLE);Delay_ms(150);Servo_Up(GATE_CLOSE_ANGLE);Servo_Lo(GATE_CLOSE_ANGLE);
    OLED_Show();
    Serial_Printf("\r\n===== V9.5 =====\r\n");

    while(1){
        now=g_sys_tick;
        {static uint16_t pu=0,pd=0;if(Sensor_Up!=pu){pu=Sensor_Up;tl=now;cd=1;}if(Sensor_Lo!=pd){pd=Sensor_Lo;cd=1;}}
        if(Serial_RxFlag){Serial_RxFlag=0;parse();}
        switch(st){
        case IDLE: if(run&&tg){run=0;Sensor_Up=0;Sensor_Lo=0;bad=0;Serial_Printf("[S] TGT=%d\r\n",tg);Servo_Up(GATE_OPEN_ANGLE);Servo_Lo(GATE_CLOSE_ANGLE);to=now;tl=now;st=FEED;od=1;}break;
        case FEED: if(Sensor_Up>=((tg>lead)?(tg-lead):1)){Servo_Up(GATE_CLOSE_ANGLE);Serial_Printf("[OK] UP=%d L=%d\r\n",Sensor_Up,lead);t0=now;st=DROP;od=1;}
            else if((now-tl)>cm){Servo_Up(GATE_CLOSE_ANGLE);Serial_Printf("[TO] UP=%d/%d\r\n",Sensor_Up,tg);t0=now;st=DROP;od=1;}break;
        case DROP: if((now-t0)>=dm){Servo_Lo(GATE_OPEN_ANGLE);to=now;st=VERIFY;Serial_Printf("[V]\r\n");od=1;}break;
        case VERIFY: if(Sensor_Lo>=Sensor_Up){Servo_Lo(GATE_CLOSE_ANGLE);Serial_Printf("[OK] LO=%d\r\n",Sensor_Lo);if(Sensor_Lo!=Sensor_Up)bad=1;blk=0;t0=now;st=BLINK;od=1;}
            else if((now-to)>cm){Servo_Lo(GATE_CLOSE_ANGLE);Serial_Printf("[TO] LO=%d/%d\r\n",Sensor_Lo,Sensor_Up);if(Sensor_Lo!=Sensor_Up)bad=1;blk=0;t0=now;st=BLINK;od=1;}break;
        case BLINK: if((now-t0)>=400){t0=now;if(++blk>=6)st=DONE;od=1;}break;
        case DONE: Serial_Printf("=== DONE TGT:%d UP:%d LO:%d ===\r\n",tg,Sensor_Up,Sensor_Lo);Sensor_Up=0;Sensor_Lo=0;tg=0;bad=0;st=IDLE;break;
        }
        if(od||cd){od=0;cd=0;OLED_Show();}
    }
}
