#include "legato.h"
#include "interfaces.h"


#define AIR_SENSOR_NAME "air/industrial_pm1_0/value"

#define AIR_SENSOR_DATA "/app/airSensorToDatahub/air/industrial_pm1_0/value"

#define AIR_SENSOR_OBS "airSensorOffLimits"

#define AIR_SENSOR_LOWER_LIMIT 0.0000
#define AIR_SENSOR_UPPER_LIMIT 15.0000

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the latitude value.
 */
//--------------------------------------------------------------------------------------------------
static void AirSensorUpdateHandler
(
	double timestamp,
	double value,
	void* contextPtr
)
{
	LE_DEBUG("air sensor PM: = %lf (timestamped %lf)", value, timestamp);
}


//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the led matrix value.
 */
//--------------------------------------------------------------------------------------------------
static void AirSensorObservationUpdateHandler
(
	double timestamp,	///< time stamp
	double value,		///< air sensor value,
	void* contextPtr	///< not used 
)
{
	LE_INFO("Observed filtered air sensor = %lf (timestamped %lf)", value, timestamp);	
}


COMPONENT_INIT
{
	le_result_t result;

	// This will be received from the Data Hub.
	result = io_CreateOutput(AIR_SENSOR_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
	LE_ASSERT(result == LE_OK);

	// Register for notification of updates to the counter value.
	io_AddNumericPushHandler(AIR_SENSOR_NAME, AirSensorUpdateHandler, NULL);

	// Connect to the sensor
	result = admin_SetSource("/app/airSensorDisplay/" AIR_SENSOR_NAME, AIR_SENSOR_DATA);
	LE_ASSERT(result == LE_OK);

	// Create observation (filter) for air sensor. To set up the "dead band" filter,
	// lower limit assigned to high limit and vice versa (see admin.io doc for details).
	admin_CreateObs(AIR_SENSOR_OBS);
	admin_SetLowLimit(AIR_SENSOR_OBS, AIR_SENSOR_LOWER_LIMIT);
	admin_SetHighLimit(AIR_SENSOR_OBS, AIR_SENSOR_UPPER_LIMIT);
	result = admin_SetSource("/obs/" AIR_SENSOR_OBS, AIR_SENSOR_DATA);
	LE_ASSERT(result == LE_OK);
	admin_AddNumericPushHandler("/obs/" AIR_SENSOR_OBS, AirSensorObservationUpdateHandler, NULL);

	LE_ASSERT(result == LE_OK);
}
