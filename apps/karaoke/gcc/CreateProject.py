#!/bin/python3
# coding: utf-8

#########################################################
# usage:
# python CreateProject.py  src_prject_name dst_prjcct_name

#########################################################

import  os, sys, shutil

BASE_DIR = os.path.dirname(os.path.realpath('__file__'))

SOURCE_DIR_PATH = os.path.abspath(os.path.join(BASE_DIR, '..')) + os.sep +'app_config'


copy_file_list = [
    'bin',
    'tone',
    'config'
]

rename_file_list = [
    '_user_config.h',
    '_audio_params_config.h',
    '_ui.c'
	'_usb_app_config.h'
]

def _rename_project_file(source_path, target_path):
    if os.path.exists(source_path):
        if not os.path.exists(target_path):
            os.rename(source_path,target_path)
        else:
            os.remove(target_path)
            os.rename(source_path,target_path)

def _copy_project_file(source_path,target_path):
   
    if not os.path.exists(target_path):
        os.makedirs(target_path)

    if os.path.exists(source_path):
        # root 所指的是当前正在遍历的这个文件夹的本身的地址
        # dirs 是一个 list，内容是该文件夹中所有的目录的名字(不包括子目录)
        # files 同样是 list, 内容是该文件夹中所有的文件(不包括子目录)
        for root, dirs, files in os.walk(source_path):
            for file in files:
                src_file = os.path.join(root, file)
                shutil.copy(src_file, target_path)

if __name__ == '__main__':
    print('create new project...')
    if len(sys.argv) > 2:
        if sys.argv[1] == 'default':
            print('use default project')
            sys.argv[1] = 'a6'

            
        src_path = SOURCE_DIR_PATH + os.sep + sys.argv[1]
       
        if os.path.exists(src_path):
            dst_path = SOURCE_DIR_PATH + os.sep + sys.argv[2]
            
            if not os.path.exists(dst_path):
                os.makedirs(dst_path)
            print('copy file form %s to %s ...' % (sys.argv[1], sys.argv[2]))
            for item in copy_file_list:
                _copy_project_file(src_path+os.sep+item, dst_path+os.sep+item)
            
            print('rename file...')
            for item in rename_file_list:                
                _rename_project_file(dst_path+os.sep+'config'+os.sep+sys.argv[1]+item,
                                    dst_path+os.sep+'config'+os.sep+sys.argv[2]+item)
        else:
            print('Do not exist: %s'  % src_path)
    else:
        print('====================================')
        print("##use cmd CreateProject.py src dst")
        print("##src is the source project name to copy")
        print("##dst is the  project your  want to create")
        print("##you can use 'default' as source project too ")
        print('====================================')


