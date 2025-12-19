#pragma once

namespace KEYS {
    const unsigned char ADDRESS = 0xb4;

    enum Orders {
        GET_VERSION = 1,
        PRESS_KEY = 0x10,
        KEY_SIG = 0x12,   // low byte: KEY_xxx_SIG, high byte: UI::Flags
    };
}

namespace INSUF {
    const unsigned char ADDRESS = 0xb0;

    enum Signals {
        CALLING_SIG = 4,
        DATA_READY_SIG,
        ADC_READY_SIG,
        NO_ANSWER_SIG,
        KEY_POWER_SIG,
        TIMEOUT_SIG,
        U_MAX_SIG, // the last subcribed signal
        RESTART_SIG,
        REBOOT_SIG,
        KEY_START_SIG,
        KEY_PRES_DN_SIG,
        KEY_PRES_UP_SIG,
        KEY_BOOST_SIG,
        KEY_FLOW_DN_SIG,
        KEY_FLOW_UP_SIG,
        KEY_RESET_SIG,
        KEY_PRES_ZERO_SIG,
        KEY_WORKTIME_SIG,
        KEY_SERVICE_SIG,
        KEY_HEAT_SIG,
        KEY_AIR_SIG,
        KEY_MODE_SIG,
        KEY_ESCAPE_SIG,
        KEY_RELEASE_SIG,
        POWER_ON_SIG,
        POWER_OFF_SIG,
        ALARM_NO_CO2_SIG,
        ALARM_PINCH_SIG, // пережим трубки
        ALARM_BLEED_SIG, // давление превышает заданное на 3мм.рт.ст. в течении 10 сек
        ALARM_BREAK_SIG, // обрыв трубки
        ALARM_HIPRES_SIG, // слишком высокое давление (> 5атм)
        ALARM_NEG_PRES_SIG, // давление ниже -3 мм.рт.ст
        ALARM_SLAVE_FAULT_SIG, // подчиненный контроллер не подает признаков жизни
        ALARM_BLEED_VALVE_SIG, // неисправен стравливающий клапан
        BTA_SIG,
        END_BONDING_SIG,
        UPDATE_PRESET_PRES_SIG,
        UPDATE_PRESET_FLOW_SIG,
        ALARM_OVERHEATING_SIG,
        ALARM_HEATER_BREAK_SIG,
    };
}


namespace REDUC {
    const unsigned char ADDRESS = 0xb8;

    enum Orders {
        GET_VERSION = 1,
        ON_BLEED = 0x10, // начать сброс давления
        OFF_BLEED, // закончить сброс давления
        ON_FLOW, // включить подачу газа (открыть клапан)
        OFF_FLOW, // выключить подачу газа (закрыть клапан)
        SET_SHIM, // установка ШИМ'а (сразу отрабатывается)
        SHUT_OFF, // закрыть редуктор
        SET_GAS_FLOW, // l/min * 100, for only trocar, 15th bit - veresh status
        SET_FLOW_ERR, // l/min * 10, error of flow set
        SET_HIGH_PRES, // kPa
        ORDER_END
    };
}

namespace REGUL {
    const unsigned char ADDRESS = 0xb0;

    enum Orders {
        // команды сохраняемые в EEPROM
        TERROR, // признак ошибки
        GET_VERSION = 1,
        SET_PRES, // установка давления в мм.рт.ст.
        SET_FLOW, // установка расхода(ограничения) в л/мин
        GET_ADC_PRES, // измеренное давление в отсчетах АЦП
        GET_ADC_FLOW, // измеренный поток в отсчетах АЦП
        GET_MSR_PRES, // измеренное давление инсуффляции в мм.рт.ст. * 10
        GET_RDC_PRES, // вычесленное давление в мм.рт.ст. * 10
        GET_MSR_FLOW, // измеренный поток в л/мин * 100
        GET_MED_PRES, // среднее давление за импульс в мм.рт.ст. * 10
        GET_MED_FLOW, // средний поток за импульс в л/мин * 10
        PRESS_KEY, // ТОЛЬКО для регистрации
        // команды no save
        GET_TEMPERATURE, // if heater is on: High byte - PWM value, Low byte - heater temperture;
        // else: case interior temperature * 2
        SET_HEATER, // 1/0 - heater on/off
        SET_AIR, // 1 - using of air
        SET_MODE, //
        SET_ESCAPE, // 'escape level'/0 - escape mode on/off and set escape level
        TSTART = 22, // запуск инсуффляции
        TSTOP, // останов инсуффляции
        READ_MEM, // считать память по текущему адресу
        READ_MODE,
        GET_STATUS,
        ZERO_CALIBRATION,
        BOOST_FLOW,
        GET_ADC_DATA,
        GET_MSR_DATA,
        GET_RDC_DATA,
        ORDER_END
    };
}
