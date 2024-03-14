#include "Battery.h"

Battery::Battery() {}

Battery::Battery(int capacityInMilliAmpereHours, int emptyVoltageInMilliVolts) {
  batteryCapacityInMiliampereHours = capacityInMilliAmpereHours;
  batteryEmptyVoltage = emptyVoltageInMilliVolts;
}

bool Battery::begin() {
  if (readBitFromRegister(this->wire, FUEL_GAUGE_ADDRESS, STATUS_REG, POR_BIT)) {

    uint16_t tempHibernateConfigRegister = readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, HIB_CFG_REG);

    // release from hibernation
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, SOFT_WAKEUP_REG, 0x90); // Exit Hibernate Mode step 1
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, HIB_CFG_REG, 0x0);      // Exit Hibernate Mode step 2
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, SOFT_WAKEUP_REG, 0x0);  // Exit Hibernate Mode step 3

    // set battery configuration
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, V_EMPTY_REG, (uint16_t)(batteryEmptyVoltage / VOLTAGE_MULTIPLIER));
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, DESIGN_CAP_REG, (uint16_t)(batteryCapacityInMiliampereHours / CAP_MULTIPLIER));
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, I_CHG_TERM_REG, (uint16_t)(101 / CURRENT_MULTIPLIER));

    // TODO: Where does this value come from?
    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, MODEL_CFG_REG, 0x8000); // Write ModelCFG

    writeRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, HIB_CFG_REG, tempHibernateConfigRegister); // Restore Original HibCFG value
    replaceRegisterBits(this->wire, FUEL_GAUGE_ADDRESS, STATUS_REG, 0, 0x01, POR_BIT);
  }
}

bool Battery::isConnected(){
  uint16_t statusRegister = readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, STATUS_REG);
  return getBitFromOffset(statusRegister, BATTERY_STATUS_BIT) == 0;
}

int Battery::voltage(){
  if(!isConnected()){
    return -1;
  }
  
  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, VCELL_REG) * VOLTAGE_MULTIPLIER;
}

unsigned int Battery::averageVoltage(){
  if(!isConnected()){
    return -1;
  }
  
  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, AVG_VCELL_REG) * VOLTAGE_MULTIPLIER;
}

int Battery::temperature(){
  if(!isConnected()){
    return -1;
  }

  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, TEMP_REG) >> 8;
}

int Battery::averageTemperature(){
  if(!isConnected()){
    return -1;
  }

  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, AVG_TA_REG) >> 8;
}

int Battery::current(){
  if(!isConnected()){
    return -1;
  }
  
  return (int16_t)readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, CURRENT_REG) * CURRENT_MULTIPLIER;
}

int Battery::averageCurrent(){
  if(!isConnected()){
    return -1;
  }

  return (int16_t)readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, AVG_CURRENT_REG) * CURRENT_MULTIPLIER;
}

int Battery::percentage(){
  if(!isConnected()){
    return -1;
  }

  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, AV_SOC_REG) >> 8;
}

 int Battery::remainingCapacity(){
  if(!isConnected()){
    return -1;
  }
  
  return readRegister16Bits(this->wire, FUEL_GAUGE_ADDRESS, REP_CAP_REG) * CAP_MULTIPLIER;
}
