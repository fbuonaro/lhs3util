#!/bin/bash

TARGET_STAGE="$1"
if [[ "x${TARGET_STAGE}" == "x" ]];
then
    TARGET_STAGE="dist"
fi

# lhmiscutil
pushd ./modules/lhmiscutil
./scripts/buildLHMiscUtilImage.sh
popd


# libs3
./scripts/buildLibS3LHDistImage.sh

./modules/lhscriptutil/scripts/buildImage.sh ./Dockerfiles/Dockerfile.lhs3util lhs3util "${TARGET_STAGE}"