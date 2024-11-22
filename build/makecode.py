#!/bin/python3
# coding: utf-8

import os, sys, json, time


def __calc_checksum(init, bytes_to_calc):
    ret = init
    for b in bytes_to_calc:
        ret += int.from_bytes(b, 'little')
    return ret


def __crc16_one_byte(lastcrc, one_byte):
    POLY_CRC16 = 0x1021
    b = one_byte  # int.from_bytes(one_byte, 'little')
    for i in range(8):
        if (((b << i) ^ (lastcrc >> 8)) & 0x80) != 0:
            lastcrc <<= 1
            lastcrc = (lastcrc ^ POLY_CRC16) & 0xFFFF
        else:
            lastcrc <<= 1
            lastcrc = lastcrc & 0xFFFF
    return lastcrc & 0xFFFF


def __crc16(init, bytes_to_calc):
    for b in bytes_to_calc:
        init = __crc16_one_byte(init, b)
    return init


def __get_file_data(_path):
    if not os.path.exists(_path):
        print('Error: %s file is not exist.' % _path)
        return None
    with open(_path, 'rb') as f:
        return f.read()

ENVS_VARS = [
    'ROOT_PATH',
    'BOARD',
    'PROJECT',
    'VERSION',
    'SUBVERSION',
]

def __access_attr(obj, *attr):
    global ENV
    tmp = obj
    for a in attr:
        if hasattr(tmp, a):
            tmp = getattr(tmp, a)
        else:
            return '0'
    for v in ENVS_VARS:
        json_env_var = '$('+v+')'
        if json_env_var in tmp:
            tmp = tmp.replace(json_env_var, ENV[v])
    return tmp

def get_files(path, suffix, prefix=None, ignoredir=None):
    files_name = []
    for parent,dirnames,filenames in os.walk(path, topdown=True):
        if ignoredir is not None:
            dirnames[:] = [d for d in dirnames if d not in ignoredir]
        for filename in filenames:
            if suffix is not None:
                if(filename.endswith(suffix)):
                    files_name.append(filename)
            elif prefix is not None:
                if(filename.startswith(prefix)):
                    files_name.append(filename)
            else:
                files_name.append(filename)
    return files_name


##########################################################################
#       section     name                                addr        length
# ---------------------------------- in file -----------------------------
#       1           up header                           ~           0x100
# ---------------------------------- in flash ----------------------------
#       2           boot loader                         0           0x3000
#       3           partition table                     0x3000      0x100
#       4           firmware                            0x4000      ?
#       5           tone
#       6           ota
#       x           ...
#       xx          ......
#       last        psm
##########################################################################
DEFAULT_BOOTLOADER_OFFSET = 0
DEFAULT_PARTITION_TABLE_OFFSET = 0x3000
DEFAULT_FIRMWARE_OFFSET = 0x4000
##########################################################################
class section: 
    def __init__(self, _type=''):
        self.offset = 0
        self.data = None
        self.items = []  # (data_offset, data_bytes)
        self.type = _type
    def add(self, item):
        self.items.append(item)
    def merge(self, force=False):
        if self.data is not None and force is False:
            return self.data
        self.data = bytes(0)
        for item in sorted(self.items):  # 0:data_offset, 1:data_bytes
            if len(item[1]) > 0:
                self.data += bytes(item[0] - len(self.data))
                self.data += item[1]
        return self.data
    def unmerge(self):
        self.data = None

def make_up_header(all_flash_data):
    up_header = section()
    up_header.add((0x00, b'CONFIG'))
    up_header.add((0x06, int.to_bytes(0x100, 4, 'little')))
    up_header.add((0x10, int.to_bytes(len(all_flash_data), 4, 'little')))
    up_header.add((0x14, int.to_bytes(__crc16(0xFFFF, all_flash_data), 2, 'little')))
    up_header.add((0x16, "SL6801".encode()))
    up_header.add((0xFE, b'\x55\xAA'))
    return up_header.merge()

