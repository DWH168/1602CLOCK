#include<reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit RS=P2^6;
sbit RW=P2^5;
sbit E=P2^7;
sbit K1=P1^0;
sbit K2=P1^1;
sbit K3=P1^2;
sbit K4=P1^3;
sbit K5=P1^4;
sbit BEEP=P3^0;
unsigned char cishu=0;
unsigned char mode_flag;//状态标志位 0表示正处于正常，1设置时间，2设置闹钟
unsigned char Current_Time[]={"Current Time  "};	   //正常运行模式
unsigned char Set_time[]={"Set New Time..."};		   //设置时间模式
unsigned char Set_nao[]={"Set nao Time..."};		   //设置闹钟模式
unsigned char show_time[]={">00:00:00"};
unsigned char now_shi=12,now_fen=30,now_miao;//运行时的时间变量
unsigned char temp_shi,temp_fen,temp_miao;//调时间的时间变量
unsigned char nao_shi=13,nao_fen=30,nao_miao;//闹钟的时间变量
unsigned char flag_sf; //0表示设置时，1表示设置分
//歌曲音调编码
uchar code SONG_TONE[]={212,212,190,212,159,169,212,212,190,212,142,159,212,212,106,126,159,169,190,119,119,126,159,142,159,0};
//歌曲长音节
uchar code SONG_LONG[]={9,3,12,12,12,24,9,3,12,12,12,24,9,3,12,12,12,12,12,9,3,12,12,12,24,0};

	
void display_mode();
void display_time(unsigned char shi,unsigned char fen,unsigned char miao);	
void keyscan();
void PlayMusic();
void lcd_w_cmd(unsigned char com);
void lcd_w_dat(unsigned char dat);
void lcd_int ();
void delay1(unsigned int i);
unsigned char lcd_r_start();
void been(unsigned int time);
void main()
{			 
	lcd_int ();	   //初始化lcd
	mode_flag=0;//运行
	display_mode();	  //调用显示模式函数
	TMOD=0x01;		//初始化定时器
	EA=1;			 //打开总中断
	TH0=(65536-50000)/256;			//赋初值
	TL0=(65536-50000)%256;
	ET0=1;						   //打开定时器T0中断标志位
	TR0=1;							//启动T0
	while(1){			
			if(mode_flag==0)display_time(now_shi,now_fen,now_miao);		  //状态标志位mode_flag=0表示正处于时间正常运行
			else display_time(temp_shi,temp_fen,temp_miao);			  //否则mode_flag为1时设置时间，为2时设置闹钟
			keyscan();			   //调用按键开关控制函数
			if(now_shi==nao_shi&&now_fen==nao_fen&&now_miao==nao_miao) PlayMusic();	  //如果正在运行的时间为闹钟的时间，则闹铃响（此为音乐）
	}	
}
void display_mode()//第一行模式的显示，有三种模式：（1）正在运行的显示Current_Time[]字符串（2）设置时间的显示Set_time[]字符串	（3）设置闹钟的显示Set_nao[]字符串
{
	 
	unsigned char i;
	lcd_w_cmd(0x82);	   //设置显示地址（第一行）
	if(mode_flag==0)		  //状态标志位mode_flag=0表示正处于时间正常运行
	{
		for(i=0;Current_Time[i]!='\0';i++)		//此处显示Current_Time[]字符串，字符串结束符为‘0’
		{
			lcd_w_dat(Current_Time[i]);
			
		}	
	}
	else if(mode_flag==1)
	{	
		for(i=0;Set_time[i]!='\0';i++)		//此处显示Set_time[]字符串，字符串结束符为‘0’
		{
			lcd_w_dat(Set_time[i]);
		
		}
	}
	else if(mode_flag==2)
	{
		 for(i=0;Set_nao[i]!='\0';i++)		//此处显示Set_nao[]字符串，字符串结束符为‘0’
		{
			lcd_w_dat(Set_nao[i]);		
		}
	}
}
void display_time(unsigned char shi,unsigned char fen,unsigned char miao)//第二行的数字显示
{
	unsigned char i;
	if(mode_flag==0)show_time[0]=' ';	 //如果mode_flag=0，第二行第0位显示' '
	else  show_time[0]='>';				 //否则第二行第0位显示'>'

	show_time[1]=shi/10+0x30;			 //第一位显示时的十位数
	show_time[2]=shi%10+0x30;			//第二位显示时的个位数

	show_time[4]=fen/10+'0';			 //第四位显示分的十位数
	show_time[5]=fen%10+'0';			 //第五位显示分的个位数

	show_time[7]=miao/10+'0';			  //第七位显示秒的十位数
	show_time[8]=miao%10+'0';			  //第八位显示秒的个位数

	lcd_w_cmd(0xC3);  //更改显示位置用
	for(i=0;show_time[i]!='\0';i++)		//此处显示show_time[]字符串，字符串结束符为‘0’
	{
		lcd_w_dat(show_time[i]);
	
	}
}
void keyscan()					  //按键控制，k1调时或分，k2加时或分，k3减时或分，k4确定调好的时间，k5调闹钟
{
	if(K1==0)
	{
		delay1(1000);
		if(K1==0)
		{
			if(mode_flag==0)			 //正在运行的时间要进入调时间的状态
			{
				been(300);
			   	temp_shi=now_shi;		
				temp_fen=now_fen;
				temp_miao=now_miao;
				mode_flag=1;
			}
			else{ 
					been(300);
				 	flag_sf=~flag_sf; //切换状态，flag_sf=0调时钟 flag_sf=1调分钟
				 }
				display_mode();
			while(!K1);	//松手检测
		}
	}
	if(K2==0)						  //加时或分
	{
		delay1(1000);
		if(K2==0)
		{
			if(mode_flag!=0)			
			{ 
				 been(300);
			   if(flag_sf==0)			//调时钟状态
			   {
			   	  temp_shi++;
				  if(temp_shi==24)temp_shi=0;		 //如果加时钟到24就清零
			   }
			   else {							   //调分钟状态
			   	  temp_fen++;
				  if(temp_fen==60)temp_fen=0;		  //如果加分钟到60就清零
			   }			  	
			}
			while(!K2);	//松手检测
		}	
	}	
	if(K3==0)						   //减时或分
	{
		delay1(1000);
		if(K3==0)
		{
			if(mode_flag!=0)
			{
				been(300);
			   if(flag_sf==0)			   //调时钟状态
			   {
			   	
				if(temp_shi==0)temp_shi=24;		//如果减时钟减到最低到0就又回到最高位24
			   	  temp_shi--;
				  
			   }
			   else {						 //调分钟状态

			   	if(temp_fen==0)temp_fen=60;			//如果减分钟减到最低到0就又回到最高位60
			   	  temp_fen--; 
			   }
			  	
			}
		  
			while(!K3);	//松手检测
		}
	
	}
										 //确定设置的时间和调好闹钟的时间
	if(K4==0)
	{
		delay1(1000);
		if(K4==0)
		{
			if(mode_flag==1)				  //设置时间  设置好的时间确定后正常运行
			{
				been(300);
				now_shi=temp_shi;			   
				now_fen=temp_fen;
				now_miao=temp_miao;
				mode_flag=0;
			}
			if(mode_flag==2)					 //设置闹钟	设置好的时间确定好之后定为闹钟
			{									 
				been(300);
				nao_shi=temp_shi;
				nao_fen=temp_fen;
				nao_miao=temp_miao;
				mode_flag=0;
			}
			while(!K4);	//松手检测
		}
		display_mode();			   //调用显示模式函数
	
	}

	if(K5==0)					   //设置闹钟
	{
		delay1(1000);
		if(K5==0)
		{
			if(mode_flag==0)
			{
			   been(300);
			   mode_flag=2;
			   temp_shi=nao_shi;
			   temp_fen=nao_fen;
			   temp_miao=nao_miao;
			}		  
			display_mode();
			while(!K5);	//松手检测
		}
	
	}
}
void been(unsigned int time)
{
	unsigned int i;
	for(i=0;i<time;i++)
	{
		 BEEP=~BEEP;		  
		 delay1(10);	
	}
}
void delay(uint ms)	   //闹铃的延时函数
{
 char t;
 while(ms--) for(t=0;t<120;t++);
}

