import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import CONF_ID, UNIT_AMPERE
from . import EVBox, evbox_ns

CONF_EVBOX_ID = "evbox_id"

EVBoxNumber = evbox_ns.class_("EVBoxNumber", number.Number, cg.Component)

CONFIG_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(EVBoxNumber),
        cv.GenerateID(CONF_EVBOX_ID): cv.use_id(EVBox),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await number.register_number(var, config, min_value=9, max_value=32, step=1)

    parent = await cg.get_variable(config[CONF_EVBOX_ID])
    cg.add(var.set_evbox_parent(parent))
