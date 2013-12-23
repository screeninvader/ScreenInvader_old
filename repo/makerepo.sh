#!/bin/bash

reprepro -Vb . includedeb latest ../packaging/screeninvader-core-all.deb
reprepro -Vb . includedeb latest ../packaging/screeninvader-config-all.deb 
reprepro -Vb . includedeb latest ../packaging/screeninvader-misc-all.deb 
reprepro -Vb . includedeb latest ../packaging/screeninvader-binaries-all.deb 
