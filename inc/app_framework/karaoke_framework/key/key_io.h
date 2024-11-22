#ifndef _KEY_IO_H_
#define _KEY_IO_H_

#define KIO0            0x01
#define KIO1            0x02
#define KIO2            0x04
#define KIO3            0x08
#define KIO4            0x10
#define KIO5            0x20
#define KIO6            0x40
#define KIO7            0x80


typedef struct key_io_info {
    bool enable;
    uint32_t pin;
    uint32_t key_event;
} key_io_info_t;


void kio_init(key_io_info_t *table, uint8_t table_size);
void kio_get_sta(void);
uint32_t kio_get_key(bool kio_m_en);

#endif

