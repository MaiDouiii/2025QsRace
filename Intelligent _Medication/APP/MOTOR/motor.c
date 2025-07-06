#include "motor.h"

uint8_t motor_stop_flag = 0;
uint8_t motorBottom_turn = 0, motorTop_turn = 0;
uint8_t zero_motor1_speed_buffer[] = {0x01, 0x35, 0x00, 0x00, 0x00, 0x6B};
uint8_t zero_motor2_speed_buffer[] = {0x02, 0x35, 0x00, 0x00, 0x00, 0x6B};
/**
 * @brief    使能信号控制
 * @param    addr  ：电机地址
 * @param    state ：使能状态     ，1为使能电机，0为关闭电机
 * @param    snF   ：多机同步标志 ，0为不启用，1为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void motor_En_Control(uint8_t addr, uint8_t state, uint8_t snF)
{
  uint8_t cmd[16] = {0};

  // 装载命令
  cmd[0] = addr;           // 地址
  cmd[1] = 0xF3;           // 功能码
  cmd[2] = 0xAB;           // 辅助码
  cmd[3] = (uint8_t)state; // 使能状态
  cmd[4] = snF;            // 多机同步运动标志
  cmd[5] = 0x6B;           // 校验字节

  // 发送命令
  HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 6, 10);
}

/**
 * @brief    位置模式
 * @param    addr  ：电机地址
 * @param    dir   ：方向        ，0为CW顺时针（1上下向上)(2靠近电机)，其余值为CCW逆时针（1向下）(2远离电机)
 * @param    speed ：速度(RPM)   ，范围0 - 5000RPM
 * @param    acc   ：加速度      ，范围0 - 255，注意：0是直接启动
 * @param    clk   ：脉冲数      ，范围0- (2^32 - 1)个
 * @param    mode  ：相位/绝对标志，0为相对运动，1为绝对值运动
 * @param   tongbu ：多机同步标志 ，0为不启用，1为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void motor_pos_control(uint8_t addr, uint8_t dir, uint16_t speed, uint8_t acceleration, uint32_t clk, uint8_t mode, uint8_t duoji_tongbu)
{
  uint8_t cmd[16] = {'\0'};
  cmd[0] = addr;
  cmd[1] = 0xFD;
  cmd[2] = dir;
  cmd[3] = (uint8_t)(speed >> 8);
  cmd[4] = (uint8_t)(speed);
  cmd[5] = acceleration;
  cmd[6] = (uint8_t)(clk >> 24);
  cmd[7] = (uint8_t)(clk >> 16);
  cmd[8] = (uint8_t)(clk >> 8);
  cmd[9] = (uint8_t)(clk);
  cmd[10] = mode;
  cmd[11] = duoji_tongbu;
  cmd[12] = 0X6B;
  if (addr == 1)
  {
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 13, 100);
  }
  else if (addr == 2)
  {
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 13, 100);
  }
  //HAL_Delay(10);
}

/**
 * @brief    立即停止（所有控制模式都通用）
 * @param    addr  ：电机地址
 * @param    snF   ：多机同步标志，0为不启用，1为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void motor_Stop_Now(uint8_t addr, uint8_t snF)
{
  uint8_t cmd[16] = {'\0'};

  // 装载命令
  cmd[0] = addr; // 地址
  cmd[1] = 0xFE; // 功能码
  cmd[2] = 0x98; // 辅助码
  cmd[3] = snF;  // 多机同步运动标志
  cmd[4] = 0x6B; // 校验字节

  // 发送命令
  HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 5, 10);
}

/**
 * @brief    读取转速（所有控制模式都通用）
 * @param    addr  ：电机地址
 * @return   在串口接收状态:addr 35 y x1 x2 6B  -- y为正负(1为负，0为正) X1 X2为转速    错误指令为01 00 EE
 */
