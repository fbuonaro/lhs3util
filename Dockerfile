##################################################################################
# STAGE 0 - base environment with first set of runtime dependencies
##################################################################################
FROM centos:centos7 as lhs3util-s0-base-env
LABEL lhs3util-stage-base-env="yes"
LABEL lhs3util-stage-build-env="no"
LABEL lhs3util-stage-build="no"
LABEL lhs3util-stage-test-env="no"
LABEL lhs3util-stage-main="no"

RUN yum -y --enablerepo=extras install epel-release && \
    yum clean all

##################################################################################
# STAGE 1 - build tools and libraries needed to build lhs3util
##################################################################################
FROM lhs3util-s0-base-env as lhs3util-s1-build-env
LABEL lhs3util-stage-base-env="no"
LABEL lhs3util-stage-build-env="yes"
LABEL lhs3util-stage-build="no"
LABEL lhs3util-stage-test-env="no"
LABEL lhs3util-stage-main="no"

# for compiling and unit testing
RUN yum -y install \
        cmake3 \
        make \
        gcc \
        gcc-c++ \
        gtest-devel \
        libcurl-devel \
        libxml2-devel \
        openssl-devel && \
    yum clean all

RUN yum -y install git && \
    git clone https://github.com/bji/libs3.git /libs3 && \
    cd /libs3 && \
    make DESTDIR=/usr && \
    make install DESTDIR=/usr && \
    ldconfig

ENTRYPOINT [ "bash" ]

##################################################################################
# STAGE 2 - the lhs3util source and compiled binaries
##################################################################################
FROM lhs3util-s1-build-env as lhs3util-s2-build
LABEL lhs3util-stage-base-env="no"
LABEL lhs3util-stage-build-env="no"
LABEL lhs3util-stage-build="yes"
LABEL lhs3util-stage-test-env="no"
LABEL lhs3util-stage-main="no"

ADD . /lhs3util
RUN cd /lhs3util && \
    mkdir ./build && \
    cd ./build && \
    cmake3 \
        -DCMAKE_BUILD_TYPE=Release \
        ../ && \
    make

##################################################################################
# STAGE 3 - the base image with additional built runtime dependencies, lhs3util 
#           binaries and test binaries needed for running integration tests
#           includes everything from build-env
##################################################################################
FROM lhs3util-s2-build as lhs3util-s3-test-env
LABEL lhs3util-stage-base-env="no"
LABEL lhs3util-stage-build-env="no"
LABEL lhs3util-stage-build="no"
LABEL lhs3util-stage-test-env="yes"
LABEL lhs3util-stage-main="no"

RUN cd /lhs3util/build && \
    make install-lhs3util && \
    ldconfig

##################################################################################
# STAGE 4 - the base image with additional built runtime dependencies and 
#           lhs3util binaries includes nothing from build-env
##################################################################################
FROM lhs3util-s0-base-env as lhs3util-s4-main-env
LABEL lhs3util-stage-base-env="no"
LABEL lhs3util-stage-build-env="no"
LABEL lhs3util-stage-build="no"
LABEL lhs3util-stage-test-env="no"
LABEL lhs3util-stage-main="yes"

COPY --from=lhs3util-s2-build /usr/ /usr/
COPY --from=lhs3util-s2-build /lhs3util/ /lhs3util/
RUN cd /lhs3util/build && \
    make install-lhs3util && \
    cd / && \
    rm -rf /lhs3util && \
    ldconfig
