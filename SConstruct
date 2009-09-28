import os
import sys

EnsureSConsVersion(1,1)

def quiet_print(msg):
    if not GetOption("silent"):
        print msg

env = Environment(ENV = { 'PATH' : os.environ[ 'PATH' ],
			  'HOME' : os.environ[ 'HOME' ] 
			  },
                  tools = ["default", "casaoptions", "buildenv", "casa", "utils",
                           "assaytest", "installer", "dependencies"],
                  toolpath = ["scons-tools"],
                  casashrdir="scons-tools",
                  DATA_DIR=".",
		  )
# keep a local sconsign database, rather than in very directory
env.SConsignFile()

AddOption("--enable-hdf5", dest="enable_hdf5",
          action="store_true", default=False,
          help="Enable the HDF5 library")
AddOption("--disable-dl", dest="disable_dl",
          action="store_true", default=False,
          help="Disable the use of dlopen")
AddOption("--disable-static", dest="disable_static",
          action="store_true", default=False,
          help="Disable building static libraries")
AddOption("--enable-shared", dest="enable_shared",
          action="store_true", default=False,
          help="Enable building shared (dynamic) libraries")
AddOption("--data-dir", dest="data_dir", default=None,
          action="store", type="string",
          help="The location of the measures data directory to compile in as the defautl search location")
AddOption("--build-type", dest="build_type", default="opt",
          action="store", type="string",
          help="Build optimized 'opt' (default) or debug 'dbg'")

env.AddPkgOptions("hdf5")
env.AddPkgOptions("dl")
env.AddPkgOptions("blas")
env.AddPkgOptions("lapack")
env.AddPkgOptions("f2c", lib="gfortran")
env.AddPkgOptions("cfitsio")
env.AddPkgOptions("wcs")

if not (env.Detect(["flex","lex"]) and env.Detect(["bison", "yacc"])):
    print "lex/yacc needs to be installed"
    env.Exit(1)

# create the installer which handles installing the final build
installer = env.Installer()

# Auto configure
if not env.GetOption('clean') and not env.GetOption("help"):
    conf = Configure(env)

    # DON'T USE conf.env.Finish
    # Add all packages explicitly to 'env'
    # but use autoadd=1 for all CheckLibs
    # DL
    if not conf.env.GetOption("disable_dl"):
        pkgname = "dl"
        libname = env.get(pkgname+"_lib")
        conf.env.AddCustomPackage(pkgname)
        if conf.CheckLibWithHeader(libname, 'dlfcn.h', language='c',
                                   autoadd=0):
            env.AppendUnique(LIBS=[libname])
            env.Append(CPPFLAGS=['-DHAVE_DLOPEN'])
        else:
            env.Exit(1)
    else:
        print "Building without dlopen support"    

    conf.env.CheckFortran(conf)

    f2cname = conf.env.get("f2c_lib", conf.env["F2CLIB"])
    conf.env.AddCustomPackage("f2c")
    if not conf.CheckLib(f2cname, autoadd=0):
        Exit(1)
    conf.env.PrependUnique(LIBS=f2cname)        
    env.AddCustomPackage("f2c")
    env["F2CLIB"] = [f2cname]

    blasname = conf.env.get("blas_lib", "blas").split(",")
    conf.env.AddCustomPackage("blas")
    blasname.reverse()
    for b in blasname:
        if not conf.CheckLib(b, autoadd=0):
            Exit(1)
    conf.env.PrependUnique(LIBS=b)            
    env.AddCustomPackage("blas")
    env["BLAS"] = blasname

    lapackname = conf.env.get("lapack_lib", "lapack").split(",")
    conf.env.AddCustomPackage("lapack")
    lapackname.reverse()
    for l in lapackname:
        if not conf.CheckLib(l, autoadd=0):
            Exit(1)
    conf.env.PrependUnique(LIBS=l)    
    env.AddCustomPackage("lapack")
    env["LAPACK"] = lapackname

    # HDF5
    if conf.env.GetOption("enable_hdf5"):
        pkgname = "hdf5"
        libname = conf.env.get(pkgname+"_lib")
        conf.env.AddCustomPackage(pkgname)
        if conf.CheckLib(libname, autoadd=0):
            env.PrependUnique(LIBS=[libname])
            env.Append(CPPFLAGS=['-DHAVE_LIBHDF5'])
        else:
            env.Exit(1)
    else:
        quiet_print("Building without HDF5 support")

    pkgname = "cfitsio"
    cfitsioname = conf.env.get(pkgname+"_lib")
    conf.env.AddCustomPackage(pkgname)
    if not conf.CheckLibWithHeader(cfitsioname, "fitsio.h", "c", autoadd=0):
        Exit(1)
    env["CFITSIO"] = [cfitsioname]
    env.AddCustomPackage(pkgname)

    pkgname = "wcs"
    libname = conf.env.get(pkgname+"_lib")
    conf.env.AddCustomPackage(pkgname)
    if not conf.CheckLibWithHeader(libname, "wcslib/wcs.h", "c", autoadd=0):
        Exit(1)
    env["WCS"] = [libname]
    env.AddCustomPackage(pkgname)

    ddir = env.GetOption("data_dir")
    if ddir:
        adir = os.path.expanduser(os.path.expandvars(ddir))
        if os.path.exists(adir) \
               and os.path.exists(os.path.join(adir, 'ephemerides')) \
               and os.path.exists(os.path.join(adir, 'geodetic')):
            pass
        else:
            print """Warning: measures data directory given doesn't contain
            geodetic and ephemerides.
            Compiling in '%s' as default search location""" % ddir
            
            # Note the escaped single quotes to handle the string in the define
    else:
        shrdir = conf.env.GetOption("sharedir") or \
            os.path.join(conf.env.GetOption("prefix"), "share")
        ddir = os.path.join(shrdir,
                            "casacore", "data")
    env["DATA_DIR"] = '-DCASADATA=\'"%s"\'' % ddir
#    env = conf.Finish()

# create an environment copy with the dbg/opt compiler flags
buildenv = env.BuildEnv(env.GetOption("build_type"))
env.SConscript("SConscript" , 
               build_dir= buildenv["BUILDDIR"],
               duplicate=0, exports=["buildenv", "installer"])

# add the Tools to the casacore/share directory. This way they can be imported
# by the other casacore packages without having to duplicate them.
installer.AddShares("scons-tools", "*.py", "casacore/", True)
installer.AddShares("scons-tools", "casacore_assay", "casacore/")
installer.AddShares("scons-tools", "floatcheck.sh", "casacore/")