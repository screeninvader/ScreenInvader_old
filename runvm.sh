#!/bin/bash

kdesu qemu-system-x86_64 -soundhw ac97 -enable-kvm -hda /dev/sdb -net user,hostfwd=tcp::5555-:80,hostfwd=tcp::5556-:22 -net nic -m 256
