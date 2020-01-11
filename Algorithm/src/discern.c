#include "discern.h"
#include "delay.h"
#include "lcd.h"

#include "ShowChar.h"

#include <stdio.h>

extern vu8 TableChangePoint_240[240];//�����240��

extern vu8 Max_ChangePoint_240,Min_ChangePoint_240,Max_bChangePoint,Min_bChangePoint;//���������ʼ��ĩ����,��������ʼ��ĩ����
extern vu8 flag_MaxMinCompare;//Max_aChangePoint_reset_1��Max_aChangePoint_reset�ı�־

extern float V,S,H;//����HSVֵ

extern vu16 Min_blue;
extern vu16 Max_blue;//���峵����ɫ����ĺ������ֵ����Сֵ

extern vu8 TableChangePoint_320[320];//���������320��

//��ʾ����
extern vu8 table_char[36];

// all
extern vu8 table_picture[150]; //���屣��ͼƬ������
extern u8 Table[6300];

//�ַ�ʶ��
extern vu8 table_cardMeasure[7];//�����ĳ��ƽ��
extern vu8 table_card[5][8];
extern vu8 table_char_char[36];

extern vu16 k1,kk1,k2,kk2,k3,kk3,k4,kk4,k5,kk5,k6,kk6,k7,kk7,k8,kk8;//�˸��ַ��߽�


void ChangePoint_Show_240(void) { //240�����������ʾ
	vu16 a=0,b=0;
	for(a=0;a<240;a++)//�����ο���10��20��30
	{
		LCD_DrawPoint(10,a,0x63<<5);//10
		LCD_DrawPoint(20,a,0x63<<5);//20
		LCD_DrawPoint(30,a,0x63<<5);//30
	}
	
	for(a=0;a<240;a++)//��ʾ��Ӧ�ĺ��������								
	{
		LCD_DrawPoint(TableChangePoint_240[a],a,0xf800);//�������ʾ����ɫ���
		if(TableChangePoint_240[a]>=12)					//������������ֵ���趨       ��ֵ����3-��1��
		{
			for(b=35;b<40;b++)						//��ʾ�ﵽ��ֵ��׼�ĵ�
			{
				LCD_DrawPoint(b,a,0x63<<5);//Green			
			}
		}
	}
}


void ChangePoint_Analysis_240(void) { //240��������  ��ȡ�߶�
	vu16 a=0,b=0;
	Min_ChangePoint_240=240;Max_ChangePoint_240=0;
	
	for(a=0;a<240;a++)//240ɨ��	����ȡ������ֵ	��Min_ChangePoint_240��Max_ChangePoint_240				
	{
		while(TableChangePoint_240[a]<12)									//��ֵ����3-��2��
		{
			a++;
		}
		Min_ChangePoint_240=a;//�ϱ߽�
		while(TableChangePoint_240[a]>=12)									//��ֵ����3-��3��
		{
			a++;
		}
		Max_ChangePoint_240=a;//�±߽�
		if(Max_ChangePoint_240-Min_ChangePoint_240>=15) a=240;//��������ֵ   	//��ֵ����2-��1��
	}
	Min_ChangePoint_240=Min_ChangePoint_240-3;//����΢��3����
	Max_ChangePoint_240=Max_ChangePoint_240+2;//����΢��2����
	for(a=30;a<280;a++)//��ʾ�Ͻ���				
	{
		LCD_DrawPoint(a,Max_ChangePoint_240,0x001f);
	}
	for(a=30;a<280;a++)//��ʾ�½���						
	{
		LCD_DrawPoint(a,Min_ChangePoint_240,0x001f);
	}
	for(a=30;a<280;a++)//��ʾ50,�ο�50����λ�ô�������λ�ò�Ҫ��������ߣ���ò����ַ��Ĺ�һ������						
	{
		LCD_DrawPoint(a,Min_ChangePoint_240+50,0xf800);
	}
	flag_MaxMinCompare=1;
	if(Min_ChangePoint_240>Max_ChangePoint_240)//�жϺϷ���1����Сֵ>���ֵ
	{
		flag_MaxMinCompare=0;
	}
	if(Min_ChangePoint_240==240||Max_ChangePoint_240==0)//�жϺϷ���2��ֵû�����¸�ֵ
	{
		flag_MaxMinCompare=0;
	}
	if(Max_ChangePoint_240-Min_ChangePoint_240<15)		//�жϺϷ���3��			//��ֵ����2-��2��
	{
		flag_MaxMinCompare=0;
	}
}

