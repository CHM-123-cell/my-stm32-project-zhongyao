/**
 * OLED SSD1306 0.96寸 I2C 驱动 (SPL风格, 软件I2C)
 * PB8 = SCL, PB9 = SDA
 * 基于 OLED驱动函数模块/4针脚I2C版本 适配
 */

#include "stm32f10x.h"
#include "OLED_Font.h"

/* ========== 引脚宏定义 ========== */
#define OLED_W_SCL(x)  GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)  GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

/**
 * @brief  I2C 引脚初始化
 */
static void OLED_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* PB8(SCL) PB9(SDA) 开漏输出 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/**
 * @brief  I2C 起始信号
 */
static void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

/**
 * @brief  I2C 停止信号
 */
static void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/**
 * @brief  I2C 发送一个字节
 */
static void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1);  /* 额外时钟，不处理应答 */
    OLED_W_SCL(0);
}

/**
 * @brief  OLED 写命令
 */
static void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);   /* 从机地址 0x3C << 1 */
    OLED_I2C_SendByte(0x00);   /* 控制字节：命令 */
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

/**
 * @brief  OLED 写数据
 */
static void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);   /* 从机地址 0x3C << 1 */
    OLED_I2C_SendByte(0x40);   /* 控制字节：数据 */
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/**
 * @brief  OLED 设置光标位置
 * @param  Y: 页地址 0~7
 * @param  X: 列地址 0~127
 */
static void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                      /* 设置页地址 */
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));      /* 列地址高4位 */
    OLED_WriteCommand(0x00 | (X & 0x0F));              /* 列地址低4位 */
}

/**
 * @brief  OLED 清屏
 */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
 * @brief  OLED 显示一个 8x16 字符
 * @param  Line:   行号 1~4
 * @param  Column: 列号 1~16
 * @param  Char:   要显示的 ASCII 字符
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;

    /* 设置光标到上半部分 (8像素) */
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);
    }

    /* 设置光标到下半部分 (8像素) */
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
    }
}

/**
 * @brief  OLED 显示字符串
 * @param  Line:   起始行 1~4
 * @param  Column: 起始列 1~16
 * @param  String: 字符串 (以 \0 结尾)
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
 * @brief  次方函数 (内部使用)
 */
static uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
 * @brief  OLED 显示十进制正数
 * @param  Line:   行号 1~4
 * @param  Column: 起始列 1~16
 * @param  Number: 数字 0~4294967295
 * @param  Length: 显示位数 1~10
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED 显示带符号十进制数
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED 初始化 (SSD1306)
 */
void OLED_Init(void)
{
    uint32_t i, j;

    /* 上电稳定延时 */
    for (i = 0; i < 1000; i++)
    {
        for (j = 0; j < 1000; j++);
    }

    OLED_I2C_Init();

    OLED_WriteCommand(0xAE);  /* 关闭显示 */

    OLED_WriteCommand(0xD5);  /* 时钟分频比/振荡器频率 */
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);  /* 多路复用率 64 */
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);  /* 显示偏移 */
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);  /* 显示起始行 */

    OLED_WriteCommand(0xA1);  /* 段重映射：左右正常 */

    OLED_WriteCommand(0xC8);  /* COM扫描方向：上下正常 */

    OLED_WriteCommand(0xDA);  /* COM引脚配置 */
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);  /* 对比度 */
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);  /* 预充电周期 */
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);  /* VCOMH 电压 */
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);  /* 正常显示（非全屏点亮） */

    OLED_WriteCommand(0xA6);  /* 正常显示（非反色） */

    OLED_WriteCommand(0x8D);  /* 电荷泵使能 */
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);  /* 开启显示 */

    OLED_Clear();             /* 清屏 */
}
