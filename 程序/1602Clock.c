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
unsigned char mode_flag;//״̬��־λ 0��ʾ������������1����ʱ�䣬2��������
unsigned char Current_Time[]={"Current Time  "};	   //��������ģʽ
unsigned char Set_time[]={"Set New Time..."};		   //����ʱ��ģʽ
unsigned char Set_nao[]={"Set nao Time..."};		   //��������ģʽ
unsigned char show_time[]={">00:00:00"};
unsigned char now_shi=12,now_fen=30,now_miao;//����ʱ��ʱ�����
unsigned char temp_shi,temp_fen,temp_miao;//��ʱ���ʱ�����
unsigned char nao_shi=13,nao_fen=30,nao_miao;//���ӵ�ʱ�����
unsigned char flag_sf; //0��ʾ����ʱ��1��ʾ���÷�
//������������
uchar code SONG_TONE[]={212,212,190,212,159,169,212,212,190,212,142,159,212,212,106,126,159,169,190,119,119,126,159,142,159,0};
//����������
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
	lcd_int ();	   //��ʼ��lcd
	mode_flag=0;//����
	display_mode();	  //������ʾģʽ����
	TMOD=0x01;		//��ʼ����ʱ��
	EA=1;			 //�����ж�
	TH0=(65536-50000)/256;			//����ֵ
	TL0=(65536-50000)%256;
	ET0=1;						   //�򿪶�ʱ��T0�жϱ�־λ
	TR0=1;							//����T0
	while(1){			
			if(mode_flag==0)display_time(now_shi,now_fen,now_miao);		  //״̬��־λmode_flag=0��ʾ������ʱ����������
			else display_time(temp_shi,temp_fen,temp_miao);			  //����mode_flagΪ1ʱ����ʱ�䣬Ϊ2ʱ��������
			keyscan();			   //���ð������ؿ��ƺ���
			if(now_shi==nao_shi&&now_fen==nao_fen&&now_miao==nao_miao) PlayMusic();	  //����������е�ʱ��Ϊ���ӵ�ʱ�䣬�������죨��Ϊ���֣�
	}	
}
void display_mode()//��һ��ģʽ����ʾ��������ģʽ����1���������е���ʾCurrent_Time[]�ַ�����2������ʱ�����ʾSet_time[]�ַ���	��3���������ӵ���ʾSet_nao[]�ַ���
{
	 
	unsigned char i;
	lcd_w_cmd(0x82);	   //������ʾ��ַ����һ�У�
	if(mode_flag==0)		  //״̬��־λmode_flag=0��ʾ������ʱ����������
	{
		for(i=0;Current_Time[i]!='\0';i++)		//�˴���ʾCurrent_Time[]�ַ������ַ���������Ϊ��0��
		{
			lcd_w_dat(Current_Time[i]);
			
		}	
	}
	else if(mode_flag==1)
	{	
		for(i=0;Set_time[i]!='\0';i++)		//�˴���ʾSet_time[]�ַ������ַ���������Ϊ��0��
		{
			lcd_w_dat(Set_time[i]);
		
		}
	}
	else if(mode_flag==2)
	{
		 for(i=0;Set_nao[i]!='\0';i++)		//�˴���ʾSet_nao[]�ַ������ַ���������Ϊ��0��
		{
			lcd_w_dat(Set_nao[i]);		
		}
	}
}
void display_time(unsigned char shi,unsigned char fen,unsigned char miao)//�ڶ��е�������ʾ
{
	unsigned char i;
	if(mode_flag==0)show_time[0]=' ';	 //���mode_flag=0���ڶ��е�0λ��ʾ' '
	else  show_time[0]='>';				 //����ڶ��е�0λ��ʾ'>'

	show_time[1]=shi/10+0x30;			 //��һλ��ʾʱ��ʮλ��
	show_time[2]=shi%10+0x30;			//�ڶ�λ��ʾʱ�ĸ�λ��

	show_time[4]=fen/10+'0';			 //����λ��ʾ�ֵ�ʮλ��
	show_time[5]=fen%10+'0';			 //����λ��ʾ�ֵĸ�λ��

	show_time[7]=miao/10+'0';			  //����λ��ʾ���ʮλ��
	show_time[8]=miao%10+'0';			  //�ڰ�λ��ʾ��ĸ�λ��

	lcd_w_cmd(0xC3);  //������ʾλ����
	for(i=0;show_time[i]!='\0';i++)		//�˴���ʾshow_time[]�ַ������ַ���������Ϊ��0��
	{
		lcd_w_dat(show_time[i]);
	
	}
}
void keyscan()					  //�������ƣ�k1��ʱ��֣�k2��ʱ��֣�k3��ʱ��֣�k4ȷ�����õ�ʱ�䣬k5������
{
	if(K1==0)
	{
		delay1(1000);
		if(K1==0)
		{
			if(mode_flag==0)			 //�������е�ʱ��Ҫ�����ʱ���״̬
			{
				been(300);
			   	temp_shi=now_shi;		
				temp_fen=now_fen;
				temp_miao=now_miao;
				mode_flag=1;
			}
			else{ 
					been(300);
				 	flag_sf=~flag_sf; //�л�״̬��flag_sf=0��ʱ�� flag_sf=1������
				 }
				display_mode();
			while(!K1);	//���ּ��
		}
	}
	if(K2==0)						  //��ʱ���
	{
		delay1(1000);
		if(K2==0)
		{
			if(mode_flag!=0)			
			{ 
				 been(300);
			   if(flag_sf==0)			//��ʱ��״̬
			   {
			   	  temp_shi++;
				  if(temp_shi==24)temp_shi=0;		 //�����ʱ�ӵ�24������
			   }
			   else {							   //������״̬
			   	  temp_fen++;
				  if(temp_fen==60)temp_fen=0;		  //����ӷ��ӵ�60������
			   }			  	
			}
			while(!K2);	//���ּ��
		}	
	}	
	if(K3==0)						   //��ʱ���
	{
		delay1(1000);
		if(K3==0)
		{
			if(mode_flag!=0)
			{
				been(300);
			   if(flag_sf==0)			   //��ʱ��״̬
			   {
			   	
				if(temp_shi==0)temp_shi=24;		//�����ʱ�Ӽ�����͵�0���ֻص����λ24
			   	  temp_shi--;
				  
			   }
			   else {						 //������״̬

			   	if(temp_fen==0)temp_fen=60;			//��������Ӽ�����͵�0���ֻص����λ60
			   	  temp_fen--; 
			   }
			  	
			}
		  
			while(!K3);	//���ּ��
		}
	
	}
										 //ȷ�����õ�ʱ��͵������ӵ�ʱ��
	if(K4==0)
	{
		delay1(1000);
		if(K4==0)
		{
			if(mode_flag==1)				  //����ʱ��  ���úõ�ʱ��ȷ������������
			{
				been(300);
				now_shi=temp_shi;			   
				now_fen=temp_fen;
				now_miao=temp_miao;
				mode_flag=0;
			}
			if(mode_flag==2)					 //��������	���úõ�ʱ��ȷ����֮��Ϊ����
			{									 
				been(300);
				nao_shi=temp_shi;
				nao_fen=temp_fen;
				nao_miao=temp_miao;
				mode_flag=0;
			}
			while(!K4);	//���ּ��
		}
		display_mode();			   //������ʾģʽ����
	
	}

	if(K5==0)					   //��������
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
			while(!K5);	//���ּ��
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
void delay(uint ms)	   //�������ʱ����
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
//��������time0
//�������ܣ���ʱ��T0���жϺ�����T0�ڹ�����ʽ1��ÿ50ms�����жϣ�ִ�и��жϺ���
void time0() interrupt 1		 
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;

	cishu++;
	if(cishu==20)			   //һ��ʱ�䵽�󣬴������㣬������һ
	{
		cishu=0;
		now_miao++;
	}
	if(now_miao==60)			//60��ʱ�䵽���������㣬���Ӽ�һ
	{
	 	now_miao=0;
		now_fen++;
	}

	if(now_fen==60)				//60����ʱ�䵽�󣬷������㣬ʱ�Ӽ�һ
	{
		
	  now_fen=0;
	  now_shi++;
	}

	if(now_shi==24)				 //24Сʱ����ʱ������
	{
	   now_shi=0;
	}
}

