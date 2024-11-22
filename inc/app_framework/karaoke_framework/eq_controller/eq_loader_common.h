#ifndef EQ_LOADER_COMMON_H
#define EQ_LOADER_COMMON_H

#include "atunning_common.h"

typedef struct _EQ_FIXED_HEADER{
    uint8_t  magic[4]; //文件头"SLEQ"
    uint8_t  version;  //版本号
    uint8_t  eq_num;   //EQ配置的个数。一般都是只有一个配置; 部分产品可能有多个配置，例如分成：音乐模式EQ, 游戏模式EQ, 电影模式EQ
    uint8_t  reserved[2]; //保留位
    uint16_t first_eq_off; //第一个EQ配置数据存放位置的偏移
}EQ_FIXED_HEADER;

typedef struct _EQ_HEADER{
    EQ_FIXED_HEADER  eq_fhdr;
    uint32_t eq_lens;
    uint16_t eq_hdr_len;
    uint16_t crc_hdr;
    uint16_t crc_val;
}EQ_HEADER;

/*
    Eq文件读取api:
    @eqconfig_id : eq2文件可能有多个配置，默认一个。即默认 eqconfig_id=0
    @page_id :     决定要导入eq2文件中那个页面的参数，全页面有，可以参见eq_controller工具
                    PAGE_MUSIC   0
                    PAGE_EFFECT  1
                    PAGE_MIC     2
                    PAGE_OUTPUT1 3
                    PAGE_OUTPUT2 4
                    PAGE_VSS     5
*/
bool load_eq_setting(int8_t eqconfig_id, uint8_t page_id);

#endif