############################################
# 制作 boot loader 区
############################################
def make_section_boot_loader(makeup_json):
    bootloader = section(__access_attr(makeup_json.root, 'boot', 'loader', 'type'))
    bootloader.offset = DEFAULT_BOOTLOADER_OFFSET
    boot_addr = int(__access_attr(makeup_json.root, 'boot', 'boot_addr'), 16)
    loader_file_data = __get_file_data(__access_attr(makeup_json.root, 'boot', 'loader', 'path'))
    loader_file_len = len(loader_file_data)
    loader_sector_len = loader_file_len + boot_addr
    loader_sector_len_with_4KB = DEFAULT_PARTITION_TABLE_OFFSET
    while loader_sector_len_with_4KB < loader_sector_len:
        loader_sector_len_with_4KB += 0x1000
    bootloader.add((0x00, b'HLKJ'))
    bootloader.add((0x04, int(__access_attr(makeup_json.root, 'boot', 'ram_addr_write'), 16).to_bytes(4, 'little')))
    bootloader.add((0x08, int(__access_attr(makeup_json.root, 'boot', 'ram_addr_run'), 16).to_bytes(4, 'little')))
    bootloader.add((0x0C, boot_addr.to_bytes(4, 'little')))
    bootloader.add((0x10, loader_file_len.to_bytes(4, 'little')))
    bootloader.add((0x14, __crc16(0xFFFF, loader_file_data).to_bytes(4, 'little')))  # 4 bytes
    bootloader.add((0x18, int(__access_attr(makeup_json.root, 'boot', 'norflash', 'norf_baud'), 16).to_bytes(1, 'little')))
    bootloader.add((0x1C, int(__access_attr(makeup_json.root, 'boot', 'norflash', 'norf_line'), 16).to_bytes(1, 'little')))
    bootloader.add((0x1D, int(__access_attr(makeup_json.root, 'boot', 'norflash', 'norf_size'), 16).to_bytes(1, 'little')))
    bootloader.add((0x20, loader_sector_len_with_4KB.to_bytes(4, 'little')))
    bootloader.add((0x2C, int(__access_attr(makeup_json.root, 'boot', 'doDebug'), 16).to_bytes(4, 'little')))  # maybe add other flags
    bootloader.add((0x30, bytes(0x5C-0x30)))
    bootloader_data = bootloader.merge()
    bootloader.add((0x5C, __crc16(0xFFFF, bootloader_data).to_bytes(4, 'little')))  # 4 bytes
    bootloader.add((boot_addr, loader_file_data))
    bootloader.unmerge()
    return bootloader, loader_sector_len_with_4KB

############################################
# 制作分区表（索引区）
############################################
def make_section_partition_table(sections, version, subver=None, offset=DEFAULT_PARTITION_TABLE_OFFSET):
    partition_table = section('TABL')
    partition_table.offset = offset
    partition_table.add((0x00, len(sections).to_bytes(4, 'little')))
    partition_table.add((0x04, int(version).to_bytes(4, 'little')))
    if subver is not None:
        print('subv: %s' % subver)
        partition_table.add((0x08, str.encode(subver)))
    for i in range(1, len(sections)+1):
        partition_table.add((0x10*i, sections[i-1].type.encode()))
        partition_table.add((0x10*i+4, sections[i-1].offset.to_bytes(4, 'little')))
        if sections[i-1].data is None or 0 == len(sections[i-1].data):
            partition_table.add((0x10*i+8, sections[i-1].length.to_bytes(4, 'little')))
        else:
            partition_table.add((0x10*i+8, len(sections[i-1].data).to_bytes(4, 'little')))
            partition_table.add((0x10*i+12, __crc16(0xFFFF, sections[i-1].data).to_bytes(4, 'little')))
    return partition_table

############################################
# 制作固件分区
############################################
def make_section_firmware(makeup_json):
    firmware = section(__access_attr(makeup_json.root, 'partitions', 'firmware', 'type'))
    firmware.offset = DEFAULT_FIRMWARE_OFFSET
    app_file_data = __get_file_data(__access_attr(makeup_json.root, 'partitions', 'firmware', 'path'))
    app_file_data_first_load_from_flash = int(__access_attr(makeup_json.root, 'partitions', 'firmware', 'loadLength'), 16)
    firmware.add((0x00, int.to_bytes(0x30, 4, 'little')))
    firmware.add((0x04, int(str(int(time.strftime("%y", time.localtime())))+time.strftime("%m%d%H%M", time.localtime())).to_bytes(4, 'little')))  # 2112301230, 理解为21年12月31号12点30分
    firmware.add((0x10, int(__access_attr(makeup_json.root, 'partitions', 'firmware', 'loadToRam'), 16).to_bytes(4, 'little')))
    firmware.add((0x14, int(__access_attr(makeup_json.root, 'partitions', 'firmware', 'runFromRam'), 16).to_bytes(4, 'little')))
    firmware.add((0x18, app_file_data_first_load_from_flash.to_bytes(4, 'little')))
    firmware.add((0x1C, __crc16(0xFFFF, app_file_data[0:app_file_data_first_load_from_flash]).to_bytes(4, 'little')))  # 4 bytes
    firmware.add((0x30, app_file_data))
    return firmware