static void RGB_HSV(vu16 num){                                                        //RGB565תHSV

	float max=0.00,min=0.00;
	vu8 r=0,g=0,b=0;
	r=(num>>11)*255/31;g=((num>>5)&0x3f)*255/63;b=(num&0x1f)*255/31;
	
	max=r;min=r;
	if(g>=max)max=g;
	if(b>=max)max=b;
	if(g<=min)min=g;
	if(b<=min)min=b;
	
	V=100*max/255;                                                                     //ת��Ϊ�ٷֱ�
	S=100*(max-min)/max;                                                               //����100����ʾ
	if(max==r) H=(g-b)/(max-min)*60;
	if(max==g) H=120+(b-r)/(max-min)*60;
	if(max==b) H=240+(r-g)/(max-min)*60;
	if(H<0) H=H+360;
}

void ChangePoint_Analysis_Blue(void) {                                                //320��ɫ�������,���ö�ȡ���أ��ý��Min_blue,Max_blue
	vu16 a=0,b=0,num_color=0;
	vu16 min_320=0,max_320=0;                                                           //���е���С�����ֵ
	
	Min_blue=0;Max_blue=320;
	min_320=320;max_320=0;
	
	for(a=Min_ChangePoint_240;a<Max_ChangePoint_240;a++) {                              //�����±߽�ɨ��	
		for(b=30;b<290;b++) {                                                             //���õ�320    for(b=30;b<320;b++)
			num_color=LCD_ReadPoint(b,a);                                                   //��ȡ���أ������Ż��ٶ��д����� ��ɨ�跽��Ҳ���Ż����������ٶ�
			RGB_HSV(num_color);                                                             //RGB565תHSV
			if( 250>H && H>190 && 60>S && S>15 && 100>V && V>45) {                          // �� ��ɫHSV ��ֵ�Ƚ�
				if(b<min_320) {                                                               //��ú����Min��Maxֵ������ɫ���Ƶ����ұ߽�
					min_320=b;                                                                  //�õ���߽�
				}
				if(b>max_320) {
					max_320=b;                                                                  //�õ��ұ߽�
				}
			}
		}
	}
	Min_blue=min_320;                                                                   //��ȡ���е����ֵ//����һ��
	Max_blue=max_320-5;                                                                 //��ȡ���е���Сֵ//����һ��
	
	for(a=Min_ChangePoint_240;a<Max_ChangePoint_240;a++) {                              //��ʾ�����		
		LCD_DrawPoint(Min_blue,a,0xf8);                                                   //LCD_DrawPoint(Min_blue,a,0xf800);
	}
	for(a=Min_ChangePoint_240;a<Max_ChangePoint_240;a++) {                              //��ʾ�ҽ���	
		LCD_DrawPoint(Max_blue,a,0xf800);
	}
}



