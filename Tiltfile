###############################################################################
# BOILERPLATE START
###############################################################################
# TILT_GIT_RESOURCE_CHECKOUT_DIR can be used set the directory where dependent projects
gitModulesCheckoutOutDirEnvVar = "TILT_GIT_RESOURCE_CHECKOUT_DIR"
if not os.getenv( gitModulesCheckoutOutDirEnvVar, default = None ):
    os.putenv( gitModulesCheckoutOutDirEnvVar, os.path.abspath( "./modules" ) )
gitDir = os.getenv( gitModulesCheckoutOutDirEnvVar, default = None )
if not gitDir:
    fail( "failed to set modules checkout dir" )

load( "ext://git_resource", "git_checkout" )

git_checkout( "git@github.com:fbuonaro/lhscriptutil.git#release/v/1.3.0" )
commonTilt = load_dynamic( os.path.join( gitDir, "lhscriptutil", "tilt", "common", "Tiltfile") )
lhscriptutilTilt = load_dynamic( os.path.join( gitDir, "lhscriptutil", "Tiltfile") )
lhQuickBuildImage = commonTilt[ "lhQuickBuildImage" ]
lhQuickBuildImageNoK8 = commonTilt[ "lhQuickBuildImageNoK8" ]
cfg = lhscriptutilTilt[ "cfg" ]
###############################################################################
# <<< BOILERPLATE END
###############################################################################

git_checkout( "git@github.com:fbuonaro/lhmiscutil.git#tilt" ) # TODO - release version

lhQuickBuildImageNoK8( "libs3-lhdist", cfg, dockerfile="Dockerfiles/Dockerfile.libs3-lhdist" )

lhQuickBuildImageNoK8( "lhs3util", cfg, dockerfile="Dockerfiles/Dockerfile.lhs3util" )
