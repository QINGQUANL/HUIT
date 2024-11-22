import sys, os, subprocess

BUILD_PATH = os.path.dirname(os.path.realpath(__file__))
cross_compiler = os.path.join(BUILD_PATH, 'toolchain', 'bin', 'arm-none-eabi-')
cross_compiler_tools = ['gcc', 'g++', 'gnulink', 'ar']
cross_compiler_tool_chain = [
    ('AR', 'ar'),
    ('AS', 'as'),
    ('CC', 'gcc'),
    ('CPP', 'g++'),
    ('LD', 'ld'),
    ('SIZE', 'size'),
    ('OBJCOPY', 'objcopy'),
    ('OBJDUMP', 'objdump'),
    ('READELF', 'readelf')]

# C flags common to all targets
cpu_flags = '-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16'
c_flags = '-c -gdwarf-2 -fno-common -fmessage-length=0 -Wall -Werror  -fno-exceptions -ffunction-sections -fdata-sections ' \
          '-fomit-frame-pointer -MMD -MP -std=gnu99 -DNDEBUG -Os -DUSE_ALPS_CONFIG '
c_symbols = '-DTARGET_M4 -DTARGET_CORTEX_M -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -D__CORTEX_M4 -DARM_MATH_CM4 ' \
            '-D__FPU_PRESENT=1 -DUSE_FREERTOS '
ld_flag = '-Wl,--gc-sections --specs=nano.specs --specs=rdimon.specs -Wl,-Map=%s.map,--cref -N'
gcc_libs = ['stdc++', 'supc++', 'm', 'c', 'gcc', 'nosys']


def env_dump(env):
    if not os.path.exists(env['OUT_PATH']):
        os.mkdir(env['OUT_PATH'])
    with open(env['OUT_PATH'] + os.sep + 'env_dump.txt', 'w') as f:
        env_dumped = env.Dump()
        for key in env_dumped:
            f.write(key)


class ourSpawn:
    def ourspawn(self, sh, escape, cmd, args, env):
        if 'arm-none-eabi-gcc' in cmd:
            if '-lnosys' in args:
                llibs = []
                for item in args:
                    if item.startswith('-l') and item not in llibs:
                        llibs.append(item)
                print('linking... '+' '.join(llibs))
            else:
                print('compiling... '+args[-1])
        elif 'arm-none-eabi-ar' in cmd:
            print('packing library... '+args[2])
            print('')
        elif 'arm-none-eabi-objcopy' in cmd:
            print('generate bin file... '+' '.join(args))
        else:
            print('process... '+' '.join(args))
        newargs = ' '.join(args[1:])
        cmdline = cmd + " " + newargs
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, startupinfo=startupinfo, shell=False, env=env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err.decode())
            print("=====")
        return rv


def __env_for_win32_long_cmd(env):
    if sys.platform == 'win32':
        buf = ourSpawn()
        buf.ourenv = env
        env['SPAWN'] = buf.ourspawn


def __env_build_objs(env, c_files_list):
    o_files_list = []
    # Add specifics c and objs
    for c_file in c_files_list:
        o_file = env['OUT_PATH'] + os.sep + os.path.basename(c_file) + '.o'
        d_file = env['OUT_PATH'] + os.sep + os.path.basename(c_file) + '.d'
        srcfile = c_file + '.c'
        o_files_list.append(o_file)
        env.Object(target=[o_file,d_file], source=srcfile)
    return o_files_list


def env_setup_program(env, c_files_list, libs, ld_file, inc_paths, rom_ld_name):
    __env_for_win32_long_cmd(env)
    env['PROGSUFFIX'] = ".axf"
    env['OBJSUFFIX'] = [".o",".d"]
    env['TARGET_ARCH'] = "arm"
    env['CCFLAGS'] += cpu_flags.split() + c_flags.split() + c_symbols.split()
    env['CPPPATH'] += inc_paths
    env['LINKFLAGS'] = cpu_flags.split()                                                      # base link flag, is it needed?
    env['LINKFLAGS'] += (ld_flag % (env['OUT_PATH'] + os.sep + env['PROJECT'])).split()            # to give .map file options
    env['LINKFLAGS'] += ['-T%s' % ld_file]                                                      # to give .ld file options
    env['LINKFLAGS'] += ['-T%s' % rom_ld_name]                                                 # to give .ld file options
    env.Append(LIBS=libs)
    env.Append(LIBPATH=env['LIBS_PATH'])
    env.Append(LINKFLAGS=(' -l'.join(['']+libs)).split())
    env.Depends(env['OUT_PATH']+os.sep+env['PROJECT']+env['PROGSUFFIX'], [ld_file, rom_ld_name])
    return __env_build_objs(env, c_files_list)


def env_setup_lib(env, c_files_list, inc_paths):
    __env_for_win32_long_cmd(env)
    env['PROGSUFFIX'] = ".a"
    env['OBJSUFFIX'] = [".o",".d"]
    env['TARGET_ARCH'] = "arm"
    env['CCFLAGS'] += cpu_flags.split() + c_flags.split() + c_symbols.split()
    env['CPPPATH'] += inc_paths
    return __env_build_objs(env, c_files_list)


def env_setup_asm_commands(env, s_files):
    res = []
    for s in s_files:
        _dir = os.path.dirname(s)
        _basename = os.path.basename(s)
        o_file_full_name = _dir + os.sep + env['OUT_PATH'] + os.sep + _basename + '.o'
        s_file_full_name = s + '.s'
        line = env['CC'] + ' ' + cpu_flags + ' -c -x assembler-with-cpp -o ' + o_file_full_name + ' ' + s_file_full_name
        res.append((s_file_full_name, o_file_full_name, line))
    return res

def env_obj_dump(target, source, env):
    os.system(env['OBJDUMP'] + ' -d '+ source[0].path + ' > ' + target[0].path)