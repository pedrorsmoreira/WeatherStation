#include "mcc_generated_files/mcc.h"
#include "eeprom.h"

#ifdef CHECKSUM
uint8_t get_check_up_value(void){
#else
uint8_t get_check_up_value( uint8_t (*func) (uint8_t, uint8_t)){
#endif
    uint8_t check;
    #ifdef CHECKSUM
    check = DATAEE_ReadByte(NREG_) + DATAEE_ReadByte(PMON_) + DATAEE_ReadByte(TALA_) +
            DATAEE_ReadByte(ALAT_) + DATAEE_ReadByte(ALAL_) + DATAEE_ReadByte(ALAF_) + 
            DATAEE_ReadByte(CLKH_) + DATAEE_ReadByte(CLKM_);
    #else
    check = func(DATAEE_ReadByte(NREG_), func(DATAEE_ReadByte(PMON_), func(DATAEE_ReadByte(TALA_),
            func(DATAEE_ReadByte(ALAT_), func(DATAEE_ReadByte(ALAL_), func(DATAEE_ReadByte(ALAF_), 
            func(DATAEE_ReadByte(CLKH_), DATAEE_ReadByte(CLKM_))))))));
    #endif

    return check;
}

#ifdef CHECKSUM
void set_check_up_value(void){
#else
void set_check_up_value( uint8_t (*func) (uint8_t, uint8_t)){
#endif
    #ifdef CHECKSUM
    uint8_t check = get_check_up_value();
    #else
    uint8_t check = get_check_up_value(func);
    #endif
    DATAEE_WriteByte(CHECK_, check);
}

#ifdef CHECKSUM
bool check_corruption(void){
#else
bool check_corruption( uint8_t (*func) (uint8_t, uint8_t)){
#endif
    uint8_t check = DATAEE_ReadByte(CHECK_);
    #ifdef CHECKSUM
    return get_check_up_value() == check;
    #else
    return get_check_up_value(func) == check;
    #endif
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
    DATAEE_WriteByte(USED_, 0xAA);
    
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, nreg+pmon+tala+alat+alal+alaf+clkh+clkm);
    #endif
}

void eeprom_clk_update(uint8_t clkh, uint8_t clkm){
    DATAEE_WriteByte(CLKH_, clkh);
    DATAEE_WriteByte(CLKM_, clkm);
}

bool ring_buffer_write(uint8_t h, uint8_t m, uint8_t s, uint8_t T, uint8_t L){
    uint16_t ring_pos_ = DATAEE_ReadByte(WBUF_);
    uint16_t ring_pos = ring_pos_ + RBUF_;
    
    //none of the values changed, exit without writing
    if (T == DATAEE_ReadByte(ring_pos - 2) && L == DATAEE_ReadByte(ring_pos - 1)) 
        return false;
    
    //check if the writing position reached the end of the ring buffer
    if (ring_pos > (RBUF_ + DATAEE_ReadByte(NREG_) - 5) ){
        ring_pos = RBUF_;
        DATAEE_WriteByte(USED_, 0x55);
    }
    DATAEE_WriteByte(ring_pos  , h);
    DATAEE_WriteByte(ring_pos+1, m);
    DATAEE_WriteByte(ring_pos+2, s);
    DATAEE_WriteByte(ring_pos+3, T);
    DATAEE_WriteByte(ring_pos+4, L);
    
    //set the next write position
    DATAEE_WriteByte(WBUF_, ring_pos+5-RBUF_);
    
    #ifdef CHECKSUM
    DATAEE_WriteByte(CHECK_, DATAEE_ReadByte(CHECK_) + (ring_pos+5-RBUF_) - ring_pos_);
    #endif
    
    return true;
}

bool ring_buffer_laped(void) { return 0x55 == DATAEE_ReadByte(USED_); }

bool used(void) {
    uint8_t val = DATAEE_ReadByte(USED_);
    return (0xAA == val || 0x55 == val); 
}

uint8_t read_nreg(void) { return DATAEE_ReadByte(NREG_); }
uint8_t read_pmon(void) { return DATAEE_ReadByte(PMON_); }
uint8_t read_tala(void) { return DATAEE_ReadByte(TALA_); }
uint8_t read_alat(void) { return DATAEE_ReadByte(ALAT_); }
uint8_t read_alal(void) { return DATAEE_ReadByte(ALAL_); }
uint8_t read_alaf(void) { return DATAEE_ReadByte(ALAF_); }
uint8_t read_clkh(void) { return DATAEE_ReadByte(CLKH_); }
uint8_t read_clkm(void) { return DATAEE_ReadByte(CLKM_); }

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