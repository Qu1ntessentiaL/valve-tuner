#pragma once

#include "orders.h"

class Insufflator {
    const bool IS_CO2 = true;
    Data *mydata;
    int delay;
    int PULSE_TIME = 20;
    double SETTING;
    int PWM_INIT = 2900;
    int PAUSE = 4;
    bool is_valve_on = true;

public :
    double currentFlow;
    short pwm;
    double error = 99;

    struct result {
        double slope;
        int offset;
    };

    struct INValue {
        int PWM1 = 0;
        int PWM2 = 0;
        double FLOW1 = 0.0;
        double FLOW2 = 0.0;
    };

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

    ~Insufflator() {
        Data::DataNode node;
        mydata->SendData(REDUC::ADDRESS, REDUC::SHUT_OFF, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::SHUT_OFF);
        mydata->SendData(REDUC::ADDRESS, REDUC::OFF_FLOW, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::OFF_FLOW);
    }

    Data::DataNode getFlow(void) {
        Data::DataNode node;
        mydata->SendData(REGUL::ADDRESS, REGUL::GET_MSR_FLOW, IS_CO2, 0);
        do mydata->RecieveData(&node);
        while (node.tag != REGUL::GET_MSR_FLOW);
        UART::logUARTData("FLOW", node.data);
        node.data /= 100;
        return node;
    }

    void tick() {
        currentFlow = getFlow().data;
        if (!(--delay)) {
            if (is_valve_on) { offPulse(); } else onPulse();
        }
    }

    void onPulse() {
        is_valve_on = true;
        Data::DataNode node;
        mydata->SendData(REDUC::ADDRESS, REDUC::SET_SHIM, pwm);
        do mydata->RecieveData(&node);
        while (node.tag != REDUC::SET_SHIM);
        delay = PULSE_TIME;
    }

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

    void algorithms() {
        tick();
    }

    static result approximate(const INValue &in_value) {
        result res;
        double slope = ((in_value.PWM1 - in_value.PWM2) / (in_value.FLOW2 - in_value.FLOW1));
        res.slope = round(slope * 100) / 100;
        res.offset = in_value.PWM1 + in_value.FLOW1 * res.slope - 800;
        return res;
    }
};
