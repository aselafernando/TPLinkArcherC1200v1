--[[
Copyright(c) 2015 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  subprocess.lua
Details :  repleace sys.call func
Author  :  unknown (from heye@tp-link.net)
Version :  unknown
Date    :  25Aug, 2015
]]--
module("luci.model.subprocess", package.seeall)

function call(command)
    local pid = nixio.fork()
    if pid == 0 then
       -- patch stdin, out, err to /dev/null
       local null = nixio.open("/dev/null", "w+")
       if null then
           nixio.dup(null, nixio.stderr)
           nixio.dup(null, nixio.stdout)
           nixio.dup(null, nixio.stdin)
           if null:fileno() > 2 then
               null:close()
           end
        end

        -- replace with target command
        return nixio.execp(unpack(command))
    elseif pid > 0 then
        local _, stat, code = nixio.waitpid(pid)
        if stat == "exited" then
            return code
        else
            return nil
        end
    end
end