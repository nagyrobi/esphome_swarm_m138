# based on https://github.com/ssieb/custom_components/tree/master/components/uart_demo
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart, button, sensor, switch, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_STATE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_NONE,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_TIMESTAMP,
    DEVICE_CLASS_TEMPERATURE,
    ICON_BRIEFCASE_DOWNLOAD,
    ICON_WIFI,
    ICON_COUNTER,
    ICON_SIGNAL,
    UNIT_VOLT,
    UNIT_CELSIUS,
    UNIT_SECOND,
    UNIT_DECIBEL_MILLIWATT 
)

MULTI_CONF = True
DEPENDENCIES = ['uart']
AUTO_LOAD = ['button', 'sensor', 'switch', 'text_sensor']

swarm_m138_ns = cg.esphome_ns.namespace('swarm_m138')

SwarmModem = swarm_m138_ns.class_('SwarmModem', cg.PollingComponent, uart.UARTDevice)
SwarmModemMsgNotifSw = swarm_m138_ns.class_("SwarmModemMsgNotifSw", switch.Switch, cg.Component)
SwarmModemDelUnsentMsg = swarm_m138_ns.class_("SwarmModemDelUnsentMsg", button.Button, cg.Component)
SwarmModemDelReceivedMsg = swarm_m138_ns.class_("SwarmModemDelReceivedMsg", button.Button, cg.Component)
SwarmModemReadNewestMsg = swarm_m138_ns.class_("SwarmModemReadNewestMsg", button.Button, cg.Component)
SwarmModemRestart = swarm_m138_ns.class_("SwarmModemRestart", button.Button, cg.Component)

ICON_WIFI_ALERT = "mdi:wifi-alert"
ICON_WIFI_STAR = "mdi:wifi-star"
ICON_CHIP_CARD = "mdi:credit-card-chip-outline"
ICON_MAP_MARKER = "mdi:map-marker"
ICON_MAP_MARKER_UP = "mdi:map-marker-up"
ICON_SPEEDO = "mdi:speedometer-medium"
ICON_CHQ = "mdi:crosshairs-question"
ICON_CLOUD_UPLOAD_OUTLINE = "mdi:cloud-upload-outline"
ICON_MESSAGE_BADGE = "mdi:message-badge"
ICON_MESSAGE_PROCS = "mdi:message-processing"
ICON_MESSAGE_TEXT = "mdi:message-text"

CONF_GPS = "gps_fix"
CONF_LAT = "latitude"
CONF_LON = "longitude"
CONF_ALT = "altitude"
CONF_COU = "course"
CONF_SPE = "speed"
CONF_RSSI = "rssi"
CONF_TMEP = "time_epoch"
CONF_VER = "modem_fw_ver"
CONF_MSGT = "unsent_messages"
CONF_SPOOF = "gps_spoof_state"
CONF_JAM = "gps_jamming_level"
CONF_MSGU = "received_unread_messages"
CONF_MSG_IN_APID = "received_message_appid"
CONF_MSG_IN_TEXT = "received_message_data"
CONF_MSG_NOTI = "message_notifications_switch"
CONF_DEL_UNSENT = "delete_unsent_button"
CONF_DEL_RECEIVED = "delete_all_received_button"
CONF_READ_NEWEST = "read_newest_message_button"
CONF_CPU_V = "modem_cpu_voltage"
CONF_CPU_T = "modem_cpu_temperature"
CONF_MSG_UN_APID = "unsolicited_message_appid"
CONF_MSG_UN_TEXT = "unsolicited_message_data"
CONF_RESTART_MODEM = "restart_modem_button"

CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(SwarmModem),
    cv.Optional(CONF_MSG_NOTI): switch.SWITCH_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SwarmModemMsgNotifSw)}),
    cv.Optional(CONF_DEL_UNSENT): button.BUTTON_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SwarmModemDelUnsentMsg)}),
    cv.Optional(CONF_DEL_RECEIVED): button.BUTTON_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SwarmModemDelReceivedMsg)}),
    cv.Optional(CONF_READ_NEWEST): button.BUTTON_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SwarmModemReadNewestMsg)}),
    cv.Optional(CONF_RESTART_MODEM): button.BUTTON_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SwarmModemRestart)}),

    cv.Optional(CONF_LAT): text_sensor.text_sensor_schema(
        icon=ICON_MAP_MARKER
    ),
    cv.Optional(CONF_LON): text_sensor.text_sensor_schema(
        icon=ICON_MAP_MARKER
    ),
    cv.Optional(CONF_ALT): text_sensor.text_sensor_schema(
        icon=ICON_MAP_MARKER_UP
    ),
    cv.Optional(CONF_COU): text_sensor.text_sensor_schema(
        icon=ICON_SPEEDO
    ),
    cv.Optional(CONF_SPE): text_sensor.text_sensor_schema(
        icon=ICON_SPEEDO
    ),
    cv.Required(CONF_GPS): text_sensor.text_sensor_schema(
        icon=ICON_CHQ
    ),
    cv.Optional(CONF_SPOOF): text_sensor.text_sensor_schema(
        icon=ICON_WIFI_ALERT
    ),
    cv.Optional(CONF_VER): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_CHIP_CARD
    ),
    cv.Optional(CONF_MSG_IN_APID): text_sensor.text_sensor_schema(
        icon=ICON_MESSAGE_PROCS
    ),
    cv.Optional(CONF_MSG_IN_TEXT): text_sensor.text_sensor_schema(
        icon=ICON_MESSAGE_TEXT
    ),
    cv.Optional(CONF_MSG_UN_APID): text_sensor.text_sensor_schema(
        icon=ICON_MESSAGE_PROCS
    ),
    cv.Optional(CONF_MSG_UN_TEXT): text_sensor.text_sensor_schema(
        icon=ICON_MESSAGE_TEXT
    ),

    cv.Optional(CONF_RSSI): sensor.sensor_schema(
        unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
        icon=ICON_WIFI_STAR,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_SIGNAL_STRENGTH
    ),
    cv.Optional(CONF_TMEP): sensor.sensor_schema(
        unit_of_measurement=UNIT_SECOND,
        icon=ICON_COUNTER,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_MSGT): sensor.sensor_schema(
        icon=ICON_CLOUD_UPLOAD_OUTLINE,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_JAM): sensor.sensor_schema(
        icon=ICON_WIFI_ALERT,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_MSGU): sensor.sensor_schema(
        icon=ICON_MESSAGE_BADGE,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_CPU_V): sensor.sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=5,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_VOLTAGE
    ),
    cv.Optional(CONF_CPU_T): sensor.sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        state_class=STATE_CLASS_MEASUREMENT,
        device_class=DEVICE_CLASS_TEMPERATURE
    ),
}).extend(cv.polling_component_schema('60s'))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_MSG_NOTI in config:
        sw = await switch.new_switch(config[CONF_MSG_NOTI])
        cg.add(sw.set_parent(var))

    if CONF_DEL_UNSENT in config:
        btn = await button.new_button(config[CONF_DEL_UNSENT])
        cg.add(btn.set_parent(var))

    if CONF_DEL_RECEIVED in config:
        btn = await button.new_button(config[CONF_DEL_RECEIVED])
        cg.add(btn.set_parent(var))

    if CONF_READ_NEWEST in config:
        btn = await button.new_button(config[CONF_READ_NEWEST])
        cg.add(btn.set_parent(var))

    if CONF_RESTART_MODEM in config:
        btn = await button.new_button(config[CONF_RESTART_MODEM])
        cg.add(btn.set_parent(var))

    if CONF_GPS in config:
        sens = await text_sensor.new_text_sensor(config[CONF_GPS])
        cg.add(var.set_gps(sens))

    if CONF_LAT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_LAT])
        cg.add(var.set_lat(sens))

    if CONF_LON in config:
        sens = await text_sensor.new_text_sensor(config[CONF_LON])
        cg.add(var.set_lon(sens))

    if CONF_ALT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_ALT])
        cg.add(var.set_alt(sens))

    if CONF_COU in config:
        sens = await text_sensor.new_text_sensor(config[CONF_COU])
        cg.add(var.set_cou(sens))

    if CONF_SPE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SPE])
        cg.add(var.set_spe(sens))

    if CONF_VER in config:
        sens = await text_sensor.new_text_sensor(config[CONF_VER])
        cg.add(var.set_ver(sens))

    if CONF_SPOOF in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SPOOF])
        cg.add(var.set_spf(sens))

    if CONF_MSG_IN_APID in config:
        sens = await text_sensor.new_text_sensor(config[CONF_MSG_IN_APID])
        cg.add(var.set_msgapid(sens))

    if CONF_MSG_IN_TEXT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_MSG_IN_TEXT])
        cg.add(var.set_msgtext(sens))

    if CONF_MSG_UN_APID in config:
        sens = await text_sensor.new_text_sensor(config[CONF_MSG_UN_APID])
        cg.add(var.set_msuapid(sens))

    if CONF_MSG_UN_TEXT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_MSG_UN_TEXT])
        cg.add(var.set_msutext(sens))

    if CONF_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_RSSI])
        cg.add(var.set_rssi(sens))

    if CONF_TMEP in config:
        sens = await sensor.new_sensor(config[CONF_TMEP])
        cg.add(var.set_tme(sens))

    if CONF_MSGT in config:
        sens = await sensor.new_sensor(config[CONF_MSGT])
        cg.add(var.set_msgt(sens))

    if CONF_JAM in config:
        sens = await sensor.new_sensor(config[CONF_JAM])
        cg.add(var.set_jam(sens))

    if CONF_MSGU in config:
        sens = await sensor.new_sensor(config[CONF_MSGU])
        cg.add(var.set_msgu(sens))

    if CONF_CPU_V in config:
        sens = await sensor.new_sensor(config[CONF_CPU_V])
        cg.add(var.set_cpuv(sens))

    if CONF_CPU_T in config:
        sens = await sensor.new_sensor(config[CONF_CPU_T])
        cg.add(var.set_cput(sens))
