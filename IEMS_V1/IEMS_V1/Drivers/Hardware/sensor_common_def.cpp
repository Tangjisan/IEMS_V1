#include "sensor_common_def.h"

// // 初始化ITM，启用SWO输出
// void SWO_Init(void) {
//     // 1. 启用内核调试跟踪（DEMCR寄存器）
//     CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

//     // 2. 配置ITM全局控制寄存器（TCR）
//     ITM->TCR = 0;  // 先清零
//     ITM->TCR |= ITM_TCR_ITMENA_Msk;       // 启用ITM模块
//     ITM->TCR |= ITM_TCR_SWOENA_Msk;       // 启用SWO引脚
//     ITM->TCR |= (0x00UL << ITM_TCR_TraceBusID_Pos);  // 设置总线ID=0

//     // 3. 配置特权访问（允许所有权限访问ITM端口0）
//     ITM->TPR = 0x00000000;  // PRIVMASK=0，允许所有特权级访问

//     // 4. 启用ITM端口0（用于输出printf）
//     ITM->TER = 0x00000001;  // 仅启用端口0（bit0置1）
// }
