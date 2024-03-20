## Description
- Following Brendan Galea's vulkan [tutorial](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR)
- Little Vulkan Engine

<div align="left"><img src="https://raw.githubusercontent.com/loop614/lvedemo/main/vase_moon_floor.png" width=560 height=315 alt="vase_moon_floor"/></div>

## Quick Start
```console
$ make demo
```
- wasdqe to move
- arrows to rotate

## Requirements
- [Here](https://vulkan-tutorial.com/Development_environment)

```console
$ ldd ./LveDemo
	linux-vdso.so.1 (0x00007ffe95be0000)
	libglfw.so.3 => /lib/x86_64-linux-gnu/libglfw.so.3 (0x00007fa539040000)
	libvulkan.so.1 => /lib/x86_64-linux-gnu/libvulkan.so.1 (0x00007fa538fd0000)
	libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fa538c00000)
	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fa538fb0000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fa538a1f000)
	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fa538ecf000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fa538eca000)
	libX11.so.6 => /lib/x86_64-linux-gnu/libX11.so.6 (0x00007fa5388dd000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fa538ec5000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fa5390f5000)
	libxcb.so.1 => /lib/x86_64-linux-gnu/libxcb.so.1 (0x00007fa538e9b000)
	libXau.so.6 => /lib/x86_64-linux-gnu/libXau.so.6 (0x00007fa538e94000)
	libXdmcp.so.6 => /lib/x86_64-linux-gnu/libXdmcp.so.6 (0x00007fa538600000)
	libbsd.so.0 => /lib/x86_64-linux-gnu/libbsd.so.0 (0x00007fa538e7e000)
	libmd.so.0 => /lib/x86_64-linux-gnu/libmd.so.0 (0x00007fa538e71000)
```
