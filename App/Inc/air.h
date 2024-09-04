#ifndef __AIR_H
#define __AIR_H

#include "main.h"

#define AIR_GET_LOC "config,get,lbsloc\r\n"   // 基站定位命令
#define AIR_GET_TIME "config,get,nettime\r\n" // 获取时间命令

// 位置上传阿里云命令
// #define AIR_SEND_LOC "{\"id\":1715275393331,\"params\":{\"GeoLocation\":{\"Longitude\":%lf,\"Latitude\":%lf,\"Altitude\":10,\"CoordinateSystem\":1}},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"
#define AIR_SEND_LOC "{\"id\":1720255749424,\"params\":{\"GeoLocation\":{\"Longitude\":%lf,\"Latitude\":%lf,\"Altitude\":10,\"CoordinateSystem\":1}},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"

#define AIR_SEND_hr "{\"id\":1720255749424,\"params\":{\"HeartRate\":%d,\"SPO\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"
// #define AIR_SEND_hr  "{\"id\":1720250921402,\"params\":{\"HeartRate\":%d,\"SPO\":%lf},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"

// #define AIR_SEND_warn  "{\"id\":1715275393331,\"params\":{\"warnhr\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"
#define AIR_SEND_warn "{\"id\":1720255749424,\"params\":{\"warnhr\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"

struct airTime // 时间结构体
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t millisecond;
};

void air_get_location(void);                    // 得到定位
void air_data_trans(double *jing, double *wei); // 定位转换。经纬度
void air_send_location(void);                   // 上报定位数据
void air_send_hr(int dis_hr, int dis_spo2);     // 上报心率血氧数据
void air_send_warn(int air_warn);               // 报警上报，没用上
void air_trans_time(void);                      // 时间转换
void air_get_time(void);                        // 得到时间

uint8_t air_init(void);

extern double jing, wei; // 经纬度
// extern uint8_t loc[200];    // 存信息数组
extern uint8_t hr_warnflag; // 报警标志位
// extern uint8_t rx_buf[200]; // 接收缓存数组（用temp变量接收也一样）
extern struct airTime time;

#endif
