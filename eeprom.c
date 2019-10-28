void write_nreg(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(NREG_));
    #endif
    DATAEE_WriteByte(NREG_, x); 
}

void write_pmon(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(PMON_));
    #endif
    DATAEE_WriteByte(PMON_, x); 
}

void write_tala(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(TALA_));
    #endif
    DATAEE_WriteByte(TALA_, x); 
}

void write_alat(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(ALAT_));
    #endif
    DATAEE_WriteByte(ALAT_, x); 
}

void write_alal(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(ALAL_));
    #endif
    DATAEE_WriteByte(ALAL_, x); 
}

void write_alaf(uint8_t x) {
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + x - DATAEE_ReadByte(ALAF_));
    #endif
    DATAEE_WriteByte(ALAF_, x); 
}