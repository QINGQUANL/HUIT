#ifndef _APP_RECORD_H_
#define _APP_RECORD_H_

void prepare_record_device(void);
void index_to_filename(int32_t index, char *name);
void scan_record_file(void);
void delete_record_file_by_num(void);
int play_prev_record_file(void);
int play_next_record_file(void);
void record_exit(void);
void record_enter(void);
void record_event_process(uint32_t event);
void record_set_data(int16_t* buf, uint32_t len);
void record_data_get(int16_t* buf, uint32_t len);
#endif

