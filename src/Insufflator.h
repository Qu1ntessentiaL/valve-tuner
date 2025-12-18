/**
 * @file Insufflator.h
 * @brief Замкнутый алгоритм управления клапаном и расходом (insufflation).
 */

#pragma once

#include "orders.h"

/**
 * @brief Реализует алгоритм управления клапаном для калибровки по расходу.
 *
 * Класс Insufflator инкапсулирует последовательность обмена командами
 * с устройством и выполняет простой замкнутый контур регулирования,
 * стремящийся достигнуть заданного значения расхода @c SETTING.
 * Снаружи доступны текущий расход, PWM и ошибка регулирования.
 */
class Insufflator {
    const bool IS_CO2 = true;   ///< Режим CO₂ для измерения расхода.
    Data *mydata;               ///< Общий объект транспорта данных (протокол устройства).
    int delay;                  ///< Счётчик тиков между сменой состояний (открыт/закрыт).
    int PULSE_TIME = 20;        ///< Длительность импульса открытия клапана (в тиках).
    double SETTING;             ///< Целевое значение расхода (уставка).
    int PWM_INIT = 2900;        ///< Начальное значение PWM при запуске алгоритма.
    int PAUSE = 4;              ///< Пауза между импульсами (в тиках).
    bool is_valve_on = true;    ///< Текущее состояние клапана (открыт/закрыт).

public :
    double currentFlow;         ///< Последнее измеренное значение расхода (л/мин).
    short pwm;                  ///< Текущее значение PWM, отправленное на клапан.
    double error = 99;          ///< Текущая ошибка регулирования (расход - уставка).

    /**
     * @brief Результат линейной аппроксимации (PWM = slope * flow + offset).
     */
    struct result {
        double slope; ///< Approximation slope.
        int offset;   ///< Approximation offset.
    };

    /**
     * @brief Пара калибровочных точек, по которым считается @ref result.
     */
    struct INValue {
        int PWM1 = 0;
        int PWM2 = 0;
        double FLOW1 = 0.0;
        double FLOW2 = 0.0;
    };

    /**
     * @brief Конструктор алгоритма с заданным транспортом и уставкой расхода.
     * @param dataPtr Указатель на общий объект @ref Data.
     * @param setting Желаемое значение расхода.
     *
     * В конструкторе выполняется начальная последовательность команд:
     * переход в сервисный режим, установка давления, включение подачи газа.
     * После этого устройство готово к регулированию расхода.
     */
    Insufflator(Data *dataPtr, double setting) : mydata(dataPtr), SETTING(setting) {
        Data::DataNode node;
        pwm = PWM_INIT;
        delay = PAUSE;
        PULSE_TIME -= PAUSE;
        mydata->SendData(KEYS::ADDRESS, KEYS::KEY_SIG, INSUF::KEY_SERVICE_SIG);
        do mydata->RecieveData(&node);
        while (node.tag != KEYS::KEY_SIG);
        QThread::msleep(2000);
        mydata->SendData(REGUL::ADDRESS, REGUL::SET_PRES, 30);
        do mydata->RecieveData(&node);
        while (node.tag != REGUL::SET_PRES);
        mydata->SendData(REDUC::ADDRESS, REDUC::ON_FLOW, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::ON_FLOW);
    }

    /**
     * @brief Деструктор отправляет команды для безопасного отключения потока.
     */
    ~Insufflator() {
        Data::DataNode node;
        mydata->SendData(REDUC::ADDRESS, REDUC::SHUT_OFF, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::SHUT_OFF);
        mydata->SendData(REDUC::ADDRESS, REDUC::OFF_FLOW, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::OFF_FLOW);
    }

    /**
     * @brief Запрашивает у устройства текущее значение расхода.
     * @return Структура DataNode с «сырым» и приведённым значением расхода.
     */
    Data::DataNode getFlow(void) {
        Data::DataNode node;
        mydata->SendData(REGUL::ADDRESS, REGUL::GET_MSR_FLOW, IS_CO2, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REGUL::GET_MSR_FLOW);
        UART::logUARTData("FLOW", node.data);
        node.data /= 100;
        return node;
    }

    /**
     * @brief Один шаг алгоритма: измерение расхода и обновление состояния клапана.
     */
    void tick() {
        currentFlow = getFlow().data;
        if (!(--delay)) {
            if (is_valve_on) { offPulse(); } else onPulse();
        }
    }

    /**
     * @brief Открывает клапан и запланирует следующую паузу.
     */
    void onPulse() {
        is_valve_on = true;
        Data::DataNode node;
        mydata->SendData(REDUC::ADDRESS, REDUC::SET_SHIM, pwm);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::SET_SHIM);
        delay = PULSE_TIME;
    }

    /**
     * @brief Закрывает клапан, пересчитывает PWM по ошибке и запускает паузу.
     */
    void offPulse() {
        is_valve_on = false;
        Data::DataNode node;
        error = currentFlow - SETTING;
        pwm += 10 * error;
        if (pwm < 0) pwm = 0;
        if (pwm > 4000) pwm = 4000;
        mydata->SendData(REDUC::ADDRESS, REDUC::SHUT_OFF, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::SHUT_OFF);
        delay = PAUSE;
    }

    /**
     * @brief Публичная точка входа для продвижения состояния алгоритма.
     *
     * На данный момент просто вызывает @ref tick(), но оставлена как
     * отдельный метод для возможного усложнения логики.
     */
    void algorithms() {
        tick();
    }

    /**
     * @brief Вычисляет параметры линейной аппроксимации по двум точкам.
     *
     * @param in_value Заполненная структура @ref INValue с двумя точками.
     * @return Структура с рассчитанными наклоном и смещением.
     */
    static result approximate(const INValue &in_value) {
        result res;
        double slope = (in_value.PWM1 - in_value.PWM2) / (in_value.FLOW2 - in_value.FLOW1);
        res.slope = round(slope * 100) / 100;
        res.offset = in_value.PWM1 + in_value.FLOW1 * res.slope - 800;
        return res;
    }
};
