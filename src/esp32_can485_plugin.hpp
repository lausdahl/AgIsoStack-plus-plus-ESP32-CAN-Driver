//================================================================================================
/// @file esp32_can485_plugin.hpp
///
/// @brief A driver for using the LilyGo T-CAN485 with the stack.
/// @author Kenneth Lausdahl
///
/// @copyright 2024 Kenneth Lausdahl
//================================================================================================
#ifndef ESP32_CAN485_PLUGIN_HPP
#define ESP32_CAN485_PLUGIN_HPP

#include "isobus/hardware_integration/can_hardware_plugin.hpp"
#include "isobus/isobus/can_hardware_abstraction.hpp"
#include "isobus/isobus/can_message_frame.hpp"

#include <ESP32CAN.h>
#include <CAN_config.h>

#include <string>

namespace isobus
{
	//================================================================================================
	/// @class Esp32CAN485Plugin
	///
	/// @brief A for the LilyGo T-CAN485 board.
	//================================================================================================
	class Esp32CAN485Plugin : public CANHardwarePlugin
	{
	public:
		/// @brief Constructor for the socket CAN driver
		explicit Esp32CAN485Plugin(const CAN_device_t *CAN_cfg);
		explicit Esp32CAN485Plugin();

		/// @brief The destructor for TWAIPlugin
		virtual ~Esp32CAN485Plugin();

		/// @brief Returns if the socket connection is valid
		/// @returns `true` if connected, `false` if not connected
		bool get_is_valid() const override;

		/// @brief Closes the socket
		void close() override;

		/// @brief Connects to the socket
		void open() override;

		/// @brief Returns a frame from the hardware (synchronous), or `false` if no frame can be read.
		/// @param[in, out] canFrame The CAN frame that was read
		/// @returns `true` if a CAN frame was read, otherwise `false`
		bool read_frame(isobus::CANMessageFrame &canFrame) override;

		/// @brief Writes a frame to the bus (synchronous)
		/// @param[in] canFrame The frame to write to the bus
		/// @returns `true` if the frame was written, otherwise `false`
		bool write_frame(const isobus::CANMessageFrame &canFrame) override;

	
	};
}
#endif // ESP32_CAN485_PLUGIN_HPP
