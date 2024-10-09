#include "WaterEnvironment.hpp"

WaterEnvironment::WaterEnvironment(uint8_t motor_system_switch, uint8_t water_sensor_pin)
    : motor_system_switch(motor_system_switch), water_sensor_pin(water_sensor_pin) {
    
    pinMode(motor_system_switch, INPUT);
}

bool WaterEnvironment::isSystemOn() {
    return digitalRead(motor_system_switch) == HIGH;
}

int WaterEnvironment::getWaterLevel() {
    return analogRead(water_sensor_pin);
}