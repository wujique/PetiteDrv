
音频的功能主要是：通过cpu的I2S等总线，将声音数据传输到codec。

有用SPI接口播放声音的芯片。

因此，驱动分两部分：芯片I2S（SAI）驱动， 音频芯片（CODEC）驱动。

一个原则，codec驱动能适配不同芯片，无论是STM32还是RT1052，都用同一份代码。
平台相关的代码放到mcu文件夹中。

抽象，音频就是数据流，只要把数据流往音频管道中填充，就能播放声音。

参考linux ASOC 和乐鑫ADF机构

ST的例子，codec提供的函数有：
WM8994_Drv_t WM8994_Driver = 
{
  WM8994_Init,
  WM8994_DeInit,
  WM8994_ReadID,
  WM8994_Play,
  WM8994_Pause,
  WM8994_Resume,
  WM8994_Stop,
  WM8994_SetFrequency,
  WM8994_GetFrequency,  
  WM8994_SetVolume,
  WM8994_GetVolume,  
  WM8994_SetMute,
  WM8994_SetOutputMode,
  WM8994_SetResolution,
  WM8994_GetResolution,  
  WM8994_SetProtocol,
  WM8994_GetProtocol,  
  WM8994_Reset
};

ESP ADF codec接口


## ASOC的软件框架
        对应着硬件框架，ASOC软件也分为三部分： 板载硬件（Machine）、Soc（Platform）、Codec  。他们的具体内容如下：

1、  Codec驱动：ASOC一个重要设计原则就是要求Codec驱动是平台无关的，它包含了一些音频的控件（Controls），音频接口，DAMP（动态音频电源管理）的定义和某些Codec IO功能。为了保证硬件无关性，任何特定于平台和机器的代码都要移到Platform和Machine驱动中。

        必要时，也可以提供以下功能：

        DAPM描述信息；

        DAPM事件处理程序；

        DAC数字静音控制；

2、Platform驱动 ： 它包含了该SoC平台的音频DMA和音频接口的配置和控制（I2S，PCM，AC97等等）；它也不能包含任何与板子或机器相关的代码。

3、Machine驱动：  Machine驱动负责处理机器特有的一些控件和音频事件（例如，当播放音频时，需要先行打开一个放大器）；单独的Platform和Codec驱动是不能工作的，它必须由Machine驱动把它们结合在一起才能完成整个设备的音频处理工作。
