#!/usr/bin/env bash
# Assumptions:
# 1) BOOST_ROOT and BOOST_URL are already defined,
# and contain valid values.
# 2) The last namepart of BOOST_ROOT matches the
# folder name internal to boost's .tar.bz2
set -eu
if [ ! -d "$BOOST_ROOT/lib" ]
then
  wget --no-check-certificate $BOOST_URL -O /tmp/boost.tar.bz2
  cd `dirname $BOOST_ROOT`
  rm -fr ${BOOST_ROOT}
  tar -xf /tmp/boost.tar.bz2

  params="define=_GLIBCXX_USE_CXX11_ABI=0 \
          --with-program_options --with-system --with-coroutine --with-filesystem"
  cd $BOOST_ROOT && \
    ./bootstrap.sh --prefix=$BOOST_ROOT && \
    ./b2 -d1 $params && \
    ./b2 -d0 $params install
else
  echo "Using cached boost at $BOOST_ROOT"
fi

