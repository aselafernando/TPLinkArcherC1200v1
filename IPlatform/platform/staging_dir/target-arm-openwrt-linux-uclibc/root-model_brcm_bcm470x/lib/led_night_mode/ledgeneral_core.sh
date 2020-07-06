
ledgeneral_start()
{      
    /sbin/ledgeneral &
}

ledgeneral_stop()
{
    local pid=$(cat /var/run/ledgeneral.pid)
    [ -n "$pid" ] && kill $pid
    rm -f /var/fun/ledgeneral.pid
}

ledgeneral_restart()
{
    ledgeneral_stop
    ledgeneral_start
}

ledgeneral_reload()
{
    ledgeneral_start
}