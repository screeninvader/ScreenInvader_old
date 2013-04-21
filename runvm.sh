#!/bin/bash

kdesu qemu-system-x86_64 -soundhw ac97 -enable-kvm -hda /dev/sdb  -redir tcp:5555::80 -redir tcp:5556::22  -m 256