void motor_get_speed(uint8_t addr)
{
  uint8_t cmd[16] = {'\0'};
  cmd[0] = addr; // 地址
  cmd[1] = 0x35;
  cmd[2] = 0x6B;
	
  HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 3, 10);
	vTaskDelay(150);
}

/**
 * @brief    校准（所有控制模式都通用）
 * @param    addr  ：电机地址
 * @return   正确返回addr 06 02 6B   条件不满足 01 06 E2 6B 错误命令返回 01 00 EE 6B
 */
void motor_calibration(uint8_t addr)
{
  uint8_t cmd[16] = {'\0'};
  cmd[0] = addr; // 地址
  cmd[1] = 0x06;
  cmd[2] = 0x45;
  cmd[3] = 0x6B;
  HAL_UART_Transmit(&huart2, (uint8_t *)cmd, 4, 10);
}

void motor_init()
{
  if (HAL_GPIO_ReadPin(TOP_UP_GPIO_Port, TOP_UP_Pin) == GPIO_PIN_SET)
  {
    motor_pos_control(motor_top, motor_top_up, 1200, 0, 96000, 0, 0);      // 滑台上移
    while (HAL_GPIO_ReadPin(TOP_UP_GPIO_Port, TOP_UP_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
    {
      HAL_Delay(2);
    }
    motor_Stop_Now(motor_top, 0);
    HAL_Delay(2); // 延时2ms
  }

  if (HAL_GPIO_ReadPin(BOTTOM_BACK_GPIO_Port, BOTTOM_BACK_Pin) == GPIO_PIN_SET)
  {
    motor_pos_control(motor_bottom, motor_bottom_back, 1200, 0, 96000, 0, 0);        // 滑台后移
    while (HAL_GPIO_ReadPin(BOTTOM_BACK_GPIO_Port, BOTTOM_BACK_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
    {
      HAL_Delay(2);
    }
    motor_Stop_Now(motor_bottom, 0);
  }
  HAL_Delay(2);
}

void motor_take_control()
{
	uint8_t time_check = 0;
  //motor_pos_control(motor_top, motor_top_down, 800, 0, 45000, 0, 0);         // 滑台下移
	motor_pos_control(motor_top, motor_top_down, 800, 0, 43000, 0, 0);         // 滑台下移
	HAL_Delay(200);
	while(motor_stop_flag == 0)
	{
		motor_get_speed(motor_top);
		if(++time_check >= 10) // 150*10=1500ms 超时,出现故障
		{
			time_check = 0;
			memset(uart_buffer_2,0,sizeof(uart_buffer_2));
			break;
		}
	}
	time_check = 0;
	motor_stop_flag = 0;
  HAL_Delay(1);
  HAL_GPIO_WritePin(down_en_GPIO_Port, down_en_Pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET); // 继电器吸合

  vTaskDelay(700); // 延时700ms

  motor_pos_control(motor_top, motor_top_up, 800, 0, 96000, 0, 0);       // 滑台上移
	HAL_Delay(200);
  while (HAL_GPIO_ReadPin(TOP_UP_GPIO_Port, TOP_UP_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
  {
    HAL_Delay(5);
  }
  motor_Stop_Now(motor_top, 0);
  vTaskDelay(100);

  motor_pos_control(motor_bottom, motor_bottom_forward, 800, 0, 46000, 0, 0);            // 滑台左移动
	HAL_Delay(200);
	while(motor_stop_flag == 0)
	{
    motor_get_speed(motor_bottom);
		if(++time_check >= 13) 
		{
			time_check = 0;
			memset(uart_buffer_2,0,sizeof(uart_buffer_2));
			break;
		}
  }
	time_check = 0;
	motor_stop_flag = 0;
  HAL_Delay(50);

  motor_pos_control(motor_top, motor_top_down, 800, 0, 36000, 0, 0);         // 滑台下移
	HAL_Delay(200);
	while(motor_stop_flag == 0)
	{
		motor_get_speed(motor_top);
		if(++time_check >= 13) 
		{
			time_check = 0;
			memset(uart_buffer_2,0,sizeof(uart_buffer_2));
			break;
		}
	}
	time_check = 0;
	motor_stop_flag = 0;
  HAL_Delay(50);
  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET); // 继电器断开
  HAL_Delay(10);
  motor_init(); // 复位电机
  HAL_Delay(100);
	HAL_GPIO_WritePin(down_en_GPIO_Port, down_en_Pin, GPIO_PIN_RESET);
}

//void motor_take_control()
//{
//  motor_pos_control(motor_top, motor_top_down, 800, 0, 96000, 0, 0);         // 滑台下移
//  while (HAL_GPIO_ReadPin(TOP_DOWN_GPIO_Port, TOP_DOWN_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
//  {
//    HAL_Delay(1);
//  }
//  motor_Stop_Now(motor_top, 0);
//  HAL_Delay(1);
//  HAL_GPIO_WritePin(down_en_GPIO_Port, down_en_Pin, GPIO_PIN_SET);
//  HAL_Delay(1);
//  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET); // 继电器吸合

//  vTaskDelay(700); // 延时700ms

//  motor_pos_control(motor_top, motor_top_up, 800, 0, 96000, 0, 0);       // 滑台上移
//  while (HAL_GPIO_ReadPin(TOP_UP_GPIO_Port, TOP_UP_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
//  {
//    HAL_Delay(1);
//  }
//  motor_Stop_Now(motor_top, 0);
//  HAL_Delay(2); // 延时2ms

//  motor_pos_control(motor_bottom, motor_bottom_forward, 800, 0, 96000, 0, 0);            // 滑台左移动
//  while (HAL_GPIO_ReadPin(BOTTOM_FORWARD_GPIO_Port, BOTTOM_FORWARD_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
//  {
//    HAL_Delay(1);
//  }
//  motor_Stop_Now(motor_bottom, 0);
//  HAL_Delay(2); // 延时2ms

//  motor_pos_control(motor_top, motor_top_down, 800, 0, 96000, 0, 0);         // 滑台下移
//  while (HAL_GPIO_ReadPin(TOP_DOWN_GPIO_Port, TOP_DOWN_Pin) == GPIO_PIN_SET) // 等待对管检测是否到达指定位置
//  {
//    HAL_Delay(1);
//  }
//  motor_Stop_Now(motor_top, 0);
//  HAL_Delay(1);
//  HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET); // 继电器断开
//  HAL_Delay(1);
//  motor_init(); // 复位电机
//  HAL_Delay(50);
//}

int abs(int l)
{
  l = l >= 0 ? l : -l;
  return l;
}

uint16_t Pulse_count = 0;
int error = 0;
void motor_down_control(uint8_t dir, uint32_t num)
{
  uint8_t num2 = 0;
  HAL_GPIO_WritePin(down_en_GPIO_Port, down_en_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(down_dir_GPIO_Port, down_dir_Pin, (GPIO_PinState)dir);
  if (dir == 1)
  {
    error += num;
  }
  else if (dir == 0)
  {
    error -= num;
  }
  num2 = abs(error / 3);
  if (dir == 1)
  {
    error = error - 3 * num2;
  }
  else if (dir == 0)
  {
    error = error + 3 * num2;
  }
  Pulse_count = num * 533 + num2;
  TIM4->CCR1 = 500;
  // HAL_Delay(500);
}

void motor_down_en(uint8_t en)
{
  if (en == 1)
  {
    TIM4->CCR1 = 500;

    // HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_1);
  }
  else
  {
    TIM4->CCR1 = 0;
    // HAL_TIM_PWM_Stop_IT(&htim4, TIM_CHANNEL_1);
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4)
  {
    if (TIM4->CCR1 == 500)
    {
      Pulse_count--;
      if (Pulse_count == 0)
      {
        motor_down_en(0);
      }
    }
  }
}
