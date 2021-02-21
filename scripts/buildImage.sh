#!/bin/bash

DOCKERFILE_PATH="./Dockerfile"
TARGET_STAGE="lhs3util-s4-main-env"
TARGET_TAG="lhs3util:main"
TEST_STAGE="lhs3util-stage-test-env"
TEST_TAG="lhs3util:test-env"
BASE_STAGE="lhs3util-stage-base-env"

# Build the main image
docker build --target ${TARGET_STAGE} -t ${TARGET_TAG} -f ${DOCKERFILE_PATH} .

# Tag the intermediate image, expecting it to be test-env
docker tag $(docker image ls -f "label=${TEST_STAGE}=yes" -q | head -1) ${TEST_TAG}

# Clean up overridden images
docker image prune -f --filter "label=${BASE_STAGE}"
