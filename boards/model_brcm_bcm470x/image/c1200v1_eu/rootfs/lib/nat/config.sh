# Copyright(c) 2011-2013 Shenzhen TP-LINK Technologies Co.Ltd.
# file     config.sh
# brief    
# author   Guo Dongxian
# version  1.0.0
# date     26Feb14
# history   arg 1.0.0, 26Feb14, Guo Dongxian, Create the file

nat_config_append() {
	CONFIG_APPEND=1 config_load "$@"
	unset CONFIG_APPEND
}

nat_config_once() {
	local func=$1
	local type=$2
	shift 2

	local config=cfg00nil
	nat_config__once() {
		config=$1
	}
	config_foreach nat_config__once "$type"

	$func $config "$@"
}

nat_config_get_section() {
	local config=$1
	local prefix=$2
	shift 2

	[ -n "$config" ] || return 1
	[ -n "$prefix" ] && {
        prefix="${prefix}_"
        export ${NO_EXPORT:+-n} -- "${prefix}NAME"="${config}"
        config_get "${prefix}TYPE" "$config" TYPE
	}

	local enabled
	config_get_bool enabled "$config" enabled 1
	[ $enabled -eq 1 ] || return 1

	[ "$1" == '{' ] && shift
	while [ $# -ge 3 ]; do
        local type=$1
        local name=$2
        local dflt=$3
        shift 3
        # TODO: Move handling of defaults to /lib/functions.sh
        # and get replace the case block with the following 
        # two lines:
        # type=${type#string}
        # config_get${type:+_${type}} "${prefix}${name}" "$config" "$name" "$dflt" || return
        case "$type" in
            string)
                local tmp
                config_get tmp "$config" "$name" || return
                [ -z "$tmp" ] && tmp=$dflt
                export ${NO_EXPORT:+-n} -- "${prefix}${name}=${tmp}"
                continue
            ;;
            boolean)
                type=bool
            ;;
        esac;

        local cmd=${prefix}config_get_${type}
        type $cmd > /dev/null || {
                cmd=config_get_${type} 
        }
        type $cmd > /dev/null || {
                echo "config type $type (for $name) not supported" >&2
                return 1
        }
        $cmd "${prefix}${name}" "$config" "$name" "$dflt" || return
	done
}

