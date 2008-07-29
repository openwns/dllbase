import os
import fnmatch
import glob
Import('env installDir includeDir')
libs,headers,pyconfigs = SConscript('config/libfiles.py')

for lib,files in libs.items():
	lib = env.SharedLibrary('dllbase-' + lib.lower(), files)
	env.Install(installDir, lib )

class GlobDirectoryWalker:
    # a forward iterator that traverses a directory tree

    def __init__(self, directory, pattern="*"):
        self.stack = [directory]
        self.pattern = pattern
        self.files = []
        self.index = 0

    def __getitem__(self, index):
        while 1:
            try:
                file = self.files[self.index]
                self.index = self.index + 1
            except IndexError:
                # pop next directory from stack
                self.directory = self.stack.pop()
                self.files = os.listdir(self.directory)
                self.index = 0
            else:
                # got a filename
                fullname = os.path.join(self.directory, file)
                if os.path.isdir(fullname) and not os.path.islink(fullname):
                    self.stack.append(fullname)
                if fnmatch.fnmatch(file, self.pattern):
                    return fullname

#headers = [ x for x in GlobDirectoryWalker('src', '*.hpp')]
#targets = [header.replace('src/', '') for header in headers]
#env.InstallAs([includeDir + '/DLL/'+ target for target in targets], [header for header in headers])