void ChangePoint_Analysis_320(void) {								                                  //��ɫ�����У�320��������,���TableChangePoint_320[b]���
																																											//(�ȶ�ֵ�������жϰ׵������=0���Ƿָ��ߣ�
	vu16 a=0,b=0,num_color=0;
	vu8 R1=0,G1=0,B1=0;
	vu8 Mid_ChangePoint_240=0;
	vu8 max_R=0,max_G=0,max_B=0,min_R=0,min_G=0,min_B=0;
	vu8 mid_R=0,mid_G=0,mid_B=0;
	
	max_R=0;max_G=0;max_B=0;
	min_R=30;min_G=60;min_B=30;
	
	Mid_ChangePoint_240=(Min_ChangePoint_240+Max_ChangePoint_240)/2;
	
	for(b=Min_blue;b<Max_blue;b++) {																									  //���ұ߽�
		num_color=LCD_ReadPoint(b,Mid_ChangePoint_240);                                   //��ȡ���أ������Ż��ٶ��д����� ��ɨ�跽��Ҳ���Ż����������ٶ�
		R1=num_color>>11;
		G1=(num_color>>5)&0x3F;
		B1=num_color&0x1F;
		if( (R1>10) && (G1>25) && (B1>15) && (R1<=30) && (G1<=60) && (B1<=30) ) {        //��ֵ��,����ֵ��25.55.25���Ϻ�����ֵ��21,47,21��
			if(max_R<R1) max_R=R1;                                                         //������ֵ����Сֵ
			if(max_G<G1) max_G=G1;
			if(max_B<B1) max_B=B1;
			
			if(min_R>R1) min_R=R1;
			if(min_G>G1) min_G=G1;
			if(min_B>B1) min_B=B1;		
		}
	}
	mid_R=(max_R+min_R)/2;
	mid_G=(max_G+min_G)/2;
	mid_B=(max_B+	min_B)/2;
 
	for(b=0;b<320;b++) {                                                               //����������������������
		TableChangePoint_320[b]=0;
	}
	for(a=Min_ChangePoint_240;a<Max_ChangePoint_240;a++) {
		for(b=Min_blue+1;b<Max_blue;b++) {
			num_color=LCD_ReadPoint(b,a);                                                  //��ȡ���أ������Ż��ٶ��д����� ��ɨ�跽��Ҳ���Ż����������ٶ�
			R1=num_color>>11;
			G1=(num_color>>5)&0x3F;
			B1=num_color&0x1F;
			if((R1>=mid_R) && (G1>=mid_G) && (B1>=mid_B)) {                                //��ֵ��,����ֵ��25.55.25���Ϻ�����ֵ��21,47,21��
				LCD_DrawPoint(b,a,0xffff);
				TableChangePoint_320[b]++;                                                   //��ɫ�������+1
			}
			else {
				LCD_DrawPoint(b,a,0x0000);
			}
		}
	}
}


void ChangePoint_Show_320(void) {        //320�����������ʾ
	vu16 a=0,b=0;
	for(a=0;a<320;a++) {                   //��ʾ��Ӧ�ĺ��������	
		if(TableChangePoint_320[a]==0) {
			LCD_DrawPoint(a,0,0x001F);            //�������ʾ����ɫ���
		}
		else {
			LCD_DrawPoint(a,TableChangePoint_320[a],0xf800);//�������ʾ����ɫ���
		}
	}
}

//�ַ��ָ�,���طָ���ַ������������жϺϷ���
vu8 SegmentationChar(void) {
	vu16 a=0,b=0;
	vu8 i=0;//ͳ�Ʒָ���ַ���������Ϊ9˵���ָ�����
	 
	for(b = Max_blue; b > Min_blue; b--) {                      // ���ҽ��ߵ�ɨ�� 
		if(TableChangePoint_320[b] == 0) {       //��϶�ָ�  ����HSV�Ƚ� �����Ϊ0 �����϶
			for(a = Min_ChangePoint_240; a < Max_ChangePoint_240 ; a++) {         //����--������ ���Ƹ߶�һ������
 				LCD_DrawPoint(b,a+1,0x001f);
			}
			i++;
			b--;
			while(TableChangePoint_320[b] == 0) {            //�����ߺ��ҵ�����㲻Ϊ0�ĵط�
				b--;
				if(b <= Min_blue) break;
			}
		}
	}
	i--;
	LCD_ShowNum(30,220,i,2);//��ʾ�ָ���ַ�����+1��8������ֵ
	return i;
}

