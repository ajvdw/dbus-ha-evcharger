import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_KILOWATT_HOURS,
)
from . import EVBox, evbox_ns

CONF_EVBOX_ID = "evbox_id"
CONF_L1_CURRENT = "l1_current"
CONF_L2_CURRENT = "l2_current"
CONF_L3_CURRENT = "l3_current"
CONF_TOTAL_ENERGY = "total_energy"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_EVBOX_ID): cv.use_id(EVBox),
        cv.Optional(CONF_L1_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_L2_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_L3_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_ENERGY): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOWATT_HOURS,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_EVBOX_ID])

    if CONF_L1_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_L1_CURRENT])
        cg.add(parent.set_l1_current_sensor(sens))

    if CONF_L2_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_L2_CURRENT])
        cg.add(parent.set_l2_current_sensor(sens))

    if CONF_L3_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_L3_CURRENT])
        cg.add(parent.set_l3_current_sensor(sens))

    if CONF_TOTAL_ENERGY in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL_ENERGY])
        cg.add(parent.set_total_energy_sensor(sens))
