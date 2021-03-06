/**
 * Author: Anne van Rossum
 * Copyright: Distributed Organisms B.V. (DoBots)
 * Date: 20 Jan., 2015
 * License: LGPLv3+, Apache, or MIT, your choice
 */

#include <protocol/cs_Mesh.h>
#include <protocol/led_config.h>
#include <protocol/rbc_mesh_common.h>

#include <common/cs_Boards.h>

#include <drivers/cs_Serial.h>
#include <protocol/cs_MeshControl.h>
#include <util/cs_BleError.h>

extern "C" {

/**
 * Event handler on receiving a message from 
 */
void rbc_mesh_event_handler(rbc_mesh_event_t* evt)                  
{                              
	TICK_PIN(28);                          
	//nrf_gpio_pin_toggle(PIN_GPIO_LED1);
	switch (evt->event_type)                       
	{                             
		case RBC_MESH_EVENT_TYPE_CONFLICTING_VAL:                  
		case RBC_MESH_EVENT_TYPE_NEW_VAL:                    
		case RBC_MESH_EVENT_TYPE_UPDATE_VAL:                   

			if (evt->value_handle > 2)                     
				break; 
			//if (evt->data[0]) {
			LOGi("Got data in: %i, %i", evt->value_handle, evt->data[0]);
			MeshControl &meshControl = MeshControl::getInstance();
			meshControl.process(evt->value_handle, evt->data[0]);
			//} 
			led_config(evt->value_handle, evt->data[0]); 
			break; 
	}                             
}

}

CMesh::CMesh() {
}

CMesh::~CMesh() {
}

/**
 * Function to test mesh functionality. We have to figure out if we have to enable the radio first, and that kind of
 * thing.
 */
void CMesh::init() {
	LOGi("For now we are testing the meshing functionality.");
	nrf_gpio_pin_clear(PIN_GPIO_LED0);

	rbc_mesh_init_params_t init_params;

	init_params.access_addr = 0xA541A68F;
	init_params.adv_int_ms = 100;
	init_params.channel = 38;
	init_params.handle_count = 2;
	init_params.packet_format = RBC_MESH_PACKET_FORMAT_ORIGINAL;
	init_params.radio_mode = RBC_MESH_RADIO_MODE_BLE_1MBIT;

	LOGi("Call rbc_mesh_init");
	uint8_t error_code;
	// checks if softdevice is enabled etc.
	error_code = rbc_mesh_init(init_params);
	APP_ERROR_CHECK(error_code);

	LOGi("Call rbc_mesh_value_enable. Todo. Do this on send() and receive()!");
	error_code = rbc_mesh_value_enable(1);
	APP_ERROR_CHECK(error_code);
	error_code = rbc_mesh_value_enable(2);
	APP_ERROR_CHECK(error_code);
}

void CMesh::send(uint8_t handle, uint32_t value) {
	uint8_t val[28];
	val[0] = (uint8_t)value;
	LOGi("Set mesh data %i to %i", val[0], handle);
	APP_ERROR_CHECK(rbc_mesh_value_set(handle, &val[0], 1));
}

// returns last received message
uint32_t CMesh::receive(uint8_t handle) {
	uint8_t val[28];                       
	uint16_t len;                        
	APP_ERROR_CHECK(rbc_mesh_value_get(handle, val, &len, NULL));
	if (!len) return 0;
	return (uint32_t)val[0];
}

// set callback to receive message
void CMesh::set_callback() {
}