void Show_Title(void)//��ʾ����
{
	LCD_ShowChar(35+8*0,10,'N',0);
	LCD_ShowChar(35+8*1,10,'u',0);
	LCD_ShowChar(35+8*2,10,'m',0);
	LCD_ShowChar(35+8*3,10,'b',0);
	LCD_ShowChar(35+8*4,10,'e',0);
	LCD_ShowChar(35+8*5,10,'r',0);
	
	LCD_ShowChar(35+8*9,10,'T',0);
	LCD_ShowChar(35+8*10,10,'i',0);
	LCD_ShowChar(35+8*11,10,'m',0);
	LCD_ShowChar(35+8*12,10,'e',0);

	
	LCD_ShowChar(35+8*17,10,'P',0);
	LCD_ShowChar(35+8*18,10,'r',0);
	LCD_ShowChar(35+8*19,10,'i',0);
	LCD_ShowChar(35+8*20,10,'c',0);
	LCD_ShowChar(35+8*21,10,'e',0);
}



void Show_Card(vu8 i)//��ʾ�ڼ��鳵��
{
	vu16 t0=0;
//��ʾ����
	if(table_card[i][0]!=0)
	{
		WordShow(table_card[i][0],9,i*16+50);//
	}
	
	if(table_card[i][1]<10)
	{
		LCD_ShowNum(25,i*16+50,table_char[table_card[i][1]],1);
	}
	else
	{
		LCD_ShowChar(25,i*16+50,table_char[table_card[i][1]],0);
	}
	LCD_ShowChar(33,i*16+50,'.',0);														//��
	if(table_card[i][2]<10)
	{
		LCD_ShowNum(41,i*16+50,table_char[table_card[i][2]],1);
	}
	else
	{
		LCD_ShowChar(41,i*16+50,table_char[table_card[i][2]],0);
	}
	if(table_card[i][3]<10)
	{
		LCD_ShowNum(49,i*16+50,table_char[table_card[i][3]],1);
	}
	else
	{
		LCD_ShowChar(49,i*16+50,table_char[table_card[i][3]],0);
	}
	if(table_card[i][4]<10)
	{
		LCD_ShowNum(57,i*16+50,table_char[table_card[i][4]],1);
	}
	else
	{
		LCD_ShowChar(57,i*16+50,table_char[table_card[i][4]],0);
	}
	if(table_card[i][5]<10)
	{
		LCD_ShowNum(65,i*16+50,table_char[table_card[i][5]],1);
	}
	else
	{
		LCD_ShowChar(65,i*16+50,table_char[table_card[i][5]],0);
	}
	if(table_card[i][6]<10)
	{
		LCD_ShowNum(73,i*16+50,table_char[table_card[i][6]],1);
	}
	else
	{
		LCD_ShowChar(73,i*16+50,table_char[table_card[i][6]],0);
	}
	t0=table_card[i][7];
	LCD_ShowNum(100,i*16+50,t0,6);//��ʾʱ��

	if(t0<60)
	{
		LCD_ShowNumPoint(168,i*16+50,t0*8);
	}
	else
	{
		LCD_ShowNumPoint(168,i*16+50,t0/60*500+t0%60*8);
	}
	
}



