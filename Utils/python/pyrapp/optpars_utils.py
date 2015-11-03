import json
import csv
import copy
import os

# -----------------------------------------------------------------------------------------------------------
class ScratchAppend:
    def __init__(self,typ=str,comma=","):
        self.cold = True
        self.typ = typ
        self.comma = comma
        
    def __call__(self,option, opt_str, value, parser, *args, **kwargs):
        target = getattr(parser.values,option.dest)
        if self.cold:
            del target[:]
            self.cold = False
        if type(value) == str and self.comma in value:
            for v in value.split(self.comma):
                target.append(self.typ(v))
        else:
            target.append(self.typ(value))
                                                    
# -----------------------------------------------------------------------------
class Load:
    def __init__(self,scratch=False,empty={},comma=","):
        self.cold = True
        self.scratch = scratch
        self.empty = copy.copy(empty)
        self.comma = comma
        
    def __call__(self,option, opt_str, value, parser, *args, **kwargs):
        appendlst = True
        if self.scratch and self.cold:
            if option.dest == "__opts__":
                appendlst = False
            else:
                setattr(parser.values,option.dest,copy.copy(self.empty))
            self.cold = False
            
        if option.dest == "__opts__":
            dest = parser.values
            appendlst = False
        else:
            dest = getattr(parser.values,option.dest)
            
        if type(dest) == dict:
            setter = dict.__setitem__
            getter = dict.get
        else:
            setter = setattr
            getter = getattr
        
        for cfg in value.split(self.comma):
            if os.path.exists(cfg):
                cf = open(cfg)
                strn = cf.read()
                cf.close()
            else:
                strn = cfg
            settings = json.loads(strn)
            for k,v in settings.iteritems():
                attr  = getter(dest,k,None)
                if appendlst and attr and type(attr) == list:           
                    attr.extend(v)
                else:
                    setter(dest,k,v)
        
# -----------------------------------------------------------------------------
class Csv:
    def __call__(self,option, opt_str, value, parser, *args, **kwargs):
        dest = getattr(parser.values,option.dest)
        if not dest:
            setattr(parser.values,option.dest,[])
            dest = getattr(parser.values,option.dest)
        cf = open(value)
        reader = csv.DictReader(cf)
        for row in reader:
            dest.append(row)            
        cf.close()
        
