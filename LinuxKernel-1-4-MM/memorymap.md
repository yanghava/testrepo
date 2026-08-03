# Memory course refer to [here](https://avaya.atlassian.net/wiki/spaces/DLBBEWIKI/pages/2448850963/1-4+MM#Memory)

In an AES system, the TSAPI tsrv has the memory map below
```
[root@aes102 ~]# ps -ef | grep tsrv
root       30255   26430  0 09:20 ?        00:00:00 /usr/bin/sudo /opt/mvap/bin/tsrv
avaya      30263   30255  3 09:20 ?        00:08:01 /opt/mvap/bin/tsrv
root      143504  142178  0 13:25 pts/0    00:00:00 grep --color=auto tsrv
[root@aes102 ~]# cd /proc/30263

[root@aes102 30263]# cat maps
00400000-00488000 r-xp 00000000 fd:00 8818458                            /opt/mvap/bin/tsrv
00687000-0069b000 r--p 00087000 fd:00 8818458                            /opt/mvap/bin/tsrv
0069b000-006a1000 rw-p 0009b000 fd:00 8818458                            /opt/mvap/bin/tsrv
006a1000-006af000 rw-p 00000000 00:00 0
01526000-01849000 rw-p 00000000 00:00 0                                  [heap]
7fc54a7fd000-7fc54a7fe000 ---p 00000000 00:00 0
7fc54a7fe000-7fc54affe000 rw-p 00000000 00:00 0
7fc54affe000-7fc54afff000 ---p 00000000 00:00 0
7fc54afff000-7fc54b7ff000 rw-p 00000000 00:00 0
7fc54b7ff000-7fc54b800000 ---p 00000000 00:00 0
7fc54b800000-7fc54c000000 rw-p 00000000 00:00 0
7fc54c000000-7fc54c021000 rw-p 00000000 00:00 0
7fc54c021000-7fc550000000 ---p 00000000 00:00 0
7fc5507f9000-7fc5507fa000 ---p 00000000 00:00 0
7fc5507fa000-7fc550ffa000 rw-p 00000000 00:00 0
7fc550ffa000-7fc550ffb000 ---p 00000000 00:00 0
7fc550ffb000-7fc5517fb000 rw-p 00000000 00:00 0
7fc5517fb000-7fc5517fc000 ---p 00000000 00:00 0
7fc5517fc000-7fc551ffc000 rw-p 00000000 00:00 0
7fc551ffc000-7fc551ffd000 ---p 00000000 00:00 0
7fc551ffd000-7fc5527fd000 rw-p 00000000 00:00 0
7fc5527fd000-7fc5527fe000 ---p 00000000 00:00 0
7fc5527fe000-7fc552ffe000 rw-p 00000000 00:00 0
7fc552ffe000-7fc552fff000 ---p 00000000 00:00 0
7fc552fff000-7fc5537ff000 rw-p 00000000 00:00 0
7fc5537ff000-7fc553800000 ---p 00000000 00:00 0
7fc553800000-7fc554000000 rw-p 00000000 00:00 0
7fc554000000-7fc554021000 rw-p 00000000 00:00 0
7fc554021000-7fc558000000 ---p 00000000 00:00 0
7fc558000000-7fc558021000 rw-p 00000000 00:00 0
7fc558021000-7fc55c000000 ---p 00000000 00:00 0
7fc55c000000-7fc55c021000 rw-p 00000000 00:00 0
7fc55c021000-7fc560000000 ---p 00000000 00:00 0
7fc560000000-7fc560021000 rw-p 00000000 00:00 0
7fc560021000-7fc564000000 ---p 00000000 00:00 0
7fc5647f9000-7fc5647fa000 ---p 00000000 00:00 0
7fc5647fa000-7fc564ffa000 rw-p 00000000 00:00 0
7fc564ffa000-7fc564ffb000 ---p 00000000 00:00 0
7fc564ffb000-7fc5657fb000 rw-p 00000000 00:00 0
7fc5657fb000-7fc5657fc000 ---p 00000000 00:00 0
7fc5657fc000-7fc565ffc000 rw-p 00000000 00:00 0
7fc565ffc000-7fc565ffd000 ---p 00000000 00:00 0
7fc565ffd000-7fc5667fd000 rw-p 00000000 00:00 0
7fc5667fd000-7fc5667fe000 ---p 00000000 00:00 0
7fc5667fe000-7fc566ffe000 rw-p 00000000 00:00 0
7fc566ffe000-7fc566fff000 ---p 00000000 00:00 0
7fc566fff000-7fc5677ff000 rw-p 00000000 00:00 0
7fc5677ff000-7fc567800000 ---p 00000000 00:00 0
7fc567800000-7fc568000000 rw-p 00000000 00:00 0
7fc568000000-7fc568021000 rw-p 00000000 00:00 0
7fc568021000-7fc56c000000 ---p 00000000 00:00 0
7fc56c000000-7fc56c021000 rw-p 00000000 00:00 0
7fc56c021000-7fc570000000 ---p 00000000 00:00 0
7fc570000000-7fc570021000 rw-p 00000000 00:00 0
7fc570021000-7fc574000000 ---p 00000000 00:00 0
7fc5747f9000-7fc5747fa000 ---p 00000000 00:00 0
7fc5747fa000-7fc574ffa000 rw-p 00000000 00:00 0
7fc574ffa000-7fc574ffb000 ---p 00000000 00:00 0
7fc574ffb000-7fc5757fb000 rw-p 00000000 00:00 0
7fc5757fb000-7fc5757fc000 ---p 00000000 00:00 0
7fc5757fc000-7fc575ffc000 rw-p 00000000 00:00 0
7fc575ffc000-7fc575ffd000 ---p 00000000 00:00 0
7fc575ffd000-7fc5767fd000 rw-p 00000000 00:00 0
7fc5767fd000-7fc5767fe000 ---p 00000000 00:00 0
7fc5767fe000-7fc576ffe000 rw-p 00000000 00:00 0
7fc576ffe000-7fc576fff000 ---p 00000000 00:00 0
7fc576fff000-7fc5777ff000 rw-p 00000000 00:00 0
7fc5777ff000-7fc577800000 ---p 00000000 00:00 0
7fc577800000-7fc578000000 rw-p 00000000 00:00 0
7fc578000000-7fc578021000 rw-p 00000000 00:00 0
7fc578021000-7fc57c000000 ---p 00000000 00:00 0
7fc57c7f9000-7fc57c7fa000 ---p 00000000 00:00 0
7fc57c7fa000-7fc57cffa000 rw-p 00000000 00:00 0
7fc57cffa000-7fc57cffb000 ---p 00000000 00:00 0
7fc57cffb000-7fc57d7fb000 rw-p 00000000 00:00 0
7fc57d7fb000-7fc57d7fc000 ---p 00000000 00:00 0
7fc57d7fc000-7fc57dffc000 rw-p 00000000 00:00 0
7fc57dffc000-7fc57dffd000 ---p 00000000 00:00 0
7fc57dffd000-7fc57e7fd000 rw-p 00000000 00:00 0
7fc57e7fd000-7fc57e7fe000 ---p 00000000 00:00 0
7fc57e7fe000-7fc57effe000 rw-p 00000000 00:00 0
7fc57effe000-7fc57efff000 ---p 00000000 00:00 0
7fc57efff000-7fc57f7ff000 rw-p 00000000 00:00 0
7fc57f7ff000-7fc57f800000 ---p 00000000 00:00 0
7fc57f800000-7fc580000000 rw-p 00000000 00:00 0
7fc580000000-7fc580021000 rw-p 00000000 00:00 0
7fc580021000-7fc584000000 ---p 00000000 00:00 0
7fc584000000-7fc58404d000 rw-p 00000000 00:00 0
7fc58404d000-7fc588000000 ---p 00000000 00:00 0
7fc588000000-7fc588021000 rw-p 00000000 00:00 0
7fc588021000-7fc58c000000 ---p 00000000 00:00 0
7fc58c000000-7fc58c021000 rw-p 00000000 00:00 0
7fc58c021000-7fc590000000 ---p 00000000 00:00 0
7fc590000000-7fc590021000 rw-p 00000000 00:00 0
7fc590021000-7fc594000000 ---p 00000000 00:00 0
7fc594026000-7fc594406000 r-xp 00000000 fd:00 456445                     /usr/lib64/libg3pd.so
7fc594406000-7fc594606000 ---p 003e0000 fd:00 456445                     /usr/lib64/libg3pd.so
7fc594606000-7fc594676000 r--p 003e0000 fd:00 456445                     /usr/lib64/libg3pd.so
7fc594676000-7fc59467b000 rw-p 00450000 fd:00 456445                     /usr/lib64/libg3pd.so
7fc59467b000-7fc59468b000 rw-p 00000000 00:00 0
7fc59468b000-7fc59468c000 ---p 00000000 00:00 0
7fc59468c000-7fc594e8c000 rw-p 00000000 00:00 0
7fc594e8c000-7fc594e96000 r-xp 00000000 fd:00 572362                     /usr/lib64/libnss_sss.so.2
7fc594e96000-7fc595095000 ---p 0000a000 fd:00 572362                     /usr/lib64/libnss_sss.so.2
7fc595095000-7fc595096000 r--p 00009000 fd:00 572362                     /usr/lib64/libnss_sss.so.2
7fc595096000-7fc595097000 rw-p 0000a000 fd:00 572362                     /usr/lib64/libnss_sss.so.2
7fc595097000-7fc5ac000000 rw-s 00000000 00:16 144320                     /dev/shm/mvap/transport/asai_map
7fc5ac000000-7fc5ac021000 rw-p 00000000 00:00 0
7fc5ac021000-7fc5b0000000 ---p 00000000 00:00 0
7fc5b01bf000-7fc5b01c0000 ---p 00000000 00:00 0
7fc5b01c0000-7fc5b09c0000 rw-p 00000000 00:00 0
7fc5b09c0000-7fc5b09c1000 ---p 00000000 00:00 0
7fc5b09c1000-7fc5b11c1000 rw-p 00000000 00:00 0
7fc5b11c1000-7fc5b12a2000 rw-s 00000000 00:16 153456                     /dev/shm/mvap/tsapi/openClientConnections
7fc5b12a2000-7fc5b12a5000 r-xp 00000000 fd:00 16805736                   /usr/lib64/gconv/UTF-16.so
7fc5b12a5000-7fc5b14a4000 ---p 00003000 fd:00 16805736                   /usr/lib64/gconv/UTF-16.so
7fc5b14a4000-7fc5b14a5000 r--p 00002000 fd:00 16805736                   /usr/lib64/gconv/UTF-16.so
7fc5b14a5000-7fc5b14a6000 rw-p 00003000 fd:00 16805736                   /usr/lib64/gconv/UTF-16.so
7fc5b14a6000-7fc5b14a7000 ---p 00000000 00:00 0
7fc5b14a7000-7fc5b1ca7000 rw-p 00000000 00:00 0
7fc5b1ca7000-7fc5b1ca8000 ---p 00000000 00:00 0
7fc5b1ca8000-7fc5b24a8000 rw-p 00000000 00:00 0
7fc5b24a8000-7fc5b24a9000 ---p 00000000 00:00 0
7fc5b24a9000-7fc5b2ca9000 rw-p 00000000 00:00 0
7fc5b2ca9000-7fc5b2cb4000 r-xp 00000000 fd:00 25488                      /usr/lib64/libnss_files-2.28.so
7fc5b2cb4000-7fc5b2eb4000 ---p 0000b000 fd:00 25488                      /usr/lib64/libnss_files-2.28.so
7fc5b2eb4000-7fc5b2eb5000 r--p 0000b000 fd:00 25488                      /usr/lib64/libnss_files-2.28.so
7fc5b2eb5000-7fc5b2eb6000 rw-p 0000c000 fd:00 25488                      /usr/lib64/libnss_files-2.28.so
7fc5b2eb6000-7fc5b2ebc000 rw-p 00000000 00:00 0
7fc5b2ebc000-7fc5b2ebd000 ---p 00000000 00:00 0
7fc5b2ebd000-7fc5b36bd000 rw-p 00000000 00:00 0
7fc5b36bd000-7fc5b3740000 r-xp 00000000 fd:00 17718                      /usr/lib64/libpcre2-8.so.0.7.1
7fc5b3740000-7fc5b393f000 ---p 00083000 fd:00 17718                      /usr/lib64/libpcre2-8.so.0.7.1
7fc5b393f000-7fc5b3940000 r--p 00082000 fd:00 17718                      /usr/lib64/libpcre2-8.so.0.7.1
7fc5b3940000-7fc5b3941000 rw-p 00083000 fd:00 17718                      /usr/lib64/libpcre2-8.so.0.7.1
7fc5b3941000-7fc5b3960000 r-xp 00000000 fd:00 86666                      /usr/lib64/libbrotlicommon.so.1.0.6
7fc5b3960000-7fc5b3b60000 ---p 0001f000 fd:00 86666                      /usr/lib64/libbrotlicommon.so.1.0.6
7fc5b3b60000-7fc5b3b61000 r--p 0001f000 fd:00 86666                      /usr/lib64/libbrotlicommon.so.1.0.6
7fc5b3b61000-7fc5b3b62000 rw-p 00020000 fd:00 86666                      /usr/lib64/libbrotlicommon.so.1.0.6
7fc5b3b62000-7fc5b3cdf000 r-xp 00000000 fd:00 69133                      /usr/lib64/libunistring.so.2.1.0
7fc5b3cdf000-7fc5b3ede000 ---p 0017d000 fd:00 69133                      /usr/lib64/libunistring.so.2.1.0
7fc5b3ede000-7fc5b3ee2000 r--p 0017c000 fd:00 69133                      /usr/lib64/libunistring.so.2.1.0
7fc5b3ee2000-7fc5b3ee3000 rw-p 00180000 fd:00 69133                      /usr/lib64/libunistring.so.2.1.0
7fc5b3ee3000-7fc5b3f03000 r-xp 00000000 fd:00 25736                      /usr/lib64/libcrypt.so.1.1.0
7fc5b3f03000-7fc5b4102000 ---p 00020000 fd:00 25736                      /usr/lib64/libcrypt.so.1.1.0
7fc5b4102000-7fc5b4103000 r--p 0001f000 fd:00 25736                      /usr/lib64/libcrypt.so.1.1.0
7fc5b4103000-7fc5b410c000 rw-p 00000000 00:00 0
7fc5b410c000-7fc5b4133000 r-xp 00000000 fd:00 17726                      /usr/lib64/libselinux.so.1
7fc5b4133000-7fc5b4332000 ---p 00027000 fd:00 17726                      /usr/lib64/libselinux.so.1
7fc5b4332000-7fc5b4333000 r--p 00026000 fd:00 17726                      /usr/lib64/libselinux.so.1
7fc5b4333000-7fc5b4334000 rw-p 00027000 fd:00 17726                      /usr/lib64/libselinux.so.1
7fc5b4334000-7fc5b4336000 rw-p 00000000 00:00 0
7fc5b4336000-7fc5b4341000 r-xp 00000000 fd:00 86668                      /usr/lib64/libbrotlidec.so.1.0.6
7fc5b4341000-7fc5b4541000 ---p 0000b000 fd:00 86668                      /usr/lib64/libbrotlidec.so.1.0.6
7fc5b4541000-7fc5b4542000 r--p 0000b000 fd:00 86668                      /usr/lib64/libbrotlidec.so.1.0.6
7fc5b4542000-7fc5b4543000 rw-p 00000000 00:00 0
7fc5b4543000-7fc5b4590000 r-xp 00000000 fd:00 91514                      /usr/lib64/libldap-2.4.so.2.10.9
7fc5b4590000-7fc5b478f000 ---p 0004d000 fd:00 91514                      /usr/lib64/libldap-2.4.so.2.10.9
7fc5b478f000-7fc5b4791000 r--p 0004c000 fd:00 91514                      /usr/lib64/libldap-2.4.so.2.10.9
7fc5b4791000-7fc5b4792000 rw-p 0004e000 fd:00 91514                      /usr/lib64/libldap-2.4.so.2.10.9
7fc5b4792000-7fc5b47a2000 r-xp 00000000 fd:00 86498                      /usr/lib64/libpsl.so.5.3.1
7fc5b47a2000-7fc5b49a1000 ---p 00010000 fd:00 86498                      /usr/lib64/libpsl.so.5.3.1
7fc5b49a1000-7fc5b49a2000 r--p 0000f000 fd:00 86498                      /usr/lib64/libpsl.so.5.3.1
7fc5b49a2000-7fc5b49a3000 rw-p 00000000 00:00 0
7fc5b49a3000-7fc5b4a10000 r-xp 00000000 fd:00 415686                     /usr/lib64/libssh.so.4.8.7
7fc5b4a10000-7fc5b4c0f000 ---p 0006d000 fd:00 415686                     /usr/lib64/libssh.so.4.8.7
7fc5b4c0f000-7fc5b4c11000 r--p 0006c000 fd:00 415686                     /usr/lib64/libssh.so.4.8.7
7fc5b4c11000-7fc5b4c13000 rw-p 0006e000 fd:00 415686                     /usr/lib64/libssh.so.4.8.7
7fc5b4c13000-7fc5b4c2f000 r-xp 00000000 fd:00 69135                      /usr/lib64/libidn2.so.0.3.6
7fc5b4c2f000-7fc5b4e2f000 ---p 0001c000 fd:00 69135                      /usr/lib64/libidn2.so.0.3.6
7fc5b4e2f000-7fc5b4e30000 r--p 0001c000 fd:00 69135                      /usr/lib64/libidn2.so.0.3.6
7fc5b4e30000-7fc5b4e31000 rw-p 00000000 00:00 0
7fc5b4e31000-7fc5b4e56000 r-xp 00000000 fd:00 86697                      /usr/lib64/libnghttp2.so.14.17.0
7fc5b4e56000-7fc5b5055000 ---p 00025000 fd:00 86697                      /usr/lib64/libnghttp2.so.14.17.0
7fc5b5055000-7fc5b5056000 r--p 00024000 fd:00 86697                      /usr/lib64/libnghttp2.so.14.17.0
7fc5b5056000-7fc5b5058000 rw-p 00025000 fd:00 86697                      /usr/lib64/libnghttp2.so.14.17.0
7fc5b5058000-7fc5b5089000 r-xp 00000000 fd:00 435417                     /usr/lib64/libtirpc.so.3.0.0
7fc5b5089000-7fc5b5288000 ---p 00031000 fd:00 435417                     /usr/lib64/libtirpc.so.3.0.0
7fc5b5288000-7fc5b528a000 r--p 00030000 fd:00 435417                     /usr/lib64/libtirpc.so.3.0.0
7fc5b528a000-7fc5b528b000 rw-p 00032000 fd:00 435417                     /usr/lib64/libtirpc.so.3.0.0
7fc5b528b000-7fc5b52a7000 r-xp 00000000 fd:00 76464                      /usr/lib64/libsasl2.so.3.0.0
7fc5b52a7000-7fc5b54a7000 ---p 0001c000 fd:00 76464                      /usr/lib64/libsasl2.so.3.0.0
7fc5b54a7000-7fc5b54a8000 r--p 0001c000 fd:00 76464                      /usr/lib64/libsasl2.so.3.0.0
7fc5b54a8000-7fc5b54a9000 rw-p 0001d000 fd:00 76464                      /usr/lib64/libsasl2.so.3.0.0
7fc5b54a9000-7fc5b54b7000 r-xp 00000000 fd:00 91512                      /usr/lib64/liblber-2.4.so.2.10.9
7fc5b54b7000-7fc5b56b7000 ---p 0000e000 fd:00 91512                      /usr/lib64/liblber-2.4.so.2.10.9
7fc5b56b7000-7fc5b56b8000 r--p 0000e000 fd:00 91512                      /usr/lib64/liblber-2.4.so.2.10.9
7fc5b56b8000-7fc5b56b9000 rw-p 0000f000 fd:00 91512                      /usr/lib64/liblber-2.4.so.2.10.9
7fc5b56b9000-7fc5b56cd000 r-xp 00000000 fd:00 25492                      /usr/lib64/libresolv-2.28.so
7fc5b56cd000-7fc5b58cd000 ---p 00014000 fd:00 25492                      /usr/lib64/libresolv-2.28.so
7fc5b58cd000-7fc5b58ce000 r--p 00014000 fd:00 25492                      /usr/lib64/libresolv-2.28.so
7fc5b58ce000-7fc5b58cf000 rw-p 00015000 fd:00 25492                      /usr/lib64/libresolv-2.28.so
7fc5b58cf000-7fc5b58d1000 rw-p 00000000 00:00 0
7fc5b58d1000-7fc5b58d4000 r-xp 00000000 fd:00 69289                      /usr/lib64/libkeyutils.so.1.6
7fc5b58d4000-7fc5b5ad3000 ---p 00003000 fd:00 69289                      /usr/lib64/libkeyutils.so.1.6
7fc5b5ad3000-7fc5b5ad4000 r--p 00002000 fd:00 69289                      /usr/lib64/libkeyutils.so.1.6
7fc5b5ad4000-7fc5b5ad5000 rw-p 00000000 00:00 0
7fc5b5ad5000-7fc5b5ae4000 r-xp 00000000 fd:00 48442                      /usr/lib64/libkrb5support.so.0.1
7fc5b5ae4000-7fc5b5ce4000 ---p 0000f000 fd:00 48442                      /usr/lib64/libkrb5support.so.0.1
7fc5b5ce4000-7fc5b5ce5000 r--p 0000f000 fd:00 48442                      /usr/lib64/libkrb5support.so.0.1
7fc5b5ce5000-7fc5b5ce6000 rw-p 00010000 fd:00 48442                      /usr/lib64/libkrb5support.so.0.1
7fc5b5ce6000-7fc5b5ce9000 r-xp 00000000 fd:00 48426                      /usr/lib64/libcom_err.so.2.1
7fc5b5ce9000-7fc5b5ee8000 ---p 00003000 fd:00 48426                      /usr/lib64/libcom_err.so.2.1
7fc5b5ee8000-7fc5b5ee9000 r--p 00002000 fd:00 48426                      /usr/lib64/libcom_err.so.2.1
7fc5b5ee9000-7fc5b5eea000 rw-p 00003000 fd:00 48426                      /usr/lib64/libcom_err.so.2.1
7fc5b5eea000-7fc5b5eff000 r-xp 00000000 fd:00 48434                      /usr/lib64/libk5crypto.so.3.1
7fc5b5eff000-7fc5b60fe000 ---p 00015000 fd:00 48434                      /usr/lib64/libk5crypto.so.3.1
7fc5b60fe000-7fc5b6100000 r--p 00014000 fd:00 48434                      /usr/lib64/libk5crypto.so.3.1
7fc5b6100000-7fc5b6101000 rw-p 00016000 fd:00 48434                      /usr/lib64/libk5crypto.so.3.1
7fc5b6101000-7fc5b61db000 r-xp 00000000 fd:00 48440                      /usr/lib64/libkrb5.so.3.3
7fc5b61db000-7fc5b63db000 ---p 000da000 fd:00 48440                      /usr/lib64/libkrb5.so.3.3
7fc5b63db000-7fc5b63ea000 r--p 000da000 fd:00 48440                      /usr/lib64/libkrb5.so.3.3
7fc5b63ea000-7fc5b63ec000 rw-p 000e9000 fd:00 48440                      /usr/lib64/libkrb5.so.3.3
7fc5b63ec000-7fc5b63f1000 r-xp 00000000 fd:00 25810                      /usr/lib64/libcap-ng.so.0.0.0
7fc5b63f1000-7fc5b65f0000 ---p 00005000 fd:00 25810                      /usr/lib64/libcap-ng.so.0.0.0
7fc5b65f0000-7fc5b65f1000 r--p 00004000 fd:00 25810                      /usr/lib64/libcap-ng.so.0.0.0
7fc5b65f1000-7fc5b65f2000 rw-p 00005000 fd:00 25810                      /usr/lib64/libcap-ng.so.0.0.0
7fc5b65f2000-7fc5b667d000 r-xp 00000000 fd:00 433278                     /usr/lib64/libcurl.so.4.5.0
7fc5b667d000-7fc5b687d000 ---p 0008b000 fd:00 433278                     /usr/lib64/libcurl.so.4.5.0
7fc5b687d000-7fc5b6880000 r--p 0008b000 fd:00 433278                     /usr/lib64/libcurl.so.4.5.0
7fc5b6880000-7fc5b6881000 rw-p 0008e000 fd:00 433278                     /usr/lib64/libcurl.so.4.5.0
7fc5b6881000-7fc5b6897000 r-xp 00000000 fd:00 435419                     /usr/lib64/libnsl.so.2.0.0
7fc5b6897000-7fc5b6a97000 ---p 00016000 fd:00 435419                     /usr/lib64/libnsl.so.2.0.0
7fc5b6a97000-7fc5b6a98000 r--p 00016000 fd:00 435419                     /usr/lib64/libnsl.so.2.0.0
7fc5b6a98000-7fc5b6a9b000 rw-p 00000000 00:00 0
7fc5b6a9b000-7fc5b6ab1000 r-xp 00000000 fd:00 25681                      /usr/lib64/libz.so.1.2.11
7fc5b6ab1000-7fc5b6cb1000 ---p 00016000 fd:00 25681                      /usr/lib64/libz.so.1.2.11
7fc5b6cb1000-7fc5b6cb2000 r--p 00016000 fd:00 25681                      /usr/lib64/libz.so.1.2.11
7fc5b6cb2000-7fc5b6cb3000 rw-p 00000000 00:00 0
7fc5b6cb3000-7fc5b6cba000 r-xp 00000000 fd:00 25494                      /usr/lib64/librt-2.28.so
7fc5b6cba000-7fc5b6eb9000 ---p 00007000 fd:00 25494                      /usr/lib64/librt-2.28.so
7fc5b6eb9000-7fc5b6eba000 r--p 00006000 fd:00 25494                      /usr/lib64/librt-2.28.so
7fc5b6eba000-7fc5b6ebb000 rw-p 00007000 fd:00 25494                      /usr/lib64/librt-2.28.so
7fc5b6ebb000-7fc5b6f0e000 r-xp 00000000 fd:00 91516                      /usr/lib64/libldap_r-2.4.so.2.10.9
7fc5b6f0e000-7fc5b710e000 ---p 00053000 fd:00 91516                      /usr/lib64/libldap_r-2.4.so.2.10.9
7fc5b710e000-7fc5b7110000 r--p 00053000 fd:00 91516                      /usr/lib64/libldap_r-2.4.so.2.10.9
7fc5b7110000-7fc5b7111000 rw-p 00055000 fd:00 91516                      /usr/lib64/libldap_r-2.4.so.2.10.9
7fc5b7111000-7fc5b7113000 rw-p 00000000 00:00 0
7fc5b7113000-7fc5b7165000 r-xp 00000000 fd:00 48430                      /usr/lib64/libgssapi_krb5.so.2.2
7fc5b7165000-7fc5b7365000 ---p 00052000 fd:00 48430                      /usr/lib64/libgssapi_krb5.so.2.2
7fc5b7365000-7fc5b7367000 r--p 00052000 fd:00 48430                      /usr/lib64/libgssapi_krb5.so.2.2
7fc5b7367000-7fc5b7368000 rw-p 00054000 fd:00 48430                      /usr/lib64/libgssapi_krb5.so.2.2
7fc5b7368000-7fc5b736b000 r-xp 00000000 fd:00 25478                      /usr/lib64/libdl-2.28.so
7fc5b736b000-7fc5b756a000 ---p 00003000 fd:00 25478                      /usr/lib64/libdl-2.28.so
7fc5b756a000-7fc5b756b000 r--p 00002000 fd:00 25478                      /usr/lib64/libdl-2.28.so
7fc5b756b000-7fc5b756c000 rw-p 00003000 fd:00 25478                      /usr/lib64/libdl-2.28.so
7fc5b756c000-7fc5b7589000 r-xp 00000000 fd:00 25812                      /usr/lib64/libaudit.so.1.0.0
7fc5b7589000-7fc5b7788000 ---p 0001d000 fd:00 25812                      /usr/lib64/libaudit.so.1.0.0
7fc5b7788000-7fc5b7789000 r--p 0001c000 fd:00 25812                      /usr/lib64/libaudit.so.1.0.0
7fc5b7789000-7fc5b778a000 rw-p 0001d000 fd:00 25812                      /usr/lib64/libaudit.so.1.0.0
7fc5b778a000-7fc5b7796000 rw-p 00000000 00:00 0
7fc5b7796000-7fc5b77cf000 r-xp 00000000 fd:00 25749                      /usr/lib64/libexpat.so.1.6.7
7fc5b77cf000-7fc5b79cf000 ---p 00039000 fd:00 25749                      /usr/lib64/libexpat.so.1.6.7
7fc5b79cf000-7fc5b79d1000 r--p 00039000 fd:00 25749                      /usr/lib64/libexpat.so.1.6.7
7fc5b79d1000-7fc5b79d2000 rw-p 0003b000 fd:00 25749                      /usr/lib64/libexpat.so.1.6.7
7fc5b79d2000-7fc5b7b9f000 r-xp 00000000 fd:00 25476                      /usr/lib64/libc-2.28.so
7fc5b7b9f000-7fc5b7d9f000 ---p 001cd000 fd:00 25476                      /usr/lib64/libc-2.28.so
7fc5b7d9f000-7fc5b7da3000 r--p 001cd000 fd:00 25476                      /usr/lib64/libc-2.28.so
7fc5b7da3000-7fc5b7da5000 rw-p 001d1000 fd:00 25476                      /usr/lib64/libc-2.28.so
7fc5b7da5000-7fc5b7da9000 rw-p 00000000 00:00 0
7fc5b7da9000-7fc5b7dc0000 r-xp 00000000 fd:00 91794                      /usr/lib64/libgcc_s-8-20210514.so.1
7fc5b7dc0000-7fc5b7fbf000 ---p 00017000 fd:00 91794                      /usr/lib64/libgcc_s-8-20210514.so.1
7fc5b7fbf000-7fc5b7fc0000 r--p 00016000 fd:00 91794                      /usr/lib64/libgcc_s-8-20210514.so.1
7fc5b7fc0000-7fc5b7fc1000 rw-p 00017000 fd:00 91794                      /usr/lib64/libgcc_s-8-20210514.so.1
7fc5b7fc1000-7fc5b8142000 r-xp 00000000 fd:00 25480                      /usr/lib64/libm-2.28.so
7fc5b8142000-7fc5b8341000 ---p 00181000 fd:00 25480                      /usr/lib64/libm-2.28.so
7fc5b8341000-7fc5b8342000 r--p 00180000 fd:00 25480                      /usr/lib64/libm-2.28.so
7fc5b8342000-7fc5b8343000 rw-p 00181000 fd:00 25480                      /usr/lib64/libm-2.28.so
7fc5b8343000-7fc5b84c9000 r-xp 00000000 fd:00 25740                      /usr/lib64/libstdc++.so.6.0.25
7fc5b84c9000-7fc5b86c8000 ---p 00186000 fd:00 25740                      /usr/lib64/libstdc++.so.6.0.25
7fc5b86c8000-7fc5b86d4000 r--p 00185000 fd:00 25740                      /usr/lib64/libstdc++.so.6.0.25
7fc5b86d4000-7fc5b86d5000 rw-p 00191000 fd:00 25740                      /usr/lib64/libstdc++.so.6.0.25
7fc5b86d5000-7fc5b86d8000 rw-p 00000000 00:00 0
7fc5b86d8000-7fc5b87ca000 r-xp 00000000 fd:00 478203                     /usr/lib64/libxml-security-c.so.20.0.2
7fc5b87ca000-7fc5b89ca000 ---p 000f2000 fd:00 478203                     /usr/lib64/libxml-security-c.so.20.0.2
7fc5b89ca000-7fc5b89d1000 r--p 000f2000 fd:00 478203                     /usr/lib64/libxml-security-c.so.20.0.2
7fc5b89d1000-7fc5b89d4000 rw-p 000f9000 fd:00 478203                     /usr/lib64/libxml-security-c.so.20.0.2
7fc5b89d4000-7fc5b8e2d000 r-xp 00000000 fd:00 478199                     /usr/lib64/libxerces-c-3.2.so
7fc5b8e2d000-7fc5b902c000 ---p 00459000 fd:00 478199                     /usr/lib64/libxerces-c-3.2.so
7fc5b902c000-7fc5b9043000 r--p 00458000 fd:00 478199                     /usr/lib64/libxerces-c-3.2.so
7fc5b9043000-7fc5b9070000 rw-p 0046f000 fd:00 478199                     /usr/lib64/libxerces-c-3.2.so
7fc5b9070000-7fc5b9326000 r-xp 00000000 fd:00 48422                      /usr/lib64/libcrypto.so.1.1.1k
7fc5b9326000-7fc5b9526000 ---p 002b6000 fd:00 48422                      /usr/lib64/libcrypto.so.1.1.1k
7fc5b9526000-7fc5b9552000 r--p 002b6000 fd:00 48422                      /usr/lib64/libcrypto.so.1.1.1k
7fc5b9552000-7fc5b9556000 rw-p 002e2000 fd:00 48422                      /usr/lib64/libcrypto.so.1.1.1k
7fc5b9556000-7fc5b955b000 rw-p 00000000 00:00 0
7fc5b955b000-7fc5b95e2000 r-xp 00000000 fd:00 48424                      /usr/lib64/libssl.so.1.1.1k
7fc5b95e2000-7fc5b97e2000 ---p 00087000 fd:00 48424                      /usr/lib64/libssl.so.1.1.1k
7fc5b97e2000-7fc5b97eb000 r--p 00087000 fd:00 48424                      /usr/lib64/libssl.so.1.1.1k
7fc5b97eb000-7fc5b97ef000 rw-p 00090000 fd:00 48424                      /usr/lib64/libssl.so.1.1.1k
7fc5b97ef000-7fc5b97f0000 rw-p 00000000 00:00 0
7fc5b97f0000-7fc5b980b000 r-xp 00000000 fd:00 25607                      /usr/lib64/libpthread-2.28.so
7fc5b980b000-7fc5b9a0a000 ---p 0001b000 fd:00 25607                      /usr/lib64/libpthread-2.28.so
7fc5b9a0a000-7fc5b9a0b000 r--p 0001a000 fd:00 25607                      /usr/lib64/libpthread-2.28.so
7fc5b9a0b000-7fc5b9a0c000 rw-p 0001b000 fd:00 25607                      /usr/lib64/libpthread-2.28.so
7fc5b9a0c000-7fc5b9a10000 rw-p 00000000 00:00 0
7fc5b9a10000-7fc5b9a29000 r-xp 00000000 fd:00 91587                      /usr/lib64/libACE_SSL-7.0.so
7fc5b9a29000-7fc5b9c28000 ---p 00019000 fd:00 91587                      /usr/lib64/libACE_SSL-7.0.so
7fc5b9c28000-7fc5b9c2a000 r--p 00018000 fd:00 91587                      /usr/lib64/libACE_SSL-7.0.so
7fc5b9c2a000-7fc5b9c2b000 rw-p 0001a000 fd:00 91587                      /usr/lib64/libACE_SSL-7.0.so
7fc5b9c2b000-7fc5b9db2000 r-xp 00000000 fd:00 91586                      /usr/lib64/libACE-7.0.so
7fc5b9db2000-7fc5b9fb1000 ---p 00187000 fd:00 91586                      /usr/lib64/libACE-7.0.so
7fc5b9fb1000-7fc5b9fc1000 r--p 00186000 fd:00 91586                      /usr/lib64/libACE-7.0.so
7fc5b9fc1000-7fc5b9fc5000 rw-p 00196000 fd:00 91586                      /usr/lib64/libACE-7.0.so
7fc5b9fc5000-7fc5b9fc7000 rw-p 00000000 00:00 0
7fc5b9fc7000-7fc5ba015000 r-xp 00000000 fd:00 91788                      /usr/lib64/libpq.so.5.13
7fc5ba015000-7fc5ba215000 ---p 0004e000 fd:00 91788                      /usr/lib64/libpq.so.5.13
7fc5ba215000-7fc5ba218000 r--p 0004e000 fd:00 91788                      /usr/lib64/libpq.so.5.13
7fc5ba218000-7fc5ba219000 rw-p 00051000 fd:00 91788                      /usr/lib64/libpq.so.5.13
7fc5ba219000-7fc5ba23d000 r-xp 00000000 fd:00 91810                      /usr/lib64/libAESutil.so
7fc5ba23d000-7fc5ba43d000 ---p 00024000 fd:00 91810                      /usr/lib64/libAESutil.so
7fc5ba43d000-7fc5ba43e000 r--p 00024000 fd:00 91810                      /usr/lib64/libAESutil.so
7fc5ba43e000-7fc5ba43f000 rw-p 00025000 fd:00 91810                      /usr/lib64/libAESutil.so
7fc5ba43f000-7fc5ba44d000 r-xp 00000000 fd:00 64655                      /usr/lib64/libpam.so.0.84.2
7fc5ba44d000-7fc5ba64d000 ---p 0000e000 fd:00 64655                      /usr/lib64/libpam.so.0.84.2
7fc5ba64d000-7fc5ba64e000 r--p 0000e000 fd:00 64655                      /usr/lib64/libpam.so.0.84.2
7fc5ba64e000-7fc5ba64f000 rw-p 0000f000 fd:00 64655                      /usr/lib64/libpam.so.0.84.2
7fc5ba64f000-7fc5ba677000 r-xp 00000000 fd:00 478194                     /usr/lib64/libtsmmr.so
7fc5ba677000-7fc5ba876000 ---p 00028000 fd:00 478194                     /usr/lib64/libtsmmr.so
7fc5ba876000-7fc5ba877000 r--p 00027000 fd:00 478194                     /usr/lib64/libtsmmr.so
7fc5ba877000-7fc5ba879000 rw-p 00028000 fd:00 478194                     /usr/lib64/libtsmmr.so
7fc5ba879000-7fc5ba87b000 r-xp 00000000 fd:00 478195                     /usr/lib64/libtsoam.so
7fc5ba87b000-7fc5baa7a000 ---p 00002000 fd:00 478195                     /usr/lib64/libtsoam.so
7fc5baa7a000-7fc5baa7b000 r--p 00001000 fd:00 478195                     /usr/lib64/libtsoam.so
7fc5baa7b000-7fc5baa7c000 rw-p 00002000 fd:00 478195                     /usr/lib64/libtsoam.so
7fc5baa7c000-7fc5baa8e000 r-xp 00000000 fd:00 478188                     /usr/lib64/liblogerr.so
7fc5baa8e000-7fc5bac8e000 ---p 00012000 fd:00 478188                     /usr/lib64/liblogerr.so
7fc5bac8e000-7fc5bac8f000 r--p 00012000 fd:00 478188                     /usr/lib64/liblogerr.so
7fc5bac8f000-7fc5bac90000 rw-p 00013000 fd:00 478188                     /usr/lib64/liblogerr.so
7fc5bac90000-7fc5bac93000 rw-p 00000000 00:00 0
7fc5bac93000-7fc5bacf0000 r-xp 00000000 fd:00 91818                      /usr/lib64/libasai.so
7fc5bacf0000-7fc5baeef000 ---p 0005d000 fd:00 91818                      /usr/lib64/libasai.so
7fc5baeef000-7fc5baef0000 r--p 0005c000 fd:00 91818                      /usr/lib64/libasai.so
7fc5baef0000-7fc5baef8000 rw-p 0005d000 fd:00 91818                      /usr/lib64/libasai.so
7fc5baef8000-7fc5baf00000 rw-p 00000000 00:00 0
7fc5baf00000-7fc5baf04000 r-xp 00000000 fd:00 478192                     /usr/lib64/libtdi.so
7fc5baf04000-7fc5bb103000 ---p 00004000 fd:00 478192                     /usr/lib64/libtdi.so
7fc5bb103000-7fc5bb104000 r--p 00003000 fd:00 478192                     /usr/lib64/libtdi.so
7fc5bb104000-7fc5bb105000 rw-p 00004000 fd:00 478192                     /usr/lib64/libtdi.so
7fc5bb105000-7fc5bb11e000 r-xp 00000000 fd:00 91511                      /usr/lib64/libattpriv.so
7fc5bb11e000-7fc5bb31e000 ---p 00019000 fd:00 91511                      /usr/lib64/libattpriv.so
7fc5bb31e000-7fc5bb329000 r--p 00019000 fd:00 91511                      /usr/lib64/libattpriv.so
7fc5bb329000-7fc5bb32a000 rw-p 00024000 fd:00 91511                      /usr/lib64/libattpriv.so
7fc5bb32a000-7fc5bb48c000 r-xp 00000000 fd:00 91520                      /usr/lib64/libcsta.so
7fc5bb48c000-7fc5bb68b000 ---p 00162000 fd:00 91520                      /usr/lib64/libcsta.so
7fc5bb68b000-7fc5bb6b6000 r--p 00161000 fd:00 91520                      /usr/lib64/libcsta.so
7fc5bb6b6000-7fc5bb6b9000 rw-p 0018c000 fd:00 91520                      /usr/lib64/libcsta.so
7fc5bb6b9000-7fc5bb6d1000 rw-p 00000000 00:00 0
7fc5bb6d1000-7fc5bb82c000 r-xp 00000000 fd:00 91510                      /usr/lib64/libasn1code.so
7fc5bb82c000-7fc5bba2c000 ---p 0015b000 fd:00 91510                      /usr/lib64/libasn1code.so
7fc5bba2c000-7fc5bba32000 rw-p 0015b000 fd:00 91510                      /usr/lib64/libasn1code.so
7fc5bba32000-7fc5bba61000 r-xp 00000000 fd:00 48406                      /usr/lib64/ld-2.28.so
7fc5bbaa3000-7fc5bbaa9000 rw-s 00000000 fd:00 25603593                   /opt/mvap/conf/mapLic
7fc5bbaa9000-7fc5bbaae000 rw-s 00000000 00:16 153457                     /dev/shm/mvap/tsapi/closedClientConnections
7fc5bbaae000-7fc5bbc58000 rw-p 00000000 00:00 0
7fc5bbc58000-7fc5bbc59000 rw-s 00000000 00:16 153455                     /dev/shm/mvap/tsapi/tlinkStatus
7fc5bbc59000-7fc5bbc5a000 rw-s 00000000 00:16 153454                     /dev/shm/mvap/tsapi/serviceStatus
7fc5bbc5a000-7fc5bbc61000 r--s 00000000 fd:00 16805748                   /usr/lib64/gconv/gconv-modules.cache
7fc5bbc61000-7fc5bbc62000 r--p 0002f000 fd:00 48406                      /usr/lib64/ld-2.28.so
7fc5bbc62000-7fc5bbc64000 rw-p 00030000 fd:00 48406                      /usr/lib64/ld-2.28.so
7ffcf3895000-7ffcf38b7000 rw-p 00000000 00:00 0                          [stack]
7ffcf38d9000-7ffcf38dd000 r--p 00000000 00:00 0                          [vvar]
7ffcf38dd000-7ffcf38df000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
```

Here's the breakdown of the memory mapping

## Column explained

| Column | Example | Meaning |
| --- | --- | --- |
| Address Range | 00400000-00488000 | The virtual memory addresses the process is using.|
| Permissions | r-xp | <b>r</b>ead, <b>w</b>rite, e<b>x</b>ecute, <b>p</b>rivate (or shared).|
| Offset | 00000000 | Where in the file this mapping starts |
| Device | fd:00 | The hard drive device ID containing the file.|
| Inode | 8818458 | The specific file ID on the hard drive (0 = RAM only). |
| Pathname | /opt/.../tsrv | The file mapped, or a special tag like <b>[heap]</b>

- The Main Executable — /opt/mvap/bin/tsrv
```
00400000-00488000 r-xp  → .text  (executable code, ~544 KB)
00687000-0069b000 r--p  → .rodata (read-only data, constants)
0069b000-006a1000 rw-p  → .data/.bss (global variables)
006a1000-006af000 rw-p  → anonymous (BSS overflow / extra writable data)
```
