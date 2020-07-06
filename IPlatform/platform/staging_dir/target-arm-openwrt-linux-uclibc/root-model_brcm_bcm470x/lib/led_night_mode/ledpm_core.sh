
ledpm_start()
{      
    /sbin/ledpm &
}

ledpm_stop()
{
    [ -e /var/run/ledpm.pid ] && {
        local pid=$(cat /var/run/ledpm.pid)
        [ -n "$pid" ] && kill $pid
        rm -f /var/run/ledpm.pid
    }
}

ledpm_restart()
{
    ledpm_stop
    ledpm_start
}

ledpm_reload()
{
    ledpm_start
}

