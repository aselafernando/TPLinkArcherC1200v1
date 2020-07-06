1.该版本pptpd使用了pptp.ko，相比原版pptpd速度有较大提升;
2.使用该版本时，在ipf.config中注释掉CONFIG_PACKAGE_pptpd=y, 添加CONFIG_PACKAGE_accel-pptpd=y
3.需要修改vpn.lua，否则无法发现已经建立的vpn连接。

Index: .
===================================================================
--- .	(revision 7913)
+++ .	(working copy)
@@ -91,7 +91,7 @@
     local input2 = nil
 
     for line in input:lines() do
-        pid, _, _, ipaddr, remoteip = line:match("^pid=(%d+)%s+.+%s+(%d+)%s+(%d+%.%d+%.%d+%.%d+):(%d+%.%d+%.%d+%.%d+)%s+.+%s+remotenumber%s+(%d+%.%d+%.%d+%.%d+)")
+        pid, _, _, ipaddr, remoteip = line:match("^pid=(%d+)%s+.+%s+(%d+)%s+(%d+%.%d+%.%d+%.%d+):(%d+%.%d+%.%d+%.%d+)%s+ipparam%s+(%d+%.%d+%.%d+%.%d+)")
         if pid then
             filename = "/tmp/ppp" .. pid
             file = io.open(filename, "r")