static void GuiYi(vu16 k,vu16 kk)//��һ�� 24*50
{
	vu16 a=0,b=0,e=0;
	vu16 num=0;//�����ȡ����
	vu8 Mo=0,Yu=0;//ȡ����ȡģ
	vu8 num1=0,num2=0,num3=0;
	vu8 Mo_1=0;//
	vu8 Min_240=0,Max_240=0;//����ַ����������
	
	if((k-kk)<25)
	{
		//����ַ�
		Min_240=Min_ChangePoint_240+1; //���Ƹ߶�����
		Max_240=Max_ChangePoint_240-1; //���Ƹ߶�����
		while(Min_240++)//����󣬵õ�: Min_240
		{
			for(b=kk+1;b<k;b++)//kk1��k1                                
			{
				num=LCD_ReadPoint(b,Min_240);
				if(num) break;
			}
			if(num) break;
		}
		while(Max_240--)//����󣬵õ�: Max_240
		{
			for(b=kk+1;b<k;b++)//kk1��k1                                
			{
				num=LCD_ReadPoint(b,Max_240);
				if(num) break;
			}
			if(num) break;
		}
		Min_240-=1;
		Max_240+=2;
		LCD_DrawPoint(kk,Min_240,0xffff);//
		LCD_DrawPoint( k,Max_240,0xffff);//
		//��ʾ���Ƶ�ͼƬ
		num3=0;
		for(a=Min_240+1;a<Max_240;a++) //�߶�
		{
			num2=0;
			for(b=kk+1;b<k;b++)//kk1��k1   ���                                  +1
			{
				num=LCD_ReadPoint(b,a);
				LCD_DrawPoint(271-(k-kk-1)+num2,191+num3,num);//��������ֵ  ��ʾ
				num2++;
			}
			num3++;
		}
		
		num3=0;
		//ͼƬ�Ŵ��㷨������ڲ�ֵ �Ƚ���ȷŴ󣬺󽫳��ȷŴ�  ��24*��50
		//��ԭ��ͼ��ȵ����ص�����Ŀ��24���ص�������
		Mo=(24-(k-kk-1))/(k-kk-1);//ȡģ �����Խ��Ŵ���ԭͼ��ȶ�����ص����ÿ��ԭͼ���ص�ĸ���ΪMo
		Yu=(24-(k-kk-1))%(k-kk-1);//ȡ��
		if(Yu!=0) 
		{
			Mo_1=24/Yu;//ƽ��Mo_1�����أ�����һ�����أ� Yu��������Ҫ��ԭͼ�Ŀ���ϲ�Yu�����ص���ܴﵽĿ���24������  
		}
// 		LCD_ShowNum(30,20,Mo,3);//��ʾģ		<������>
// 		LCD_ShowNum(70,20,Yu,3);//��ʾ��
// 		LCD_ShowNum(100,20,(k1-kk1),3);//��ʾ��ֵ

		for(a=Min_240+1;a<Max_240;a++)//��Ŵ�Ϊ24����    ��ɨ��
		{//��
			num2=0;
			Yu=(24-(k-kk-1))%(k-kk-1);//ȡ��
			
				for(b=kk+1;b<k;b++)//kk1��k1                                   +1
				{
					num=LCD_ReadPoint(b,a);
					LCD_DrawPoint(271+num2,191+num3,num);
					num2++;
					Mo=(24-(k-kk-1))/(k-kk-1);//ȡģ
					while(Mo)
					{
						LCD_DrawPoint(271+num2,191+num3,num);
						Mo--;
						num2++;
					}
					if(Yu!=0)//��������
					{	
						if(((num2+1)%Mo_1==0) && (num2!=1))//�ò���ĵط� 
						{
							LCD_DrawPoint(271+num2,191+num3,num);
							Yu--;
							num2++;
						}
					}
				}
			num3++;
		}
		LCD_DrawPoint(271,191,0x07E0);//��ǵ㣬4������
		LCD_DrawPoint(271,240,0x07E0);
		LCD_DrawPoint(295,191,0x07E0);
		LCD_DrawPoint(295,240,0x07E0);
//��������
		if((Max_240-Min_240)<50)
		{
			Mo=(50-(Max_240-Min_240+1))/(Max_240-Min_240+1);//ȡģ
			Yu=(50-(Max_240-Min_240+1))%(Max_240-Min_240+1);//ȡ��
			Mo_1=50/Yu;
			
// 			LCD_ShowNum(30,170,Mo,3);//					<������>
// 			LCD_ShowNum(70,170,Yu,3);//
// 			LCD_ShowNum(100,170,Max_ChangePoint_240-Min_ChangePoint_240,3);//
			num2=0;
			for(a=0;a<(Max_240-Min_240);a++)//����ͼ��,���Ƿ�Χ�Ƿ���Ҫ����������
			{//��
				for(b=271;b<=295;b++)//271��ʼ���ƣ�295�Ž���
				{
					num=LCD_ReadPoint(b,a+191);
					LCD_DrawPoint(b+25,191+num2,num);//��������ֵ
				}
				num2++;
				while(Mo)
				{
					for(b=271;b<=295;b++)//271��ʼ���ƣ�295�Ž���
					{
						num=LCD_ReadPoint(b,a+191);
						LCD_DrawPoint(b+25,191+num2+a,num);//��������ֵ
					}
					Mo--;
					num2++;
				}
				if(Yu!=0)
				{
					if((((num2+1) % Mo_1)==0)&& (num2!=1))
					{
						for(b=271;b<=295;b++)//271��ʼ���ƣ�295�Ž���
						{
							num=LCD_ReadPoint(b,a+191);
							LCD_DrawPoint(b+25,191+num2,num);//��������ֵ
						}
						Yu--;
						num2++;
					}
				}					
			}
		}
		LCD_DrawPoint(320,191,0xf800);//��ǵ㣬1������
	}
}

