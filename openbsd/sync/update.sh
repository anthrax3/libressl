#!/bin/sh

set -e

# grab the latest openbsd-src
mkdir -p cvs/CVSROOT
cvsync -c cvs-syncfile

# remove duplicate Attic files
rm -f cvs/src/lib/libssl/src/doc/crypto/Attic/bio.pod,v \
	  cvs/src/lib/libssl/src/doc/crypto/Attic/err.pod,v \
	  cvs/src/lib/libssl/src/doc/crypto/Attic/hmac.pod,v \
	  cvs/src/lib/libssl/src/doc/crypto/Attic/md5.pod,v \
	  cvs/src/lib/libssl/src/doc/crypto/Attic/rand.pod,v \
	  cvs/src/lib/libssl/src/doc/crypto/Attic/rc4.pod,v

# build and checkout the git repository
rm -fr cvs2git-tmp
cvs2git --options cvs2git.options --fallback-encoding utf-8
rm -fr openbsd-src.git
mkdir openbsd-src.git
(cd openbsd-src.git
 git init --bare
 cat ../cvs2git-tmp/git-blob.dat ../cvs2git-tmp/git-dump.dat | git fast-import
)
rm -fr openbsd-src
git clone openbsd-src.git

# grab the directories we're interested in, trim them out
for dir in libc libssl libcrypto; do
    echo $dir
    rm -fr $dir-openbsd $dir-regress-openbsd
    cp -a openbsd-src ${dir}-openbsd
    cp -a openbsd-src ${dir}-regress-openbsd
    (cd ${dir}-openbsd;
    git filter-branch --prune-empty --subdirectory-filter src/lib/${dir} HEAD
    git remote add github git@github.com:busterb/${dir}-openbsd.git
    git push -f github master
    )
    (cd ${dir}-regress-openbsd;
    git filter-branch --prune-empty --subdirectory-filter src/regress/lib/${dir} HEAD
    git remote add github git@github.com:busterb/${dir}-regress-openbsd.git
    git push -f github master
    )
done
