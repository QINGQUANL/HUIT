#ifndef _WRITE_IRQ_H_
#define _WRITE_IRQ_H_


enum write_irq_region_id_e {
    WRIRQ_RGROM = 0,
    WRIRQ_RG1,
    WRIRQ_RG2,
    WRIRQ_RG3,
};

enum write_irq_cpu_id_e {
    WRIRQ_CPU0 = 0,
    WRIRQ_CPU1,
};

void write_irq_set_region_addr(uint8_t region, uint32_t start_addr, uint32_t end_addr);
void write_irq_enable_region_irq(uint8_t region, uint8_t cpux, bool enable);
bool write_irq_is_region_pending(uint8_t region, uint8_t cpux);
void write_irq_clear_region_pending(uint8_t region, uint8_t cpux);
uint32_t write_irq_get_addr(uint8_t cpux);
uint32_t write_irq_get_pc(uint8_t cpux);
uint32_t write_irq_get_sr(void);


#endif

