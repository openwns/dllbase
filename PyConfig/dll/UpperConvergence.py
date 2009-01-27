from wns.Logger import Logger
from wns.PyConfig import attrsetter
import wns.FUN

class ConvBase(wns.FUN.FunctionalUnit):
    logger = None

    def __init__(self, name, parent = None, **kw):
        super(ConvBase,self).__init__()
        self.logger = Logger(moduleName = "DLL",
                             name = name+"UpperConv",
                             enabled = True,
                             parent = parent)
        attrsetter(self, kw)

class No(ConvBase):
    __plugin__ = 'dll.upperconvergence.None'
    def __init__(self, parent = None, **kw):
        super(No,self).__init__("No", parent, **kw)

class UT(ConvBase):
    __plugin__ = 'dll.upperconvergence.UT'
    def __init__(self, parent = None, **kw):
        super(UT,self).__init__("UT", parent, **kw)

class AP(ConvBase):
    __plugin__ = 'dll.upperconvergence.AP'
    def __init__(self, parent = None, **kw):
        super(AP,self).__init__("AP", parent, **kw)
