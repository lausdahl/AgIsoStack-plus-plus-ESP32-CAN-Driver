//================================================================================================
/// @file esp32_can485_plugin.cpp
///
/// @brief A driver for using the LilyGo T-CAN485 with the stack.
/// @author Kenneth Lausdahl
///
/// @copyright 2024 Kenneth Lausdahl
//================================================================================================
#include "esp32_can485_plugin.hpp"
#include "isobus/isobus/can_constants.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/utility/system_timing.hpp"
#include "isobus/utility/to_string.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>
#include "driver/gpio.h"
CAN_device_t CAN_cfg;
const int rx_queue_size = 10;
namespace isobus
{

    Esp32CAN485Plugin::Esp32CAN485Plugin(const CAN_device_t *can_cfg)
    {
        CAN_cfg = *can_cfg;
    }

     Esp32CAN485Plugin::Esp32CAN485Plugin()
    {
        CAN_cfg.speed = CAN_SPEED_250KBPS;
    }

    Esp32CAN485Plugin::~Esp32CAN485Plugin()
    {
        close();
    }

    bool Esp32CAN485Plugin::get_is_valid() const
    {
        return true;
    }

    void Esp32CAN485Plugin::close()
    {
        ESP32Can.CANStop();
    }

    void Esp32CAN485Plugin::open()
    {

        gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
        gpio_pulldown_dis(GPIO_NUM_23);

        CAN_cfg.tx_pin_id = GPIO_NUM_27;
        CAN_cfg.rx_pin_id = GPIO_NUM_26;
        CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
        // Init CAN Module
        ESP32Can.CANInit();
    }

    bool Esp32CAN485Plugin::read_frame(isobus::CANMessageFrame &canFrame)
    {
        bool retVal = false;
        CAN_frame_t rx_frame;

        if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, pdMS_TO_TICKS(100)) == pdTRUE)
        {

            if (!rx_frame.FIR.B.RTR)
            {
                canFrame.identifier = rx_frame.MsgID;
                bool ide = rx_frame.FIR.B.FF != CAN_frame_std;
                canFrame.isExtendedFrame = ide;
                canFrame.dataLength = rx_frame.FIR.B.DLC;
                if (isobus::CAN_DATA_LENGTH >= canFrame.dataLength)
                {
                    memset(canFrame.data, 0, sizeof(canFrame.data));
                    memcpy(canFrame.data, rx_frame.data.u8, canFrame.dataLength);
                    retVal = true;
                }

                else
                {
                    LOG_ERROR("[ESP32CAN485] Error receiving message: no message");
                }
            }
        }

        return retVal;
    }

    bool Esp32CAN485Plugin::write_frame(const isobus::CANMessageFrame &canFrame)
    {
        bool retVal = false;
        CAN_frame_t tx_frame;

        tx_frame.FIR.B.FF = canFrame.isExtendedFrame ? CAN_frame_ext : CAN_frame_std;
        tx_frame.MsgID = canFrame.identifier;
        tx_frame.FIR.B.DLC = canFrame.dataLength;

        memcpy(tx_frame.data.u8, canFrame.data, canFrame.dataLength);

        int error = ESP32Can.CANWriteFrame(&tx_frame);
        if (0 == error)
        {
            retVal = true;
        }
        else
        {
            LOG_ERROR("[ESP32CAN485] Error sending message: ");
        }
        return retVal;
    }
}
