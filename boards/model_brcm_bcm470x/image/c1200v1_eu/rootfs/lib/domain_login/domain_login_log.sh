dlogin_MOD_ID=215

dlogin_syslog()
{
    local log_id=$1
    shift
    logx -p $$ $dlogin_MOD_ID $log_id "$@"
}

