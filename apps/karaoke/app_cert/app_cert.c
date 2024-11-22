#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "info_mem.h"
#include "console.h"

#define USE_LIMITE_COUNT        100     //设备id不匹配时的限制次数
#define PSM_USE_COUNT                   "device_use_count"

void set_certification_code(const uint8_t* certCode);
bool get_certification_check();

uint32_t g_cur_use_count;
const uint8_t cert_code[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

void cert_app_demo()
{
    //设置8BYTE的用户码，这个码由原厂提供
    set_certification_code(cert_code);
    //用这个函数来检查是否对比成功
    if(get_certification_check()) { 
        logi("cert check pass");
    } else {
        logi("cert check fail");
        //可以用while停住
        //while(1);
        
        //或者限次次数
        sys_info_read(PSM_USE_COUNT, &g_cur_use_count, 4);
        if(g_cur_use_count >= USE_LIMITE_COUNT) {
            while(1);   //达到次数停住
        } else {
            g_cur_use_count++;
            sys_info_write(PSM_USE_COUNT, &g_cur_use_count, 4 );
        }
    }
}
