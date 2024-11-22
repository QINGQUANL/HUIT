from build.path_rule import walk, walkdir, middleware_includes
from build.gcc import *
from build.makecode import make_up_command, get_json_object_from_file, get_json_depend_files_from

# clone env from total gcc env...
Import('env')
bin_env = env.Clone()

# decide source files, objs, libs...
src_dir = bin_env['APP_PATH'] + os.sep
srcs = [src_dir + 'main']
libs = ['middleware', 'sys', 'hal', 'oal']

# include path
inc_root_paths = middleware_includes() + [
    bin_env['APP_PATH'],
]

# ld files:
ld_file_name = bin_env['APP_GCC_PATH'] + os.sep + bin_env['PROJECT'] + '.ld'
rom_ld_file_name = bin_env['BUILD_PATH']+os.sep+'rom_ld'+os.sep+'sl6801'+os.sep + 'rom.ld'

# setup program 
objs = env_setup_program(bin_env, srcs, libs, ld_file_name, walkdir(inc_root_paths), rom_ld_file_name)

# build...
output_name = bin_env['OUT_PATH']+os.sep+bin_env['PROJECT']
bin_env.Program(output_name, objs, LIBS=libs+gcc_libs, LIBPATH=bin_env['LIBS_PATH'])
bin_env.Command(output_name+'.bin', output_name+'.axf', '$OBJCOPY -O binary $SOURCE $TARGET')

# after build do...
jcfg_name = bin_env['APP_GCC_PATH'] + os.sep + 'makeup.json'
bin_env['jcfg'] = get_json_object_from_file(jcfg_name)
bin_env.Command(output_name+ '.up', [jcfg_name, ld_file_name, rom_ld_file_name] + get_json_depend_files_from(bin_env), action=make_up_command)