static vu8 MoShiShiBie_All(vu8 begin,vu8 end)//�ַ�ƥ�䣬ģʽʶ��,ѡ����ƥ��begin-end
{
	vu16 num_save=0;
	vu8  a=0,b=0,e=0,a_save=0,st1=0,st2=0,s1=0,s2=0;
	 int num1=0;
	for(a=begin;a<end;a++)					//36
	{
		num1=0; 
		for(b=0;b<150;b++) //ÿ���ַ�������150���ֽ���ģ���ݣ� ������24*50=1200�� 1200/8=150�ֽڡ�
		{
					st1=table_picture[b]; //�õ���ͼƬװ���� ����
					st2=Table[150*a+b];
					for(e=0;e<8;e++) //����ֽ����λ���бȽ�
					{
						s1=st1&(1<<e);
						s2=st2&(1<<e);
						if(s1==s2) num1++; //��ͬ������
						if(s1!=s2) num1--; //��ͬ�����
					}
		}
		if(num_save<num1)
		{
			num_save=num1;
			a_save=a;
		}
		
		LCD_ShowNum(50,220,a,2);				//��ʾƥ����ַ���"a"			<������>
		if(num1<0) {
			LCD_ShowNum(70,220,0,4);			//��ʾƥ�����ȷ������       <������>
		} else {
		LCD_ShowNum(70,220,num1,4);			//��ʾƥ�����ȷ������     <������> 
	}		
		LCD_ShowNum(120,220,num_save,4);//ƥ������ֵ��ʾ         <������>
 	 	
	}
	return a_save;
}