void PlayMusic()
{
uint i=0,j,k;
while (SONG_LONG[i]!=0||SONG_TONE[i]!=0)
 {
  for(j=0;j<SONG_LONG[i]*30;j++)
  {
   BEEP=~BEEP;
   for(k=0;k<SONG_TONE[i]/4;k++);
  }
  delay(10);
  i++;
  display_time(now_shi,now_fen,now_miao);	
 }
}
//函数名：time0
//函数功能：定时器T0的中断函数，T0在工作方式1下每50ms产生中断，执行该中断函数
void time0() interrupt 1		 
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;

	cishu++;
	if(cishu==20)			   //一秒时间到后，次数清零，秒数加一
	{
		cishu=0;
		now_miao++;
	}
	if(now_miao==60)			//60秒时间到后，秒数清零，分钟加一
	{
	 	now_miao=0;
		now_fen++;
	}

	if(now_fen==60)				//60分钟时间到后，分钟清零，时钟加一
	{
		
	  now_fen=0;
	  now_shi++;
	}

	if(now_shi==24)				 //24小时到后，时钟清零
	{
	   now_shi=0;
	}
}

//函数名：lcd_int
//函数功能：lcd初始化
void lcd_int()
{
	lcd_w_cmd(0x3c);	//设置工作方式
	lcd_w_cmd(0x0c);	  //设置显示状态
	lcd_w_cmd(0x01);	 //清屏
	lcd_w_cmd(0x06);	 //设置输入方式
	lcd_w_cmd(0x80);	 //设置初始显示位置
}
//函数名：lcd_w_cmd
//函数功能：写命令
//形式参数：命令字已经存入com单元中
void lcd_w_cmd(unsigned char com)
{
	unsigned char i;
	do
	{						 //查lcd忙状态
	i=lcd_r_start();		  //调用读状态字函数
	i&=0x80;				   //“与”操作屏蔽掉低7位
	delay1(2);
	}while(i!=0);				//lcd忙，继续查询，否则退出循环 
	RW=0;
	delay1(1);
	RS=0;						 //RW=0，RS=0，写lcd命令字
	delay1(1);
	E=1;						  //E端时序以0或1高低波动
	delay1(1);
	P0=com;						  //将com中的命令字写入lcd数据口
	delay1(1);
	E=0;
	delay1(1);
	RW=1;
	delay1(2);
}
//函数名：lcd_w_dat
//函数功能：写数据
void lcd_w_dat(unsigned char dat)
{
	unsigned char i;
	do
	{						   //查忙操作
	i=lcd_r_start();		   //调用读状态字函数
	i&=0x80;				   //“与”操作屏蔽掉低7位
	delay1(2);
	}while(i!=0);			   //lcd忙，继续查询，否则退出循环 
	RW=0;
	delay1(1);
	RS=1;						 //RW=0，RS=1，写lcd命令字
	delay1(1);
	E=1;						  //E端时序以0或1高低波动
	delay1(1);
	P0=dat;						   //将dat中的显示数据写入lcd数据口
	delay1(1);
	E=0;
	delay1(1);
	RW=1;
	delay1(2);
}
void delay1(unsigned int i)
{
	unsigned int k;
	for(k=0;k<i;k++);
}
//函数名：lcd_r_start
//函数功能：读状态字
unsigned char lcd_r_start()
{
	unsigned char s;
	RW=1;			  //RW=1,RS=0,读lcd状态
	delay1(1);
	RS=0;
	delay1(1);
	E=1;			   //E端时序以0或1高低波动
	delay1(1);
	s=P0;				 //从lcd的数据口读状态  
	delay1(1);
	E=0;
	delay1(1);
	RW=0;
	delay1(1);
	return(s);			 //返回值s读取状态忙或空闲
}