############################################
# 制作ota分区
############################################
def make_section_otap(makeup_json):
    if 'OTAP' != __access_attr(makeup_json.root, 'partitions', 'otap', 'type'):
        return None
    if not os.path.exists(__access_attr(makeup_json.root, 'partitions', 'otap', 'path')):
        return None
    firmware = section(__access_attr(makeup_json.root, 'partitions', 'otap', 'type'))
    firmware.offset = int(__access_attr(makeup_json.root, 'partitions', 'otap', 'addr'), 16)
    app_file_data = __get_file_data(__access_attr(makeup_json.root, 'partitions', 'otap', 'path'))
    app_file_data_first_load_from_flash = int(__access_attr(makeup_json.root, 'partitions', 'otap', 'loadLength'), 16)
    firmware.add((0x00, int.to_bytes(0x30, 4, 'little')))
    firmware.add((0x04, int(str(int(time.strftime("%y", time.localtime())))+time.strftime("%m%d%H%M", time.localtime())).to_bytes(4, 'little')))  # 2112301230, 理解为21年12月31号12点30分
    firmware.add((0x10, int(__access_attr(makeup_json.root, 'partitions', 'otap', 'loadToRam'), 16).to_bytes(4, 'little')))
    firmware.add((0x14, int(__access_attr(makeup_json.root, 'partitions', 'otap', 'runFromRam'), 16).to_bytes(4, 'little')))
    firmware.add((0x18, app_file_data_first_load_from_flash.to_bytes(4, 'little')))
    firmware.add((0x1C, __crc16(0xFFFF, app_file_data[0:app_file_data_first_load_from_flash]).to_bytes(4, 'little')))  # 4 bytes
    firmware.add((0x30, app_file_data))
    return firmware

############################################
# 爬行函数，制作其他分区文件
############################################
def make_section_partitions(makeup_json_others):
    for file_obj in makeup_json_others:
        sec = section(__access_attr(file_obj, 'type'))
        other_path = __access_attr(file_obj, 'path')
        if os.path.exists(other_path):
            sec.add((0, __get_file_data(other_path)))
            yield sec

############################################
# 制作psm分区（内容为空，仅供索引）
############################################
def make_section_psm(makeup_json):
    sec = section(__access_attr(makeup_json.root, 'partitions', 'psm', 'type'))
    sec.offset = int(__access_attr(makeup_json.root, 'partitions', 'psm', 'addr'), 16)
    sec.length = int(__access_attr(makeup_json.root, 'partitions', 'psm', 'length'), 16)
    return sec

############################################
# 制作tone.bin
############################################
def make_tone(dst_dir, src_dir):
    #print('make tone file:')
    total_size = 0
    tone_srcs = get_files(src_dir, '.mp3')+get_files(src_dir, '.wav')
    out_file = open(dst_dir + os.sep + "tone.bin", "wb")
    #tone.bin头信息生成
    for item in tone_srcs:
        size = os.path.getsize(src_dir + os.sep + item)
        #print(item + ": size:" + str(size));
        total_size += size
        out_file.write(size.to_bytes(4, 'little'))
        out_file.write(bytes(item, encoding='utf-8') + b'\x00')
    # 0用于标记已到最后一个文件
    total_size /= 1024
    #print( "total size:" + str(total_size));
    if total_size > 350:#tone.bin暂不允许大于350k
        print( "##error tone too big(>350k)!! size:"+ str(total_size))
        exit(-1)
    size = 0
    out_file.write(size.to_bytes(4, 'little'))
    out_file.write(b'\x00')
    #音频数据从1024offset开始
    out_file.seek(1024, 0)
    #添加音频数据
    for item in tone_srcs:
        in_file = open(src_dir + os.sep + item, "rb")
        out_file.write(in_file.read())
        in_file.close()
    out_file.close()
    #print('Make tone success!')

def make_4KB_align(sections):
    last_4KB_align = 0
    last_data_tail = 0
    for sec in sections:
        while last_4KB_align < last_data_tail:
            last_4KB_align += 0x1000
        if sec.offset < last_4KB_align:
            if sec.offset > 0: 
                print('Notes: partition: %s offset: 0x%x is overlaped, then move to: 0x%08x' % (sec.type, sec.offset, last_4KB_align))
            sec.offset = last_4KB_align
        last_data_tail = sec.offset + len(sec.merge())