void ZhiFuShiBie(void)//�ַ�ʶ��
{
	vu16 a=0,b=0,e=0;
	vu16 i=0,u=0;
	vu8 Result=0;//ʶ����
  vu8 temp[50]={0},temp1[50]={0};

	for(b=Max_blue-1;b>Min_blue;b--)//��������ʶ�𣬻�ȡ�����ַ���k,kKֵ
	{
		while(TableChangePoint_320[b]==0) b--;//ȡ��1���ַ�
		k1=b+1;//+1 �ҵ��ַ������ұ߽�
		while(TableChangePoint_320[b]>0) b--;
		kk1=b;     //�ҵ��ַ������ұ߽�
		if((k1-kk1)<4)//ʡ�Ե����������ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;// ������һ�����ұ߽�
			k1=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk1=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��2���ַ�
		k2=b+1;
		while(TableChangePoint_320[b]>0) b--;
		kk2=b;
		if((k2-kk2)<4)//ʡ�Ե���3�����ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;//
			k2=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk2=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��3���ַ�
		k3=b+1;//+1
		while(TableChangePoint_320[b]>0) b--;
		kk3=b;
		if((k3-kk3)<4)//ʡ�Ե���3�����ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;//
			k3=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk3=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��4���ַ�
		k4=b+1;
		while(TableChangePoint_320[b]>0) b--;
		kk4=b;
		if((k4-kk4)<4)//ʡ�Ե���3�����ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;//
			k4=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk4=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��5���ַ�
		k5=b+1;//+1
		while(TableChangePoint_320[b]>0) b--;
		kk5=b;
		if((k5-kk5)<4)//ʡ�Ե���3�����ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;//
			k5=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk5=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��6���ַ�
		k6=b+1;
		while(TableChangePoint_320[b]>0) b--;
		kk6=b;
		while(TableChangePoint_320[b]==0) b--;//ȡ��7���ַ�
		k7=b+1;//+1
		while(TableChangePoint_320[b]>0) b--;
		kk7=b;
		if((k7-kk7)<4)//ʡ�Ե���3�����ص�λ��
		{
			while(TableChangePoint_320[b]==0) b--;//
			k7=b+1;//+1
			while(TableChangePoint_320[b]>0) b--;
			kk7=b;
		}
		while(TableChangePoint_320[b]==0) b--;//ȡ��8���ַ�
		k8=b+1;
 		while(TableChangePoint_320[b]>0) 
		{
			if(b<=Min_blue)
			{
				break;
			}
			b--;
		}
		kk8=b;
		b=Min_blue;//�Է���һ��������forѭ������
	}
	for(a=Min_ChangePoint_240;a<Max_ChangePoint_240;a++)//����  ���ָ�������ұ߽���ʾ����
	{
		LCD_DrawPoint(k1,a+1,0x001f);
		LCD_DrawPoint(kk1,a+1,0x001f);
		LCD_DrawPoint(k2,a+1,0x001f);
		LCD_DrawPoint(kk2,a+1,0x001f);
		LCD_DrawPoint(k3,a+1,0x001f);
		LCD_DrawPoint(kk3,a+1,0x001f);
		LCD_DrawPoint(k4,a+1,0x001f);
		LCD_DrawPoint(kk4,a+1,0x001f);
		LCD_DrawPoint(k5,a+1,0x001f);
		LCD_DrawPoint(kk5,a+1,0x001f);
		LCD_DrawPoint(k6,a+1,0x001f);
		LCD_DrawPoint(kk6,a+1,0x001f);
		LCD_DrawPoint(k7,a+1,0x001f);
		LCD_DrawPoint(kk7,a+1,0x001f);
		LCD_DrawPoint(k8,a+1,0x001f);
		LCD_DrawPoint(kk8,a+1,0x001f);
	}
//��һ��������СΪ24*50
	
//��1���ַ���
	GuiYi(k1,kk1);//��һ�� 24*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(0,36);//�ַ�ƥ�䣬ģʽʶ��,����a,0<= a <36
	if(Result<10)
	{
		LCD_ShowNum(240,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(240,220,table_char[Result],0);
	}
	table_cardMeasure[6]=Result;//����ʶ��ĳ��ƽ��
	
//��2���ַ���
	GuiYi(k2,kk2);//��һ�� 25*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(0,36);//�ַ�ƥ�䣬ģʽʶ��
	if(Result<10)
	{
		LCD_ShowNum(230,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(230,220,table_char[Result],0);
	}
	table_cardMeasure[5]=Result;//����ʶ��ĳ��ƽ��
	
	GuiYi(k3,kk3);//��һ�� 25*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(0,36);//�ַ�ƥ�䣬ģʽʶ��
	if(Result<10)
	{
		LCD_ShowNum(220,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(220,220,table_char[Result],0);
	}
	table_cardMeasure[4]=Result;//����ʶ��ĳ��ƽ��
	
	GuiYi(k4,kk4);//��һ�� 25*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(0,36);//�ַ�ƥ�䣬ģʽʶ��
	if(Result<10)
	{
		LCD_ShowNum(210,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(210,220,table_char[Result],0);
	}
	table_cardMeasure[3]=Result;//����ʶ��ĳ��ƽ��
	
	GuiYi(k5,kk5);//��һ�� 25*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(0,36);//�ַ�ƥ�䣬ģʽʶ��
	if(Result<10)
	{
		LCD_ShowNum(200,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(200,220,table_char[Result],0);
	}
	table_cardMeasure[2]=Result;//����ʶ��ĳ��ƽ��
	
	LCD_ShowChar(190,220,'.',0);

	GuiYi(k7,kk7);//��һ�� 25*50
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(10,36);//�ַ�ƥ�䣬ģʽʶ��ֻƥ����ĸ
	if(Result<10)
	{
		LCD_ShowNum(180,220,table_char[Result],1);
	}
	else
	{
		LCD_ShowChar(180,220,table_char[Result],0);
	}
	table_cardMeasure[1]=Result;//����ʶ��ĳ��ƽ��
	
	GuiYi(k8,kk8);//��һ�� 25*50					���һ������
	PictureToString();//ͼƬ->����
	Result=MoShiShiBie_All(36,42);//�ַ�ƥ�䣬ƥ�人��
	WordShow(Result-35,160,220);//��ʾ����
	table_cardMeasure[0]=Result-35;//����ʶ��ĳ��ƽ��
	//ʶ�����
	//���ڷ��ͳ�����Ϣ     //
	if(Result==36)
	{
		sprintf ((char*)temp,"ʶ��������");
	}
	else if(Result==37)
	{
		sprintf ((char*)temp,"ʶ��������");
	}
	else if(Result==38)
	{
		sprintf ((char*)temp,"ʶ��������");
	}
	else if(Result==39)
	{
		sprintf ((char*)temp,"ʶ��������");
	}
	else if(Result==40)
	{
	sprintf ((char*)temp,"ʶ��������");
	}
	else if(Result==41)
	{
		sprintf ((char*)temp,"ʶ��������");
	}
	
	sprintf((char*)temp1,"%c.%c%c%c%c%c\r\n"
		,table_char_char[table_cardMeasure[1]],table_char_char[table_cardMeasure[2]],
	table_char_char[table_cardMeasure[3]],table_char_char[table_cardMeasure[4]],
	table_char_char[table_cardMeasure[5]],table_char_char[table_cardMeasure[6]]);

	//��ƥ���ѱ���ĳ��ƺ�
	for(u=0;u<5;u++)
	{
				for(i=0;i<7;i++)
				{
					if(table_card[u][i]!=table_cardMeasure[i]) i=8;//�˳�forѭ��
				}	
			if(i==7)//ƥ��ɹ�
			{
				LCD_Fill(0x00);//����
				Show_Title();//��ʾ����
				Show_Card(u);//��ʾ�ڼ��鳵��			
				u=5;
				while(GPIO_ReadInputDataBit(KEY1_PORT,KEY2_PIN)==1);
			}
	}
	if(i==9)//��ƥ�䳵�ƣ��򱣴泵��
	{
		i=0;
		while(1)
		{
			if(GPIO_ReadInputDataBit(KEY1_PORT,KEY2_PIN)==0) break;
			LCD_ShowNum(30,220,i/100,2);
			if(i==300)														//��������
			{
				for(u=0;u<5;u++)
				{
					if(table_card[u][0]==0)
					{
						for(i=0;i<7;i++)
						{
							table_card[u][i]=table_cardMeasure[i];
						}					
						u=5;//�˳�ѭ��
					}
				}
				LCD_Fill(0x00);//����
				Show_Title();//��ʾ����
				Show_Card(0);//��ʾ�ڼ��鳵��
				Show_Card(1);
				Show_Card(2);
				Show_Card(3);
				Show_Card(4);
			 while(GPIO_ReadInputDataBit(KEY1_PORT,KEY2_PIN)==1);
				break;
			}
			delay_ms(1);
			i++;
		}
		
	}
	
}


