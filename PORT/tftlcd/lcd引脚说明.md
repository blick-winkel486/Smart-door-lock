

480 800


| 引脚名称 | 端口 | 引脚号 | 作用 | 配置 |
|---------|------|--------|------|------|
| **TFT_RST** | GPIOE | PIN_2 | **复位信号** - 硬件复位LCD控制器，低电平有效 | 推挽输出，高速(GPIO_SPEED_FREQ_HIGH) |
| **TFT_RS** | GPIOG | PIN_0 | **寄存器选择** - 0:命令寄存器, 1:数据寄存器 | 推挽输出，高速(GPIO_SPEED_FREQ_HIGH) |
| **TFT_CS** | GPIOG | PIN_12 | **片选信号** - 低电平选中LCD，高电平禁用 | 推挽输出，高速(GPIO_SPEED_FREQ_HIGH) |
| **TFT_RD** | GPIOD | PIN_4 | **读使能** - 低电平有效，读取LCD数据 | 推挽输出，高速(GPIO_SPEED_FREQ_HIGH) |
| **TFT_WR** | GPIOD | PIN_5 | **写使能** - 低电平有效，写入数据到LCD | 推挽输出，高速(GPIO_SPEED_FREQ_HIGH) |



| 引脚名称 | 端口 | 引脚号 | 数据位 | 初始状态 | 配置 |
|---------|------|--------|--------|----------|------|
| **TFT_D0** | GPIOD | PIN_14 | 数据位0 | GPIO_PIN_SET | 推挽输出，中速 |
| **TFT_D1** | GPIOD | PIN_15 | 数据位1 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D2** | GPIOD | PIN_0 | 数据位2 | GPIO_PIN_SET | 推挽输出，中速 |
| **TFT_D3** | GPIOD | PIN_1 | 数据位3 | 未设置初始状态 | 推挽输出，中速 |
| **TFT_D4** | GPIOE | PIN_7 | 数据位4 | GPIO_PIN_SET | 推挽输出，中速 |
| **TFT_D5** | GPIOE | PIN_8 | 数据位5 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D6** | GPIOE | PIN_9 | 数据位6 | GPIO_PIN_SET | 推挽输出，中速 |
| **TFT_D7** | GPIOE | PIN_10 | 数据位7 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D8** | GPIOE | PIN_11 | 数据位8 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D9** | GPIOE | PIN_12 | 数据位9 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D10** | GPIOE | PIN_13 | 数据位10 | 未设置初始状态 | 推挽输出，中速 |
| **TFT_D11** | GPIOE | PIN_14 | 数据位11 | 未设置初始状态 | 推挽输出，中速 |
| **TFT_D12** | GPIOE | PIN_15 | 数据位12 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D13** | GPIOD | PIN_8 | 数据位13 | GPIO_PIN_RESET | 推挽输出，中速 |
| **TFT_D14** | GPIOD | PIN_9 | 数据位14 | 未设置初始状态 | 推挽输出，中速 |
| **TFT_D15** | GPIOD | PIN_10 | 数据位15 | 未设置初始状态 | 推挽输出，中速 |


**总计**: 5个控制信号 + 16个数据信号 = 21个LCD专用引脚