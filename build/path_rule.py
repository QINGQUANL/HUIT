import os, copy

ROOT_PATH, APP_PATH, BUILD_PATH, LIBS_PATH, PROJECT, OUT_PATH = '', '', '', '', '', ''

def lowware_includes(root=None):
    root_path = ROOT_PATH if root is None else root
    res = []
    res.append(root_path+os.sep+'inc'+os.sep+'oal')
    res.append(root_path+os.sep+'inc'+os.sep+'hal')
    res.append(root_path+os.sep+'inc'+os.sep+'sys')
    return res

def middleware_includes(root=None):
    root_path = ROOT_PATH if root is None else root
    return [root_path+os.sep+'inc'+os.sep+'middleware'] + lowware_includes(root)

def rule(app_gcc_path, root_path=''):
    global ROOT_PATH, APP_PATH, BUILD_PATH, LIBS_PATH, INC_PATH, PROJECT, OUT_PATH
    ROOT_PATH = os.path.abspath(os.path.join(app_gcc_path, '..', '..', '..')) if root_path == '' else root_path
    APP_PATH = os.path.abspath(os.path.join(app_gcc_path, '..'))
    BUILD_PATH = os.path.abspath(os.path.join(ROOT_PATH, 'build'))
    LIBS_PATH = os.path.abspath(os.path.join(ROOT_PATH, 'libs'))
    INC_PATH = os.path.abspath(os.path.join(ROOT_PATH, 'inc'))
    PROJECT = os.path.basename(APP_PATH)
    # OUT_PATH = APP_PATH + os.sep + 'gcc' + os.sep + '.out'
    OUT_PATH = '.out'
    print('====================================')
    print('APP_GCC_PATH: %s' % app_gcc_path)
    print('ROOT_PATH: %s' % ROOT_PATH)
    print('APP_PATH: %s' % APP_PATH)
    print('BUILD_PATH: %s' % BUILD_PATH)
    print('PROJECT: %s' % PROJECT)
    print('OUT_PATH: %s' % OUT_PATH)
    print('====================================')
    return ROOT_PATH, APP_PATH, BUILD_PATH, LIBS_PATH, INC_PATH, PROJECT, OUT_PATH

def walk(path, suffix, prefix=None, ignoredir=None):
    path_name = []
    for parent,dirnames,filenames in os.walk(path, topdown=True):
        if ignoredir is not None:
            dirnames[:] = [d for d in dirnames if d not in ignoredir]
        for filename in filenames:
            if suffix is not None:
                if(filename.endswith(suffix)):
                    (n, e) = os.path.splitext(filename)
                    path_name.append(os.path.join(parent,n))
            elif prefix is not None:
                if(filename.startswith(prefix)):
                    path_name.append(os.path.join(parent, filename))
            else:
                path_name.append(os.path.join(parent, filename))
    return path_name

def walkdir(path, ignoredir=None, ignoreroot=False):
    if type(path) == str:
        dir_name = [] if ignoreroot else [path]
        for parent,dirnames,filenames in os.walk(path):
            if ignoredir is not None:
                dirnames[:] = [d for d in dirnames if d not in ignoredir]
            for dirname in dirnames:
                dir = os.path.join(parent, dirname)
                dir_name.append(dir)                
        return dir_name
    elif type(path) == list:
        ret = []
        for item_path in path:
            ret += walkdir(item_path, ignoredir)
        return ret
    return []