############################################
# 制作 up 文件中将要写入 Flash 的内容
############################################
def make_body(makeup_json):
    # 产生 bootloader 区
    sec_bootloader, partition_table_offset = make_section_boot_loader(makeup_json)
    print('make bootloader!')
    
    # 产生固件区
    sec_firmware = make_section_firmware(makeup_json)
    print('make firmware!')
    
    # 产生 psm 分区标记
    sec_psm = make_section_psm(makeup_json)
    print('make psm!')

    # 产生 ota 分区，（但可选）
    sec_ota = make_section_otap(makeup_json)
    ota_secttions = [] if sec_ota is None else [sec_ota]
    print('ota partition is', ota_secttions)
    
    # 被索引的区全部集合在一起
    tabled_sections = [sec_firmware] + \
                      [sec for sec in make_section_partitions(__access_attr(makeup_json.root, 'partitions', 'others'))] + \
                      ota_secttions + \
                      [sec_psm]
    if len(tabled_sections) > 14:
        print('Error: too many paritions, parition number must less than 14!')
        exit(-1)

    # 确保所有区 4KB align.
    fake_partition_table = section()
    fake_partition_table.offset = partition_table_offset
    fake_partition_table.add((0, bytes(256)))  # parition table must be less than 256 bytes.
    make_4KB_align([sec_bootloader, fake_partition_table] + tabled_sections)

    # 产生分区表索引区
    sec_partition_table = make_section_partition_table(tabled_sections, __access_attr(makeup_json.root, 'version'), __access_attr(makeup_json.root, 'subver'), offset=fake_partition_table.offset)

    # these are all sections!
    all_sections = [sec_bootloader, sec_partition_table] + tabled_sections
    for sec in all_sections:
        print('parition: %s offset is: 0x%x' % (sec.type, sec.offset))
        if sec.type == 'OTAP':
            if sec.offset > int(__access_attr(makeup_json.root, 'partitions', 'otap', 'addr'), 16):
                print('##OTA can no move offset,please adjust section table!')
                exit(-1)
    # do all flash data
    flash_content = section()
    for sec in all_sections:
        flash_content.add((sec.offset, sec.merge()))
    return flash_content.merge()

######################################
# 输出 make_up.json 对象
######################################
def get_json_object_from_file(json_config_file):
    class JSONObject:
        def __init__(self, d):
            self.__dict__ = d
    with open(json_config_file, 'r', encoding='utf-8') as f:
        cfg = json.load(f, object_hook=JSONObject)
    return cfg

######################################
# 提取 make_up.json 中的变化依赖项
######################################
def get_json_depend_files_from(env):
    global ENV
    ENV=env
    jo = env['jcfg']
    file_list = []
    file_list.append(__access_attr(jo.root, 'boot', 'loader', 'path'))
    file_list.append(__access_attr(jo.root, 'partitions', 'firmware', 'path'))
    for file_obj in __access_attr(jo.root, 'partitions', 'others'):
        if os.path.exists(__access_attr(file_obj, 'path')):
            file_list.append(__access_attr(file_obj, 'path'))
    return file_list


######################################
# 制作 up 文件
######################################
def make_up_command(target, source, env):
    global ENV
    ENV=env
    print('Build new up file from:')
    for item in source:
        print(item)
    # 准备好写入Flash 的全部内容
    body = make_body(env['jcfg'])
    with open(target[0].path, 'wb') as up_file:      # 保存到目标目录下，存成 .up文件
        # 为这个内容做一个up文件头加到前面保存成文件
        up_file.write(make_up_header(body) + body)
    print('Build success!')

######################################
# 基于已生成 up 文件制作全Flash覆盖版 up
######################################
def make_full_up_command(target, source, env):
    global ENV
    ENV=env
    if 'FLASH_SIZE' in env.keys():
        print('Build full up file ...')
        with open(source[0].path, 'rb') as up_file:
            up = up_file.read()
        with open(target[0].path, 'wb') as full_up_file:
            to_fill_bytes = (env['FLASH_SIZE'] * 1024 + 0x100) - len(up)
            full_up_file.write(up + bytes([0xFF for i in range(to_fill_bytes)]))
        print('Build full up file success!')

    
