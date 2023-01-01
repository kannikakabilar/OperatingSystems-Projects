# Ext2-Filesystem
The Ext2 Filesystem is a project that contains implementation of tools that modify ext2-format virtual disks. This program is written in C that operates on an ext2 formatted virtual disk. This assignment was done to demonstrate our understanding of the theory behind Filesystems as part of the Operating Systems course. The information about the files is stored using an inode data structure where it utilizes indexed access for file accessing methods. The information about directories and its collection of files are stored in structs. Information regarding the files are stored in its own struct designing after the File Control Block. <br/><br/>
The following features were implemented as tools that can be utilized to operate in this filesystem.<br/>
```md
> ext2_cp
> ext2_mkdir
> ext2_ln
> ext2_rm
> ext2_restore
> ext2_checker
> ext2_ls
> ext2_dump
> ext2_corruptor
```

