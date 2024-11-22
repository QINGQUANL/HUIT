from build.path_rule import  walk, walkdir, middleware_includes
from build.gcc import *
from build.makecode import make_up_command, make_tone, get_json_object_from_file, get_json_depend_files_from
import os , time
# clone env from total gcc env...
Import('env')
bin_env = env.Clone()

project_name = bin_env['BOARD']
print('board_name: %s' % project_name)

#copy xx_user_config.h -> user_config.h
project_dir = bin_env['APP_PATH'] + os.sep+'app_config'+ os.sep + project_name + os.sep
project_config_path = project_dir+'config'

temp_path = bin_env['APP_PATH'] + os.sep + 'temp'
if not os.path.exists(temp_path):
    os.makedirs(temp_path)

project_src = project_config_path + os.sep + project_name + '_user_config.h'
project_dst = bin_env['APP_PATH'] + os.sep + 'temp' + os.sep +'user_config.h'
if os.path.exists(project_dst):
    os.remove(project_dst)
bin_env.Command(project_dst, project_src, Copy("$TARGET", "$SOURCE"))

project_src = project_config_path + os.sep + project_name + '_usb_app_config.h'
project_dst = bin_env['APP_PATH'] + os.sep + 'temp' + os.sep +'usb_app_config.h'
if os.path.exists(project_dst):
    os.remove(project_dst)
bin_env.Command(project_dst, project_src, Copy("$TARGET", "$SOURCE"))

#copy _audio_params_config.h -> audio_params_config.h
audio_src = project_config_path + os.sep + project_name + '_audio_params_config.h'
audio_dst = bin_env['APP_PATH'] + os.sep + 'temp' + os.sep +'audio_params_config.h'
if os.path.exists(audio_dst):
    os.remove(audio_dst)
bin_env.Command(audio_dst, audio_src, Copy("$TARGET", "$SOURCE"))


#copy xx_ui.c -> ui_process.c
ui_src = project_config_path + os.sep + project_name + '_ui.c'
ui_dst = bin_env['APP_PATH'] + os.sep + 'temp' + os.sep  +'ui_process.c'
if os.path.exists(ui_dst):
    os.remove(ui_dst)
os.system('copy %s %s' % (ui_src, ui_dst))


y=time.strftime("%y", time.localtime())
ver = str(int(y))+time.strftime("%m%d%H%M", time.localtime())

print('ver: %s' % ver)

fp=open(bin_env['APP_PATH'] + os.sep + 'temp' + os.sep+'sys_version.h', "w")#打开记录文件
fp.write("#define sys_ver %s\n" % ver)
fp.close()
# decide source files, objs, libs...
src_dir = bin_env['APP_PATH']
dst_dir = 'dst' + os.sep + project_name

srcs = walk(src_dir, '.c',None, 'app_config')
libs = ['karaoke_framework', 'filesystem', 'middleware', 'motion', 'alg', 'g24', 'net',  'rfc', 'wakeup', 'sys', 'hal', 'oal', 'utils','xzy']
#libs = ['karaoke_framework', 'filesystem', 'middleware', 'motion', 'alg', 'g24', 'rfc', 'wakeup', 'sys', 'hal', 'oal']
#libs += ['third_party']

# include path
inc_root_paths = middleware_includes() + [
    bin_env['APP_PATH'],
    bin_env['INC_PATH']+'/app_framework/karaoke_framework',
	bin_env['INC_PATH']+'/g24',
    bin_env['INC_PATH']+'/net',
	bin_env['INC_PATH']+'/rfc',
    bin_env['INC_PATH']+'/filesystem',
    bin_env['INC_PATH']+'/third_party',
	bin_env['INC_PATH']+'/utils',
]
dir_name = []
dir_name += walkdir(inc_root_paths, ignoredir=['app_config'])
#dir_name += walkdir(project_config_path)

#srcs+= walk(project_config_path, '.c')
# ld files:
ld_file_name = bin_env['APP_GCC_PATH'] + os.sep + bin_env['PROJECT'] + '.ld'
rom_ld_file_name = bin_env['BUILD_PATH']+os.sep+'rom_ld'+os.sep+'sl6801'+os.sep + 'rom.ld'

# setup program
objs = env_setup_program(bin_env, srcs, libs, ld_file_name, dir_name, rom_ld_file_name)

# build...
output_name = bin_env['OUT_PATH']+os.sep + bin_env['PROJECT']
bin_env.Program(output_name, objs, LIBS=libs+gcc_libs, LIBPATH=bin_env['LIBS_PATH'])
bin_env.Command(output_name+'.bin', output_name+'.axf',  '$OBJCOPY -O binary $SOURCE $TARGET')

# after build, make .up file...
make_tone(project_dir+'bin', project_dir+'tone')
jcfg_name = bin_env['APP_GCC_PATH'] + os.sep + 'makeup.json'
bin_env['jcfg'] = get_json_object_from_file(jcfg_name)
bin_env.Command(output_name+ '.up', [jcfg_name, ld_file_name, rom_ld_file_name] + get_json_depend_files_from(bin_env), action=make_up_command)

# install the binary target files to a single directory.
install_name = dst_dir + os.sep + project_name
bin_env.Command(install_name+ '.axf', output_name+ '.axf', Copy("$TARGET", "$SOURCE"))
#bin_env.Command(install_name+ '.map', output_name+ '.map', Copy("$TARGET", "$SOURCE"))
bin_env.Command(install_name+ '.bin', output_name+ '.bin', Copy("$TARGET", "$SOURCE"))
if bin_env['BUILD_SEL'] == 'release':
    bin_env.Command(install_name+'_'+ ver + '.up', output_name+ '.up', Copy("$TARGET", "$SOURCE"))
else:
    bin_env.Command(install_name+ '.up', output_name+ '.up', Copy("$TARGET", "$SOURCE"))

bin_env.Command(dst_dir + os.sep+ 'ota.up', output_name+ '.up', Copy("$TARGET", "$SOURCE"))

if os.path.exists(install_name+ '.map'):
    os.remove(install_name+ '.map')
os.system('copy %s %s' % (output_name+ '.map', install_name+ '.map'))
