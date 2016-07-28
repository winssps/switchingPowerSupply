#include"st7565.h"

/*******************************************************************************
* 函 数 名         : LCD12864_WriteCmd
* 函数功能		   : 写入一个命令到12864
* 输    入         : cmd
* 输    出         : 无
*******************************************************************************/

void LcdSt7565_Write(uchar cmd,at,dat)
{
	LCD12864_CS = 0;	   //chip select,打开片选
	LCD12864_RD = 1;	   //disable read，读失能	
	LCD12864_RS = cmd;       //select command，选择命令
	LCD12864_RW = 0;       //select write，选择写模式
	_nop_();
	_nop_();

	if(at == 0)DATA_PORT = dat; //put command，放置命令
	else DATA_PORT = ~dat;
	_nop_();
	_nop_();

	LCD12864_RW = 1;	   //command writing ，写入命令
}
/*******************************************************************************
* 函 数 名         : LCD12864_Init
* 函数功能		   : 初始化12864
* 输    入         : 无
* 输    出         : 无
* 说    明         : LCD12864的命令指令可以查看例程文件夹下的《ST7565p数据手册》
*                  * 的第51页的位置。
*******************************************************************************/

void Lcd12864_Init()
{
	uchar i;
	P4SW = 0X70;
	LCD12864_RSET = 0;
	for (i=0; i<100; i++);
	LCD12864_CS = 0;
	LCD12864_RSET = 1;
	
	//----------------Star Initial Sequence-------//
	//------程序初始化设置，具体命令可以看文件夹下---//

	//--软件初始化--//
	LcdSt7565_Write(0,0,0xE2);  //reset
	for (i=0; i<100; i++);	   //延时一下

	//--表格第8个命令，0xA0段（左右）方向选择正常方向（0xA1为反方向）--//
	LcdSt7565_Write(0,0,0xA0);  //ADC select segment direction 
	
	//--表格第15个命令，0xC8普通(上下)方向选择选择反向，0xC0为正常方向--// 
	LcdSt7565_Write(0,0,0xC8);  //Common direction 
	                  
	//--表格第9个命令，0xA6为设置字体为黑色，背景为白色---//
	//--0xA7为设置字体为白色，背景为黑色---//
	LcdSt7565_Write(0,0,0xA6);  //reverse display

	//--表格第10个命令，0xA4像素正常显示，0xA5像素全开--//
	LcdSt7565_Write(0,0,0xA4);  //normal display
	
	//--表格第11个命令，0xA3偏压为1/7,0xA2偏压为1/9--//
	LcdSt7565_Write(0,0,0xA2);  //bias set 1/9
	
	//--表格第19个命令，这个是个双字节的命令，0xF800选择增压为4X;--//
	//--0xF801,选择增压为5X，其实效果差不多--//	
	LcdSt7565_Write(0,0,0xF8);  //Boost ratio set
	LcdSt7565_Write(0,0,0x01);  //x4
	
	//--表格第18个命令，这个是个双字节命令，高字节为0X81，低字节可以--//
	//--选择从0x00到0X3F。用来设置背景光对比度。---/
	LcdSt7565_Write(0,0,0x81);  //V0 a set
	LcdSt7565_Write(0,0,0x23);

	//--表格第17个命令，选择调节电阻率--//
	LcdSt7565_Write(0,0,0x25);  //Ra/Rb set
	
	//--表格第16个命令，电源设置。--//
	LcdSt7565_Write(0,0,0x2F);
	for (i=0; i<100; i++);

	//--表格第2个命令，设置显示开始位置--//
	LcdSt7565_Write(0,0,0x40);  //start line

	//--表格第1个命令，开启显示--//
	LcdSt7565_Write(0,0,0xAF);  // display on
	for (i=0; i<100; i++);

}

/*******************************************************************************
* 函 数 名         : LCD12864_ClearScreen
* 函数功能		   : 清屏12864
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Lcd12864_ClearScreen(uchar temp)
{
	uchar i, j;

	for(i=0; i<8; i++)
	{
		//--表格第3个命令，设置Y的坐标--//
		//--Y轴有64个，一个坐标8位，也就是有8个坐标--//
		//所以一般我们使用的也就是从0xB0到0x07,就够了--//	
		LcdSt7565_Write(0,0,0xB0+i); 

		//--表格第4个命令，设置X坐标--//
		//--当你的段初始化为0xA1时，X坐标从0x10,0x04到0x18,0x04,一共128位--//
		//--当你的段初始化为0xA0时，X坐标从0x10,0x00到0x18,0x00,一共128位--//
		//--在写入数据之后X坐标的坐标是会自动加1的，我们初始化使用0xA0所以--//
		//--我们的X坐标从0x10,0x00开始---//
		LcdSt7565_Write(0,0,0x10); 
		LcdSt7565_Write(0,0,0x00);							   
		
		//--X轴有128位，就一共刷128次，X坐标会自动加1，所以我们不用再设置坐标--//
		for(j=0; j<128; j++)
		{
			LcdSt7565_Write(1,0,temp);  //如果设置背景为白色时，清屏选择0XFF
		}
	}
}

/*******************************************************************************
* 函 数 名         : LCD12864_Write16CnCHAR
* 函数功能		   : 在12864上面书写16X16的汉字
* 输    入         : x, y, cn
* 输    出         : 无
* 说    明		   : 该函数可以直接书写字符串，但是书写是从右到左，所以输入的时
*                  * 侯注意输入。汉字的取模方式请观看文件夹下的《汉字取模软件的
*                  * 使用说明》
*******************************************************************************/

