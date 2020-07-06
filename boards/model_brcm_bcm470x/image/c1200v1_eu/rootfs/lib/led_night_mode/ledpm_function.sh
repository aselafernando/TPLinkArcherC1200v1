. /lib/functions.sh

ledpm_cli()
{
    [ -z "$(which ledcli)" ] && return
    ledcli "$@"
}

ledpm_wifi()
{
    [ -z "$(which wifi)" ] && return
    wifi led "$@"
}

# HH:MM:SS
ledpm_date()
{
    echo `date|grep -Eo "([0-9]{2}:){2}[0-9]{2}"`
}

ledpm_hour()
{
    echo `echo "$1"|cut -d ":" -f 1`
}

ledpm_mintue()
{
    echo `echo "$1"|cut -d ":" -f 2`
}

# $1 - hour $2 - minute
ledpm_date_to_min()
{
    local h=${1#0}
    local m=${2#0}

    echo "$((h * 60 + m))"
}

ledpm_set_ledctrl_config()
{
    uci set ledctrl.NIGHT.${1}="$2"
}

