from build.path_rule import  walkdir, walk, middleware_includes
from build.gcc import *

Import('env')
lib_env = env.Clone()

# decide source files, objs, libs...
src_dir = os.path.dirname(os.path.realpath('__file__'))
out_name = 'libdemo'
modules = [
 	src_dir+'/./',
    #src_dir+'/audio/service',
]
srcs = []
for module in modules:
    srcs += walk(module, '.c')

inc_root_paths = middleware_includes() + [
    src_dir,
]

objs = env_setup_lib(lib_env, srcs, walkdir(inc_root_paths))
# env_dump(lib_env)

# continue build asm files...
srcs_asm = walk(src_dir, '.s')
asm_objs = env_setup_asm_commands(lib_env, srcs_asm)
for (s,o,l) in asm_objs:
    lib_env.Command(o, s, l)
    objs.append(o)
#env_dump(lib_env)

output_name = lib_env['OUT_PATH']+os.sep+out_name
lib = lib_env.StaticLibrary(target=output_name, source=objs)
staticInstall = lib_env.Install(lib_env['LIBS_PATH'], lib)
lib_env.Alias('lib', lib_env['LIBS_PATH'])