#ifdef  CHAR_CODE

#include"charcode.h"


uchar Lcd12864_Write16CnCHAR(uchar x, uchar y, at,uchar *cn)
{
	uchar i, j, x1, x2, wordNum;
	//--Y的坐标只能从0到7，大于则直接返回--//
	if(y > 7)
	{
		return 0;
	}

	//--X的坐标只能从0到128，大于则直接返回--//
	if(x > 128)
	{
		return 0;
	}
	y += 0xB0;	   //求取Y坐标的值
	//--设置Y坐标--//
	LcdSt7565_Write(0,0,y);
	while ( *cn != '\0')	 //在C语言中字符串结束以‘\0’结尾
	{	
	
		//--设置Y坐标--//
		LcdSt7565_Write(0,0,y);

		x1 = (x >> 4) & 0x0F;   //由于X坐标要两句命令，分高低4位，所以这里先取出高4位
		x2 = x & 0x0F;          //去低四位
		//--设置X坐标--//
		LcdSt7565_Write(0,0,0x10 + x1);   //高4位
		LcdSt7565_Write(0,0,0x00 + x2);	//低4位

		//--查询要写的字在字库中的位置--//
		if(*cn < 126)//字母数字
		{
			for(i = 32;i < 126;i++)if(i == *cn)break;
			for(j=0;j<16;j++)
			{
				if(j == 8) 
				{
					//--设置Y坐标--//
		   			LcdSt7565_Write(0,0,y + 1);
					//--设置X坐标--//
					LcdSt7565_Write(0,0,0x10 + x1);   //高4位
					LcdSt7565_Write(0,0,0x00 + x2);	//低4位
				}
				LcdSt7565_Write(1,at,nAsciiDot[(i-32)*16+j]);
			}
			x += 8;cn +=1;
		}
		else 
		{
			for (wordNum=0; wordNum<50; wordNum++)
			{
				if ((CN16CHAR[wordNum].Index[0] == *cn) 
						&&(CN16CHAR[wordNum].Index[1] == *(cn+1)))
				{
					for (j=0; j<32; j++) //写一个字
					{		
						if (j == 16)	 //由于16X16用到两个Y坐标，当大于等于16时，切换坐标
						{
							//--设置Y坐标--//
				   			LcdSt7565_Write(0,0,y + 1);
				
							//--设置X坐标--//
							LcdSt7565_Write(0,0,0x10 + x1);   //高4位
							LcdSt7565_Write(0,0,0x00 + x2);	//低4位
						}
						LcdSt7565_Write(1,at,CN16CHAR[wordNum].Msk[j]);
					}
					x += 16;
				}//if查到字结束		
			} //for查字结束	
			cn += 2;			
		}
	}	//while结束
	return 1;
}

uchar lcd_putnumstr(uchar x,y,at,uint num)
{
	uchar i = 0, j = 0, temp = 0;
	uchar x1 = 0,x2 = 0;
	uchar table[4] = 0;
	if(num >= 0 && num < 10)temp=3;
	else if(num >= 10 && num < 100)temp=2;
	else if(num >= 100 && num < 1000)temp=1;
	else if(num >= 1000 && num < 10000)temp=0;
	table[0] = num/1000;
	table[1] = num%1000/100;
	table[2] = num%1000%100/10;
	table[3] = num%1000%100%10;

	//--Y的坐标只能从0到7，大于则直接返回--//
	if(y > 7){
		return 0;
	}
	//--X的坐标只能从0到128，大于则直接返回--//
	if(x > 128){
		return 0;
	}
	y += 0xB0;	   //求取Y坐标的值
	//--设置Y坐标--//
	LcdSt7565_Write(0,0,y);
	for(i = temp;i < 4;i++)
	{
		//--设置Y坐标--//
		LcdSt7565_Write(0,0,y);
		x1 = (x >> 4) & 0x0F;   //由于X坐标要两句命令，分高低4位，所以这里先取出高4位
		x2 = x & 0x0F;          //去低四位
		//--设置X坐标--//
		LcdSt7565_Write(0,0,0x10 + x1);   //高4位
		LcdSt7565_Write(0,0,0x00 + x2);	//低4位

		for(j=0;j<16;j++)
			{
				if(j == 8) 
				{
					//--设置Y坐标--//
		   			LcdSt7565_Write(0,0,y + 1);
					//--设置X坐标--//
					LcdSt7565_Write(0,0,0x10 + x1);   //高4位
					LcdSt7565_Write(0,0,0x00 + x2);	//低4位
				}
				LcdSt7565_Write(1,at,nAsciiDot[(table[i]+16)*16+j]);
			}
		y+=8;
	}
	return 1;
}



#endif

