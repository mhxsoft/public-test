/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-03     HASEE       the first version
 */
#include    "includes.h"

const unsigned int YEAR_Arr[5]=//days of each 4 years
{   0,
    366, //366
    731, //366+365
    1096,//366+365+365
    1461 //366+365+365+365
};
const unsigned int MONTH_Arr[13]=//days of a year at month
{   0,
    31,//+31
    59,//+28<>
    90,//+31
    120,//+30
    151,//+31
    181,//+30
    212,//+31
    243,//+31
    273,//+30
    304,//+31
    334,//+30
    365 //+31
};
const unsigned int MONTH_Arr1[13]=//days of a lead year at month
{   0,
    31, //+31
    60, //+29<>
    91, //+31
    121,//+30
    152,//+31
    182,//+30
    213,//+31
    244,//+31
    274,//+30
    305,//+31
    335,//+30
    366 //+31
};
DATETIME CurrentDateTime;
//-------------------------------------------------------------------------------------------
/*******************************************************************************
* Function Name  : GetDays_Year
* Description    : Get days for years.
* Input          : yys=[00,99]
* Output         : dddddddd
* Return         : 返回整年是多少天数
*******************************************************************************/
unsigned long GetDays_Year(unsigned int yys)
{
    return( (yys/4)*1461+YEAR_Arr[yys%4] );
}
/*******************************************************************************
* Function Name  : GetDays_Month
* Description    : Get days for Months.
* Input          : ymms=[01,12]
* Output         : dddddddd
* Return         : 返回每月的第一天是一年内的第几天
*******************************************************************************/
unsigned long GetDays_Month(unsigned int ymms,unsigned int leap)
{   if(leap==0)
    {   return(MONTH_Arr[ymms-1]);
    }else //if(leap==1)
    {   return(MONTH_Arr1[ymms-1]);
    }
}
//-------------------------------------------------------------------------------------------
/*******************************************************************************
* Function Name  : GetYear_Days
* Description    : Get yy for days.
* Input          : days>0输入总天数
* Output         : yy=[00-99]
* Return         : 得到是第几年
*******************************************************************************/
unsigned int GetYear_Days(unsigned long days)
{   unsigned int i,dds;
    dds=days%YEAR_Arr[4];
    for(i=0;i<4;i++){if(dds<YEAR_Arr[i])break;}
    return((days/YEAR_Arr[4])*4+i-1);
}
/*******************************************************************************
* Function Name  : GetMonth_Days
* Description    : Get mm for days.
* Input          : days>0输入总天数
* Output         : mm=[01-12]
* Return         : 得到年内第几月
*******************************************************************************/
unsigned int GetMonth_Days(unsigned long days)
{   unsigned int i,dds,leap,ydds;
    dds=days%YEAR_Arr[4];
    for(i=0;i<4;i++){if(dds<YEAR_Arr[i])break;}
    if(i==1)leap=1;//闰年
    else leap=0;
    ydds=dds-YEAR_Arr[i-1];

    if(leap==0)
    {   for(i=0;i<12;i++){if(ydds<MONTH_Arr[i])break;}
    }else //if(leap==1)//闰年
    {   for(i=0;i<12;i++){if(ydds<MONTH_Arr1[i])break;}
    }
    return(i);
}
/*******************************************************************************
* Function Name  : GetDay_Days
* Description    : Get dd for days.
* Input          : days>0输入总天数
* Output         : dd=[01-28],[01-29],[01-30],[01-31]
* Return         : 得到月内第几天
*******************************************************************************/
unsigned int GetDay_Days(unsigned long days)
{   unsigned int i,dds,mdds,ydds,leap;
    dds=days%YEAR_Arr[4];
    for(i=0;i<4;i++){if(dds<YEAR_Arr[i])break;}
    if(i==1)leap=1;//闰年
    else leap=0;
    ydds=dds-YEAR_Arr[i-1];

    if(leap==0)
    {   for(i=0;i<12;i++){if(ydds<MONTH_Arr[i])break;}
        mdds=ydds-MONTH_Arr[i-1];
    }else //if(leap==1)//闰年
    {   for(i=0;i<12;i++){if(ydds<MONTH_Arr1[i])break;}
        mdds=ydds-MONTH_Arr1[i-1];
    }
    return(mdds+1);
}
//***************************************************************************
void Read_RTC_to_CurrentDateTime(void)
{
    unsigned long tmp32,yymmdd,hhmmss;

    tmp32=RTC_GetCounter();
    yymmdd=tmp32/(3600*24);
    hhmmss=tmp32%(3600*24);
    CurrentDateTime.Minute=Bin8toBCD3((hhmmss%3600)/60);//00-59
    CurrentDateTime.Hour=Bin8toBCD3(hhmmss/3600);//00-23
    //CurrentDateTime.Weekday=Bin8toBCD3((yyyymmdd+WeekdayBase)%7);//1-7
    CurrentDateTime.Day=Bin8toBCD3(GetDay_Days(yymmdd));//01-28/29,01-30,01-31
    CurrentDateTime.Month=Bin8toBCD3(GetMonth_Days(yymmdd));//01-12
    CurrentDateTime.Year=Bin8toBCD3(GetYear_Days(yymmdd));//00-99
}
void Write_CurrentDateTime_to_RTC(void)
{
    unsigned long tmp32,mm,yymmdd,hhmmss;
    unsigned int leap;

    tmp32=CurrentDateTime.Hour>>4;
    tmp32*=10;
    tmp32+=CurrentDateTime.Hour&0x0f;
    if(tmp32>=24)tmp32=23;
    hhmmss=tmp32*3600;

    tmp32=CurrentDateTime.Minute>>4;
    tmp32*=10;
    tmp32+=CurrentDateTime.Minute&0x0f;
    if(tmp32>=60)tmp32=59;
    hhmmss+=tmp32*60;

    tmp32=CurrentDateTime.Year>>4;
    tmp32*=10;
    tmp32+=CurrentDateTime.Year&0x0f;
    if((tmp32%4)==0)leap=1;else leap=0;
    yymmdd=GetDays_Year(tmp32);

    tmp32=CurrentDateTime.Month>>4;
    tmp32*=10;
    tmp32+=CurrentDateTime.Month&0x0f;
    if(tmp32==0)tmp32=1;
    else if(tmp32>=13)tmp32=12;
    mm=tmp32;
    yymmdd+=GetDays_Month(tmp32,leap);

    tmp32=CurrentDateTime.Day>>4;
    tmp32*=10;
    tmp32+=CurrentDateTime.Day&0x0f;
    switch(mm)
    {   case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if(tmp32==0)tmp32=1;
            else if(tmp32>=32)tmp32=31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if(tmp32==0)tmp32=1;
            else if(tmp32>=31)tmp32=30;
            break;
        case 2:
            if(tmp32==0)tmp32=1;
            else
            {   if(leap==1){ if(tmp32>=30)tmp32=29;}
                else { if(tmp32>=29)tmp32=28;}
            }
            break;
    }
    yymmdd+=tmp32-1;

    tmp32=yymmdd*24*3600+hhmmss;
    //1.5S*2=3S waiting......
    PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_SetCounter(tmp32);
    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);
}
*/

