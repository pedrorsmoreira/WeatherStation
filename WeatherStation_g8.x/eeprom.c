#include "mcc_generated_files/mcc.h"
#include "eeprom.h"

uint8_t ALAF;

uint8_t get_check_up_value( uint8_t (*func) (uint8_t, uint8_t)){
    
    uint8_t check = func(DATAEE_ReadByte(NREG_), func(DATAEE_ReadByte(PMON_), 
            func(DATAEE_ReadByte(TALA_), func(DATAEE_ReadByte(ALAT_), func(DATAEE_ReadByte(ALAL_), 
            func(DATAEE_ReadByte(ALAF_), func(DATAEE_ReadByte(CLKH_), DATAEE_ReadByte(CLKM_))))))));
    //uint8_t xor = func(nreg, func(pmon, func(tala, func(alat, func(alal, func(alaf, func(clkh, clkm)))))))
    
    uint8_t wbuf = DATAEE_ReadByte(WBUF_);
    uint8_t rbuf = DATAEE_ReadByte(RBUF_);
    check = func(check, func(wbuf, rbuf));
    
    while (rbuf < wbuf)
        check = func(check, DATAEE_ReadByte(++rbuf));
    
    return check;
}

void set_check_up_value( uint8_t (*func) (uint8_t, uint8_t)){
    uint8_t check = get_check_up_value(func);
    DATAEE_WriteByte(CHECK_, check);
}

bool check_corruption( uint8_t (*func) (uint8_t, uint8_t)){
    uint8_t check = DATAEE_ReadByte(CHECK_);
    return get_check_up_value(func) == check;
}

void eeprom_setup(bool reset_buffer, uint8_t nreg, uint8_t pmon, uint8_t tala, 
        uint8_t alat, uint8_t alal, uint8_t alaf, uint8_t clkh, uint8_t clkm){
    
    if (reset_buffer)
        DATAEE_WriteByte(WBUF_, 0);
    
    DATAEE_WriteByte(NREG_, nreg);
    DATAEE_WriteByte(PMON_, pmon);
    DATAEE_WriteByte(TALA_, tala);
    DATAEE_WriteByte(ALAT_, alat);
    DATAEE_WriteByte(ALAL_, alal);
    DATAEE_WriteByte(ALAF_, alaf);
    DATAEE_WriteByte(CLKH_, clkh);
    DATAEE_WriteByte(CLKM_, clkm);
}

void eeprom_clk_update(uint8_t clkh, uint8_t clkm){
    DATAEE_WriteByte(CLKH_, clkh);
    DATAEE_WriteByte(CLKM_, clkm);
}

bool ring_buffer_write(uint8_t h, uint8_t m, uint8_t s, uint8_t T, uint8_t L){
    
    uint16_t ring_pos = RBUF_ + DATAEE_ReadByte(WBUF_);
    
    //none of the values changed, exit without writing
    if (T == DATAEE_ReadByte(ring_pos - 2) && L == DATAEE_ReadByte(ring_pos - 1)) 
        return false;
    
    //check if the writing position reached the end of the ring buffer
    if (ring_pos > (RBUF_ + DATAEE_ReadByte(NREG_) - 5) )
        ring_pos = RBUF_;
    
    DATAEE_WriteByte(ring_pos  , h);
    DATAEE_WriteByte(ring_pos+1, m);
    DATAEE_WriteByte(ring_pos+2, s);
    DATAEE_WriteByte(ring_pos+3, T);
    DATAEE_WriteByte(ring_pos+4, L);
    
    //set the next write position
    DATAEE_WriteByte(WBUF_, ring_pos+5-RBUF_);
    
    return true;
}