//��������lcd_int
//�������ܣ�lcd��ʼ��
void lcd_int()
{
	lcd_w_cmd(0x3c);	//���ù�����ʽ
	lcd_w_cmd(0x0c);	  //������ʾ״̬
	lcd_w_cmd(0x01);	 //����
	lcd_w_cmd(0x06);	 //�������뷽ʽ
	lcd_w_cmd(0x80);	 //���ó�ʼ��ʾλ��
}
//��������lcd_w_cmd
//�������ܣ�д����
//��ʽ�������������Ѿ�����com��Ԫ��
void lcd_w_cmd(unsigned char com)
{
	unsigned char i;
	do
	{						 //��lcdæ״̬
	i=lcd_r_start();		  //���ö�״̬�ֺ���
	i&=0x80;				   //���롱�������ε���7λ
	delay1(2);
	}while(i!=0);				//lcdæ��������ѯ�������˳�ѭ�� 
	RW=0;
	delay1(1);
	RS=0;						 //RW=0��RS=0��дlcd������
	delay1(1);
	E=1;						  //E��ʱ����0��1�ߵͲ���
	delay1(1);
	P0=com;						  //��com�е�������д��lcd���ݿ�
	delay1(1);
	E=0;
	delay1(1);
	RW=1;
	delay1(2);
}
//��������lcd_w_dat
//�������ܣ�д����
void lcd_w_dat(unsigned char dat)
{
	unsigned char i;
	do
	{						   //��æ����
	i=lcd_r_start();		   //���ö�״̬�ֺ���
	i&=0x80;				   //���롱�������ε���7λ
	delay1(2);
	}while(i!=0);			   //lcdæ��������ѯ�������˳�ѭ�� 
	RW=0;
	delay1(1);
	RS=1;						 //RW=0��RS=1��дlcd������
	delay1(1);
	E=1;						  //E��ʱ����0��1�ߵͲ���
	delay1(1);
	P0=dat;						   //��dat�е���ʾ����д��lcd���ݿ�
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
//��������lcd_r_start
//�������ܣ���״̬��
unsigned char lcd_r_start()
{
	unsigned char s;
	RW=1;			  //RW=1,RS=0,��lcd״̬
	delay1(1);
	RS=0;
	delay1(1);
	E=1;			   //E��ʱ����0��1�ߵͲ���
	delay1(1);
	s=P0;				 //��lcd�����ݿڶ�״̬  
	delay1(1);
	E=0;
	delay1(1);
	RW=0;
	delay1(1);
	return(s);			 //����ֵs��ȡ״̬æ�����
}




