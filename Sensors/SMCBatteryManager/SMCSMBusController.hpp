//
//  SMCSMBusController.hpp
//  SMCBatteryManager
//
//  Copyright © 2018 usrsse2. All rights reserved.
//

#ifndef SMCSMBusController_hpp
#define SMCSMBusController_hpp

#include <Headers/kern_util.hpp>
#include <IOKit/IOService.h>
#include <IOKit/IOSMBusController.h>
#include <IOKit/battery/AppleSmartBatteryCommands.h>
#include "BatteryManager.hpp"

class EXPORT SMCSMBusController : public IOSMBusController {
	OSDeclareDefaultStructors(SMCSMBusController)

	/**
	 *  This is here to avoid collisions in case of future expansion of IOSMBusController class.
	 *  WARNING! It is important that this is the first member of the class!
	 */
	void *reserved[8] {};

	/**
	 *  Supported power states
	 */
	IOPMPowerState smbusPowerStates[2] {
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, kIOPMPowerOn, kIOPMPowerOn, kIOPMPowerOn, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	/**
	 Set received data for a transaction to a 16-bit integer value

	 @param transaction SMBus transaction
	 @param valueToWrite Value to write
	 */
	static void setReceiveData(IOSMBusTransaction *transaction, uint16_t valueToWrite);

	/**
	 *  SMBus battery words are 16 bit, while the battery state keeps 32-bit values. Passing them to the
	 *  uint16_t version truncated them: e.g. a time to empty of 240000 minutes on AC (rate 0) became 43392.
	 *  These clamp instead (0xFFFF is also the SMBus "unknown" value); signed currents clamp to int16.
	 */
	static void setReceiveData(IOSMBusTransaction *transaction, uint32_t valueToWrite) {
		setReceiveData(transaction, static_cast<uint16_t>(valueToWrite > UINT16_MAX ? UINT16_MAX : valueToWrite));
	}
	static void setReceiveData(IOSMBusTransaction *transaction, int32_t valueToWrite) {
		if (valueToWrite > INT16_MAX) valueToWrite = INT16_MAX;
		if (valueToWrite < INT16_MIN) valueToWrite = INT16_MIN;
		setReceiveData(transaction, static_cast<uint16_t>(static_cast<int16_t>(valueToWrite)));
	}

	/**
	 *  Sensible defaults for timer probing and rechecking the new commands.
	 *  Note: when it was 1000, after extending the code it failed to read all data in 10 seconds,
	 *  so AppleSmartBatteryManager aborted polling.
	 */
	static constexpr uint32_t TimerTimeoutMs = 100;

	/**
	 *  Delay before completing a queued request. The timer only runs while requests are queued, so
	 *  this is shorter than the old periodic TimerTimeoutMs; a full AppleSmartBatteryManager update
	 *  issues dozens of requests one after another.
	 */
	static constexpr uint32_t RequestCompletionDelayMs = 10;

	/**
	 *  The completion timer is pending. Only accessed on the controller work loop.
	 */
	bool timerArmed {false};

	/**
	 *  A workloop in charge of handling timer events with requests.
	 */
	IOWorkLoop *workLoop {nullptr};

	/**
	 *  Timer event source for processing requests.
	 */
	IOTimerEventSource *timer {nullptr};

	/**
	 *  Registered SMBus requests.
	 */
	OSArray *requestQueue {nullptr};

	/**
	 *  Create battery date in AppleSmartBattery format
	 *
	 *  @param day     manufacturing date
	 *  @param month   manufacturing month
	 *  @param year    manufacturing year
	 *
	 *  @return date in AppleSmartBattery format
	 */
	static constexpr uint16_t makeBatteryDate(uint16_t day, uint16_t month, uint16_t year) {
		return (day & 0x1FU) | ((month & 0xFU) << 5U) | (((year - 1980U) & 0x7FU) << 9U);
	}

public:
	/**
	 *  Perform initialisation.
	 *
	 *  @param properties  matching properties.
	 *
	 *  @return true on success.
	 */
	bool init(OSDictionary *properties) override;

	/**
	 *  Decide on target compatibility.
	 *
	 *  @param provider  parent IOService object
	 *  @param score     probing score
	 *
	 *  @return self if we could load anyhow.
	 */
	IOService *probe(IOService *provider, SInt32 *score) override;
	
	/**
	 *  Start the driver by setting up the workloop and preparing the matching.
	 *
	 *  @param provider  parent IOService object
	 *
	 *  @return true on success
	 */
	bool start(IOService *provider) override;
	
	/**
	 *  Dummy unload of the driver (currently no-op!).
	 *
	 *  @param provider  parent IOService object
	 */
	void stop(IOService *provider) override;

	/**
	 *  Handle external notification from ACPI battery instance
	 *
	 *  @param target  SMCSMBusController instance
	 *
	 *  @return kIOReturnSuccess
	 */
	static IOReturn handleACPINotification(void *target);
	
	/**
	 *  Handle notification about going to sleep or waking up
	 *
	 *  @param state   Power state (0 - sleep, 1 - wake)
	 *  @param device  SMCBatteryManager
	 *  @return kIOPMAckImplied
	 */
	IOReturn setPowerState(unsigned long state, IOService * device) override;
	
protected:
	/**
	 *  Returns the current work loop.
	 *  We should properly override this, as our provider has no work loop.
	 *
	 *  @return current workloop for request handling.
	 */
	IOWorkLoop *getWorkLoop() const override {
		return workLoop;
	}

	/**
	 *  Request handling routine used for handling battery commands.
	 *
	 *  @param request request to handle
	 *
	 *  @return request handling result.
	 */
	IOSMBusStatus startRequest(IOSMBusRequest *request) override;

	/**
	 *  Timer event used to handled queued AppleSmartBatteryManager SMBus requests.
	 */
	void handleBatteryCommandsEvent();
	
	/**
	 *  Holds value of batteriesConnected() when the previous notification was sent, must be guarded by stateLock
	 *
	 *  @return true on success
	 */
	bool prevBatteriesConnected {false};

	/**
	 *  Holds value of adaptersConnected() when the previous notification was sent, must be guarded by stateLock
	 *
	 *  @return true on success
	 */
	bool prevAdaptersConnected {false};
};

#endif /* SMCSMBusController_hpp */
